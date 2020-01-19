/*
	Copyright 2019 Kyle Johnston

	Determine State takes raw sensor data from Sensor Shared Memory, processes it, and outputs
	vehicle state to State Shared Memory
*/

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <memory>

#include <ctime>
#include <ratio>
#include <chrono>

#include "Common/Util.h"

#include "sharedMemoryStructs.h"

void transformImuToVehicleFrame(Acceleration* ImuFrame, Acceleration* VehFrame) {
	VehFrame->x = (0 - ImuFrame->y);
	VehFrame->y = (0 - ImuFrame->x);
	VehFrame->z = (0 - ImuFrame->z);
}

void subtractAccelerationAfromB(Acceleration* A, Acceleration* B) {
	B->x -= A->x;
	B->y -= A->y;
	B->z -= A->z;
}

int main()
{

	char sensorKeyPath[] = SENSOR_SHM_KEY;
	key_t sensorShmKey = ftok(sensorKeyPath, 65);
	int sensorShmId = shmget(sensorShmKey, sizeof(shmSensorStruct), 0666|SHM_RDONLY);
	shmSensorStruct* sensorSharedMemory = (shmSensorStruct*) shmat(sensorShmId, (void*)0, 0); 
  

	char stateKeyPath[] = STATE_SHM_KEY;
	key_t stateShmKey = ftok(stateKeyPath, 65);
	int stateShmId = shmget(stateShmKey, sizeof(shmStateStruct), 0666|IPC_CREAT);
	shmStateStruct* stateSharedMemory = new (shmat(stateShmId, (void*)0, 0)) shmStateStruct; 

	if (check_apm()) {
		return 1;
	}

	Acceleration runningAvgAcceleration;
	Acceleration accelerationOfGravity;
	Acceleration vehicleAccel;
	Acceleration tempAccel;
	imuData tempImu;
	
	int lastCount = 0;
	constexpr float startingAvgSamples = 100;
	//constexpr float startingWeightToExisting = (startingAvgSamples - 1) / startingAvgSamples;
	//constexpr float startingWeightToNew = 1 / startingAvgSamples;

	int sequenceCount = sensorSharedMemory->imu1.sequenceCount();

	for (int i = sequenceCount; sequenceCount < (i + (int) startingAvgSamples); NULL)
	{
		if (sequenceCount > lastCount) {
			//printf("Collected average sample %i\n", lastCount);
			runningAvgAcceleration.x += sensorSharedMemory->imu1.ax;
			runningAvgAcceleration.y += sensorSharedMemory->imu1.ay;
			runningAvgAcceleration.z += sensorSharedMemory->imu1.az;
			lastCount = sequenceCount;
		}
		sequenceCount = sensorSharedMemory->imu1.sequenceCount();
	}
	runningAvgAcceleration.x /= startingAvgSamples;
	runningAvgAcceleration.y /= startingAvgSamples;
	runningAvgAcceleration.z /= startingAvgSamples;

	accelerationOfGravity = runningAvgAcceleration;

	printf(
		"Acc Gravity: %+7.3f %+7.3f %+7.3f  \n",
		accelerationOfGravity.x,
		accelerationOfGravity.y,
		accelerationOfGravity.z
	);

	constexpr unsigned int desiredFrameRate = 1000;

	constexpr float avgSamples = desiredFrameRate / 50;
	constexpr float weightToExisting = (avgSamples - 1) / avgSamples;
	constexpr float weightToNew = 1 / avgSamples;

	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span;

	unsigned long i =0;
	unsigned int numberOfFrames = 1e7;

	while (true) {
		if (i % numberOfFrames == 0) {
			t2 = std::chrono::high_resolution_clock::now();

  			time_span = std::chrono::duration_cast< std::chrono::duration<double> >(t2 - t1);
  			float frameTime = (time_span.count() / numberOfFrames) * 1e6;
  			float frameRate = numberOfFrames / time_span.count();

  			std::cout << i << ": It took " << time_span.count() << " seconds to complete " << numberOfFrames 
  			          << " frames. (Expected 1.0 s) " << "Averaged " << frameTime << " us per frame, or "
  			          << frameRate << " Hz.\n";
			t1 = t2;
		}

		tempImu = sensorSharedMemory->imu1;

		sequenceCount = tempImu.sequenceCount();

		runningAvgAcceleration.x = ( runningAvgAcceleration.x * weightToExisting ) + ( tempImu.ax * weightToNew );
		runningAvgAcceleration.y = ( runningAvgAcceleration.y * weightToExisting ) + ( tempImu.ay * weightToNew );
		runningAvgAcceleration.z = ( runningAvgAcceleration.z * weightToExisting ) + ( tempImu.az * weightToNew );

		vehicleAccel = runningAvgAcceleration;
		/*printf(
				"Avg Acc:		  %+7.3f %+7.3f %+7.3f  \n",
				runningAvgAcceleration.x,
				runningAvgAcceleration.y,
				runningAvgAcceleration.z
			);
		*/
		subtractAccelerationAfromB(&accelerationOfGravity, &vehicleAccel);
		/*printf(
				"Acc Less Gravity: %+7.3f %+7.3f %+7.3f  \n",
				vehicleAccel.x,
				vehicleAccel.y,
				vehicleAccel.z
			);
		*/
		transformImuToVehicleFrame(&vehicleAccel, &(stateSharedMemory->linearAccel));
		/*printf(
				"Acc Veh Frame:	%+7.3f %+7.3f %+7.3f  \n",
				tempAccel.x,
				tempAccel.y,
				tempAccel.z
			);
		*/

		//stateSharedMemory->linearAccel = tempAccel;
	/*
		if (sequenceCount % 50 == 0) {
			printf(
				"Sequence Count: %i  ",
				tempImu.sequenceCount()
			);
			printf(
				"Acc: %+7.3f %+7.3f %+7.3f  ",
				tempImu.ax,
				tempImu.ay,
				tempImu.az
			);
			printf(
				"Avg Acc: %+7.3f %+7.3f %+7.3f  \n",
				runningAvgAcceleration.x,
				runningAvgAcceleration.y,
				runningAvgAcceleration.z
			);
			printf(
				"Gyr: %+8.3f %+8.3f %+8.3f  ",
				sensorSharedMemory->imu1.gx,
				sensorSharedMemory->imu1.gy,
				sensorSharedMemory->imu1.gz
			);
			printf(
				"Mag: %+7.3f %+7.3f %+7.3f\n",
				sensorSharedMemory->imu1.mx,
				sensorSharedMemory->imu1.my,
				sensorSharedMemory->imu1.mz
			);
		}
	*/

		i++;
		//usleep( (1e6 / desiredFrameRate) - 65);
	}

	delete stateSharedMemory;

	shmdt(sensorSharedMemory);
	shmdt(stateSharedMemory);

	shmctl(stateShmId, IPC_RMID, NULL);

	return 0;
}

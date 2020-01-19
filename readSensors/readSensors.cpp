/*
	Copyright 2019 Kyle Johnston

	Read Sensors populates Shared Sensor Memory with the readings from
*/

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <memory>

#include <ctime>
#include <ratio>
#include <chrono>

#include "Navio2/LSM9DS1.h"
#include "Common/Util.h"

#include "sharedMemoryStructs.h"

int main()
{
	LSM9DS1* LSM = new LSM9DS1();
	LSM->initialize();

	//float ax, ay, az;
	//float gx, gy, gz;
	//float mx, my, mz;

	char keyPath[] = SENSOR_SHM_KEY;
	key_t shmKey = ftok(keyPath, 65);
	int shmId = shmget(shmKey, sizeof(shmSensorStruct), 0666|IPC_CREAT);
	shmSensorStruct* sensorSharedMemory = new (shmat(shmId, (void*)0, 0)) shmSensorStruct; 
  
	sensorSharedMemory->imu1._sequenceCount = 0;

	if (check_apm()) {
		return 1;
	}

	std::chrono::high_resolution_clock::time_point t1;
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> time_span;

	constexpr unsigned int desiredFrameRate = 1000;

	unsigned int i =0;
	unsigned int numberOfFrames = 10000;

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

		LSM->update();
		LSM->read_accelerometer(
			&(sensorSharedMemory->imu1.ax),
			&(sensorSharedMemory->imu1.ay),
			&(sensorSharedMemory->imu1.az)
		);
		LSM->read_gyroscope(
			&(sensorSharedMemory->imu1.gx),
			&(sensorSharedMemory->imu1.gy),
			&(sensorSharedMemory->imu1.gz)
		);
		LSM->read_magnetometer(
			&(sensorSharedMemory->imu1.mx),
			&(sensorSharedMemory->imu1.my),
			&(sensorSharedMemory->imu1.mz)
		);
		sensorSharedMemory->imu1.incrementSequence();
		/*printf(
			"Sequence Count: %u  ",
			sensorSharedMemory->imu1.sequenceCount()
		);
		printf(
			"Acc: %+7.3f %+7.3f %+7.3f  \n",
			sensorSharedMemory->imu1.ax,
			sensorSharedMemory->imu1.ay,
			sensorSharedMemory->imu1.az
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
	*/
		i++;
		//usleep( (1e6 / desiredFrameRate) - 830);
	}

	delete sensorSharedMemory;

	shmdt(sensorSharedMemory);

	shmctl(shmId, IPC_RMID, NULL);

	return 0;
}

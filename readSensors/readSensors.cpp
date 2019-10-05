/*
	Copyright 2019 Kyle Johnston

	Read Sensors populates Shared Sensor Memory with the readings from
*/

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <memory>

#include "Navio2/LSM9DS1.h"
#include "Common/Util.h"

#include "sharedMemoryStructs.h"

int main(int argc, char const *argv[])
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

	while (true) {
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
	   usleep(20000);
	}

	delete sensorSharedMemory;

	shmdt(sensorSharedMemory);

	shmctl(shmId, IPC_RMID, NULL);

	return 0;
}

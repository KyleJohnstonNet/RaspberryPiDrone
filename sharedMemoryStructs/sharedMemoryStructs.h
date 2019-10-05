/*
	Copyright 2019 Kyle Johnston

*/

#ifndef _sharedMemoryStructs_H_
#define _sharedMemoryStructs_H_

#include <limits.h>

#define NUMBER_OF_MOTORS 4

#define SENSOR_SHM_KEY 			"/home/pi/sensorSharedMemory"
#define STATE_SHM_KEY 			"/home/pi/stateSharedMemory"
#define MOTOR_OUTPUT_SHM_KEY 		"/home/pi/motorOutputSharedMemory"

struct imuData
{
	int _sequenceCount;
	float ax, ay, az;
	float gx, gy, gz;
	float mx, my, mz;
	imuData() {
		_sequenceCount = 0;
		ax = ay = az = 0;
		gx = gy = gz = 0;
		mx = my = mz = 0;
	}
	int sequenceCount() {
		return this->_sequenceCount;
	}
	void incrementSequence() {
		this->_sequenceCount = (this->_sequenceCount + 1) % (INT_MAX - 1);
	}
};

struct barometerData
{
	int sequenceCount;
	float temp, pressure;
	barometerData() {
		sequenceCount = (unsigned int) 0;
		temp = pressure = 0;
	}
	void incrementSequence() {
		this->sequenceCount = (this->sequenceCount + 1) % (INT_MAX - 1);
	}
};

struct shmSensorStruct
{
	imuData imu2;
	barometerData barometer;
	imuData imu1;
};

struct Acceleration {
	float x, y, z;
	Acceleration() {
		x = y = z = 0.0;
	}
};

struct dAcceleration
{
	
};

struct Ortientation {
	float roll, pitch, yaw;
	Ortientation() {
		roll = pitch = yaw = 0;
	}
};

struct shmStateStruct
{
	Acceleration linearAccel;
};

struct shmMotorThrottleStruct
{
	const int numberOfMotors = NUMBER_OF_MOTORS;
	int motor [NUMBER_OF_MOTORS];
	shmMotorThrottleStruct() {
		for (int i = 0; i < NUMBER_OF_MOTORS; ++i)
		{
			this->motor[i] = 0;
		}
	}
};



#endif

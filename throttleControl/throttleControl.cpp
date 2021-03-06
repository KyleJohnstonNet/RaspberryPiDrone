/*
	Copyright 2019 Kyle Johnston

	Throttle Control manges the pwm output based on throttle percentages placed in Motor Output
	Shared Memory by Vehicle Control.
*/

#include <iostream>
#include <unistd.h>
#include <string>
#include <vector>
#include <cstdlib>
#include <memory>
#include <csignal>
#include <sys/ipc.h>
#include <sys/shm.h>

#include <pthread.h> 

#include "Common/Util.h"

#include "sharedMemoryStructs.h"
#include "EscInterface.h"

const uint SERVO_MIN_MICROSEC = 1010000;
const uint SERVO_MAX_MICROSEC = 1990000;

int main(int argc, char const *argv[])
{
	char motorThrottleKeyPath[] = "/home/pi/motorOutputSharedMemory";
	key_t motorThrottleShmKey = ftok(motorThrottleKeyPath, 65);
	int motorThrottleShmId = shmget(motorThrottleShmKey, sizeof(shmMotorThrottleStruct), 0666|SHM_RDONLY);
	shmMotorThrottleStruct* motorThrottleSharedMemory = (shmMotorThrottleStruct*) shmat(motorThrottleShmId, (void*)0, 0);

	std::vector<EscInterface*> Motors;

	uint motorsOnPins[4] = {1, 3, 5, 7};

	if (check_apm()) {
		fprintf(stderr, "check_apm()returned true. We think that's bad. Aborting.\n");
		return false;
	}

	if (getuid()) {
		fprintf(stderr, "Not root. Please launch like this: sudo %s\n", argv[0]);
		return 1;
	}

	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		printf("Creating motor %i\n", i);
		Motors.push_back(nullptr);
		Motors[i] = new EscInterface(motorsOnPins[i] - 1, SERVO_MIN_MICROSEC, SERVO_MAX_MICROSEC);
		printf("Created Motor %i\n", i);
		sleep(2);
	}
/*
	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		Motors[i]->setPercentage(100);
		Motors[i]->setPulseWidth();
		usleep(400000);
		Motors[i]->setPulseWidth();
		Motors[i]->setPercentage(0);
		usleep(400000);
		Motors[i]->setPulseWidth();
		printf("Finished arming motor %i.\n", i);
		sleep(1);
	}
*/
	printf("Motors ready for use.\n");

	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		Motors[i]->startRefresherThread();
	}
	sleep(3);

	int throttle;

	while (true) {
		for (int j = 0; j < NUMBER_OF_MOTORS; j++)
		{
		throttle = motorThrottleSharedMemory->motor[j]; 
			Motors[j]->setPercentage(throttle);
			Motors[j]->setPulseWidth();
   	}
		usleep(190000);
		//printf(".\n");
	}

	for (int i = 0; i < NUMBER_OF_MOTORS; i++) {
		delete Motors[i];
	}

	shmdt(motorThrottleSharedMemory);

	return 0;
}

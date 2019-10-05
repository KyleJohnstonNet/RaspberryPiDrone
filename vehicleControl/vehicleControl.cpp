/*
	Copyright 2019 Kyle Johnston

    Vehicle Control calculates throttle percentages for each motor based on the vehicle's position
    state read from State Shared Memory.  Throttle values are sent to Throttle Control using Motor
    Output Shared Memory.
*/

#include <iostream>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <math.h>

#include "sharedMemoryStructs.h"

struct MotorAxes
{
    float x;
    float y;
    MotorAxes() {
        x = y =0;
    }
    float motorOne() {
        if (x > 0) {
            return x;
        } else {
            return 0;
        }
    }
    float motorTwo() {
        if (x < 0) {
            return -x;
        } else {
            return 0;
        }
    }
    float motorThree() {
        if (y > 0) {
            return y;
        } else {
            return 0;
        }
    }
    float motorFour() {
        if (y < 0) {
            return -y;
        } else {
            return 0;
        }
    }
};

struct throttleSettingsStruct
{
	int motorOne, motorTwo, motorThree, motorFour;
	MotorAxes* MA;
	int baseThrottle;
	const int deadbandOne = 0.5;
	const float degToThrottlePercentage = 3.0;
	throttleSettingsStruct() {
		motorOne = motorTwo = motorThree = motorFour = 0;
	}
	void setBaseThrottle(int BT) {
		this->baseThrottle = BT;
	}
	void setOverlayPointer(MotorAxes* _MA) {
		this->MA = _MA;
	}
	void updateOverlays() {
		if (MA->motorOne() > 0.5) {
			this->motorOne = this->baseThrottle + ( MA->motorOne() * this->degToThrottlePercentage);
		} else {
			this->motorOne = this->baseThrottle;
		}
		if (MA->motorTwo() > 0.5) {
			this->motorTwo = this->baseThrottle + ( MA->motorTwo() * this->degToThrottlePercentage);
		} else {
			this->motorTwo = this->baseThrottle;
		}
		if (MA->motorThree() > 0.5) {
			this->motorThree = this->baseThrottle + ( MA->motorThree() * this->degToThrottlePercentage);
		} else {
			this->motorThree = this->baseThrottle;
		}
		if (MA->motorFour() > 0.5) {
			this->motorFour = this->baseThrottle + ( MA->motorFour() * this->degToThrottlePercentage);
		} else {
			this->motorFour = this->baseThrottle;
		}
	}
};

void VehicleFrameToMotorAxes(Acceleration* linearAccel, MotorAxes* MA) {
	float xv = linearAccel->x;
	float yv = linearAccel->y;
	float theta = atan(3.5/4.0);

	MA->x = (xv * ( +1.0/sin(theta) ) ) + (yv * ( 1.0/cos(theta) ) );
	MA->y = (xv * ( -1.0/sin(theta) ) ) + (yv * ( 1.0/cos(theta) ) );

	return;
}

int main(int argc, char const *argv[])
{
	char stateKeyPath[] = STATE_SHM_KEY;
    key_t stateShmKey = ftok(stateKeyPath, 65);
    int stateShmId = shmget(stateShmKey, sizeof(shmStateStruct), 0666|SHM_RDONLY);
    shmStateStruct* stateSharedMemory = (shmStateStruct*) shmat(stateShmId, (void*)0, 0); 

    char motorThrottleKeyPath[] = "/home/pi/motorOutputSharedMemory";
    key_t motorThrottleShmKey = ftok(motorThrottleKeyPath, 65);
    int motorThrottleShmId = shmget(motorThrottleShmKey, sizeof(shmMotorThrottleStruct), 0666|IPC_CREAT);
    shmMotorThrottleStruct* motorThrottleSharedMemory = new (shmat(motorThrottleShmId, (void*)0, 0)) shmMotorThrottleStruct;

    int j = 0;
    int k = 0;

    MotorAxes motorOverlays;
    throttleSettingsStruct throttleSettings;

    throttleSettings.setOverlayPointer(&motorOverlays);
    throttleSettings.setBaseThrottle(25);

    while (true) {

    	VehicleFrameToMotorAxes(&(stateSharedMemory->linearAccel), &motorOverlays); 
    	throttleSettings.updateOverlays();

    	motorThrottleSharedMemory->motor[0] = throttleSettings.motorOne;
    	motorThrottleSharedMemory->motor[1] = throttleSettings.motorTwo;
    	motorThrottleSharedMemory->motor[2] = throttleSettings.motorThree;
    	motorThrottleSharedMemory->motor[3] = throttleSettings.motorFour;

    	if (k % 1 == 0) {
	    	/*printf(
	            "Vehicle Liniear Acceleration: %+7.3f %+7.3f %+7.3f    ",
	            stateSharedMemory->linearAccel.x,
	            stateSharedMemory->linearAccel.y,
	            stateSharedMemory->linearAccel.z
	        );*/
	    	/*printf(
	            "Motor Overlays: %+7.3f %+7.3f %+7.3f %+7.3f \n",
	            motorOverlays.motorOne(),
	            motorOverlays.motorTwo(),
	            motorOverlays.motorThree(),
	            motorOverlays.motorFour()
	        );*/
	        printf("Motor Throttle Settings:\n          %03d %03d \n             X    \n          %03d %03d \n\n",
	            throttleSettings.motorFour,
	            throttleSettings.motorOne,
	            throttleSettings.motorTwo,
	            throttleSettings.motorThree
	        );
	    }

    	k++;
    	usleep(20000);
    }

    delete motorThrottleSharedMemory;

    shmdt(motorThrottleSharedMemory);
    shmdt(stateSharedMemory);

    shmctl(motorThrottleShmId, IPC_RMID, NULL);

	return 0;
}
/*

*/

#include <unistd.h>
#include "Common/Util.h"
#include <unistd.h>
#include <memory>
#include <iostream>
#include <pthread.h>

#include "Navio2/PWM.h"
#include "Navio+/RCOutput_Navio.h"
#include "Navio2/RCOutput_Navio2.h"
#include <Navio2/Led_Navio2.h>
#include <Navio+/Led_Navio.h>

#include "EscInterface.h"

enum ServoLimits {
	SERVO_MIN_MICROSEC = 1000000,
	SERVO_MAX_MICROSEC = 2000000
};

std::unique_ptr <RCOutput> get_rcout()
{
	if (get_navio_version() == NAVIO2)
	{
		auto ptr = std::unique_ptr <RCOutput>{ new RCOutput_Navio2() };
		return ptr;
	} else
	{
		auto ptr = std::unique_ptr <RCOutput>{ new RCOutput_Navio() };
		return ptr;
	}

}

int main(int argc, char *argv[])
{
	const int pwm_output = 0;

	if (check_apm()) {
		return 1;
	}

	if (getuid()) {
		fprintf(stderr, "Not root. Please launch like this: sudo %s\n", argv[0]);
		return 1;
	}

	EscInterface myEsc(pwm_output);
	myEsc.start();

	myEsc.setMinPulseWidth(SERVO_MIN_MICROSEC);
	myEsc.setMaxPulseWidth(SERVO_MAX_MICROSEC);


	myEsc.setPercentage(0);
	printf("Pwm output pwm_output set to min.\n");
	printf("Press any key to begin arming sequence.\n");
	getchar();

	const int min_percentage = 40;
	int max_percentage = 50;
	for (int i = min_percentage; i < max_percentage; i++) {
		myEsc.setPercentage(i);
		usleep(1e6 / 2);
	}
	for (int i = max_percentage; i >= min_percentage; i--) {
		myEsc.setPercentage(i);
		usleep(1e6 / 2);
	}
	printf("Arming sequence complete.\n");
	printf("Press any key to begin cycling throttle up and down\n");
	getchar();

	int percentage = 0;
	while (true) {
		/*myEsc.setPercentage(percentage + min_percentage);
		percentage = (percentage + 1) % (max_percentage - min_percentage);
		usleep(1e6 / 3);*/
		for (int i = min_percentage; i < max_percentage; i++) {
			myEsc.setPercentage(i);
			usleep(1e6 / 2);
		}
		for (int i = max_percentage; i > min_percentage; i--) {
			myEsc.setPercentage(i);
			usleep(1e6 / 2);
		}
	}

	return 0;
}

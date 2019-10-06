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
	SERVO_MIN_MICROSEC = 1000,
	SERVO_MAX_MICROSEC = 2000
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
	//auto pwm = get_rcout();
	const int pwm_output = 0;

	if (check_apm()) {
		return 1;
	}

	if (getuid()) {
		fprintf(stderr, "Not root. Please launch like this: sudo %s\n", argv[0]);

		return 1;
	}
/*
	if ( !(pwm->initialize(pwm_output)) ) {
		return 1;
	}
		
	pwm->set_frequency(pwm_output, 50);

	if ( !(pwm->enable(pwm_output)) ) {
		return 1;
	}

	pwm->set_duty_cycle(pwm_output, SERVO_MIN_MICROSEC);
	printf("Pwm output pwm_output set to min.\n");
	printf("Press any key to set to high.\n");
	getchar();
	pwm->set_duty_cycle(pwm_output, SERVO_MAX_MICROSEC);

	printf("\nPress any key to set to min.\n");
	getchar();
	pwm->set_duty_cycle(pwm_output, SERVO_MIN_MICROSEC);

	printf("\nPress any key to cycle between low and high.\n");
	getchar();
*/
	EscInterface myEsc(pwm_output);
	myEsc.start();

	int width = 1000;
	int percentage = 0;

	while (true) {

		myEsc.setPercentage(percentage);
		percentage = (percentage + 10) % 100;
		sleep(2);

		/*
		pwm->set_duty_cycle(pwm_output, width);
		
		//width += 100;
		if (width == 2000) {
			width = 1000;
		} else {
			width = 2000;
		}

		usleep(19000);
		*/


	}

	return 0;
}

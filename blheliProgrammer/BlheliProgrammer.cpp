/*

*/

#include <unistd.h>
#include "Navio2/PWM.h"
#include "Navio+/RCOutput_Navio.h"
#include "Navio2/RCOutput_Navio2.h"
#include <Navio2/Led_Navio2.h>
#include <Navio+/Led_Navio.h>
#include "Common/Util.h"
#include <unistd.h>
#include <memory>
#include <iostream>
#include <pthread.h>
#include "EscInterface.h"

#define SERVO_MIN 1000 /*micro S*/
#define SERVO_MAX 2000 /*micro S*/

#define PWM_OUTPUT 0


using namespace Navio;

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

	if (check_apm()) {
		return 1;
	}

	if (getuid()) {
		fprintf(stderr, "Not root. Please launch like this: sudo %s\n", argv[0]);
	}
/*
	if ( !(pwm->initialize(PWM_OUTPUT)) ) {
		return 1;
	}
		
	pwm->set_frequency(PWM_OUTPUT, 50);

	if ( !(pwm->enable(PWM_OUTPUT)) ) {
		return 1;
	}

	pwm->set_duty_cycle(PWM_OUTPUT, SERVO_MIN);
	printf("Pwm output PWM_OUTPUT set to min.\n");
	printf("Press any key to set to high.\n");
	getchar();
	pwm->set_duty_cycle(PWM_OUTPUT, SERVO_MAX);

	printf("\nPress any key to set to min.\n");
	getchar();
	pwm->set_duty_cycle(PWM_OUTPUT, SERVO_MIN);

	printf("\nPress any key to cycle between low and high.\n");
	getchar();
*/
	EscInterface myEsc(PWM_OUTPUT);
	myEsc.start();

	int width = 1000;
	int percentage = 0;

	while (true) {

		myEsc.setPercentage(percentage);
		percentage = (percentage + 10) % 100;
		sleep(2);

		/*
		pwm->set_duty_cycle(PWM_OUTPUT, width);
		
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

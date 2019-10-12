/*

*/

#include <unistd.h>
#include "Common/Util.h"
#include <unistd.h>
#include <memory>
#include <iostream>
#include <pthread.h>
#include <vector>

#include "Navio2/PWM.h"
#include "Navio+/RCOutput_Navio.h"
#include "Navio2/RCOutput_Navio2.h"
#include <Navio2/Led_Navio2.h>
#include <Navio+/Led_Navio.h>

#include "EscInterface.h"

enum ServoLimits {
	SERVO_MIN_MICROSEC = 1010000,
	SERVO_MAX_MICROSEC = 1990000
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

	std::vector<EscInterface> myEscs;
	std::vector<EscInterface>::iterator escIt;
	EscInterface *pEsc;

	for (const int &i : {0, 2, 4, 6}) {
		myEscs.push_back( EscInterface(i, SERVO_MIN_MICROSEC, SERVO_MAX_MICROSEC) );
		//myEscs.back().setPercentage(0);
		//myEscs.back().startRefresherThread();
		printf("\n");
		sleep(2);
	}

	for ( auto &pEsc : myEscs) {
		pEsc.startRefresherThread();
	}
	sleep(3);

	const int min_percentage = 0;
	const int max_percentage = 30;
	int i;
	while (true) {
		for (i = min_percentage; i <= max_percentage; i++) {
			for ( auto &pEsc : myEscs) {
				pEsc.setPercentage(i);
			}
			//usleep(1e6 / 3);
			getchar();
		}
		for (i; i >= min_percentage; i--) {
			for ( auto &pEsc : myEscs) {
				pEsc.setPercentage(i);
			}
			usleep(1e6 / 3);
		}
	}
	return 0;

	printf("All EscInterface's constructed and their output set to 100%\n");
	/*printf("Press enter to begin arming sequence.\n");
	getchar();*/

	
	for ( auto &pEsc : myEscs) {
		pEsc.stopRefresherThread();
		sleep(1);
		pEsc.setPercentage(100);
		pEsc.startRefresherThread();
	}
	/*printf("ESC output is enabled.\n");*/
	printf("Connect ESC's to calibrate and press enter when complete.\n");
	getchar();

	const int cal_min_percentage =   0;
	const int cal_max_percentage = 100;
	const int cal_step 			 =   5;

	for ( auto &pEsc : myEscs) {
		pEsc.setPercentage(cal_min_percentage);
	}
	usleep(1e6 * 2);

	for (int i = cal_min_percentage; i <= cal_max_percentage; i += cal_step) {
		for ( auto &pEsc : myEscs) {
			pEsc.setPercentage(i);
		}
		usleep(1e6 / 5);
	}

	printf("Press enter when the repeating chime starts.\n");
	getchar();

	for (int i = cal_max_percentage; i >= cal_min_percentage; i -= cal_step) {
		for ( auto &pEsc : myEscs) {
			pEsc.setPercentage(i);
		}
		usleep(1e6 / 5);
	}

	printf("Press enter when the repeating chime starts.\n");
	getchar();

	for ( auto &pEsc : myEscs) {
		pEsc.stopRefresherThread();
	}
	usleep(1e6 * 2);

	//const int min_percentage = 0;
	//const int max_percentage = 20;
	printf("Press enter to cycle from %i to %i percent.\n", min_percentage, max_percentage);
	getchar();

	for ( auto &pEsc : myEscs) {
		pEsc.startRefresherThread();
	}

	printf("Use Ctrl+C to exit.\n");
	while (true) {
		for (int i = min_percentage; i <= max_percentage; i++) {
			for ( auto &pEsc : myEscs) {
				pEsc.setPercentage(i);
			}
			usleep(1e6 / 3);
		}
		for (int i = max_percentage; i >= min_percentage; i--) {
			for ( auto &pEsc : myEscs) {
				pEsc.setPercentage(i);
			}
			usleep(1e6 / 3);
		}
	}

	return 0;
}

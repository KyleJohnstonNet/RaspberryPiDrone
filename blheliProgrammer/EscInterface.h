/*
	Copyright 2019 Kyle Johnston

*/

#ifndef _EscInterface_H_
#define _EscInterface_H_

#include <pthread.h>

using namespace std;

class EscInterface
{
	unsigned int channel = 0;
	unsigned int minPulseWidth = 1000;
	unsigned int maxPulseWidth = 2000;
	unsigned int commandedWidth = 0;
	pthread_t refresherThread;
	bool refreshContinue = false;
	bool init();
	bool enable();
	bool setFrequency(unsigned int);
	bool setPulseWidth();
	void refreshOutput();
	static void refreshOutputPointer(void *context);
public:
	EscInterface(unsigned int);
	~EscInterface();
	int start();
	int stop();
	int setPercentage(int);
	void setMinPulseWidth(int);
	void setMaxPulseWidth(int);
};

#endif
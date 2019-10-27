/*
	Copyright 2019 Kyle Johnston

*/

#ifndef _EscInterface_H_
#define _EscInterface_H_

class EscInterface
{
	unsigned int channel = 0;
	unsigned int minPulseWidth = 1000000;
	unsigned int maxPulseWidth = 2000000;
	unsigned int commandedWidth = 0;
	pthread_t refresherThread;
	bool isActive = false;
	bool init();
	bool close();
	bool enable();
	bool disable();
	bool setFrequency(unsigned int);
	void refreshOutput();
	static void* refreshThreadMain(void* context);
public:
	EscInterface(unsigned int, unsigned int, unsigned int);
	EscInterface(unsigned int);
	~EscInterface();
	int startRefresherThread();
	int stopRefresherThread();
	int percentageToWidth(int);
	int setPercentage(int);
	void setMinPulseWidth(int);
	void setMaxPulseWidth(int);
	bool setPulseWidth(int);
	bool setPulseWidth();
};

#endif

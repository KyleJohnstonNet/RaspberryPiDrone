/*
	Copyright 2019 Kyle Johnston

*/

#ifndef _EscInterface_H_
#define _EscInterface_H_

class EscInterface;
class EscInterface
{
	unsigned int channel = 0;
	unsigned int minPulseWidth = 1000000;
	unsigned int maxPulseWidth = 2000000;
	unsigned int commandedWidth = 0;
	bool isActive = false;
	bool init();
	bool close();
	bool enable();
	bool disable();
	bool setFrequency(unsigned int);
public:
	EscInterface(unsigned int);
	~EscInterface();
	int setPercentage(int);
	void setMinPulseWidth(int);
	void setMaxPulseWidth(int);
	bool setPulseWidth();
};

#endif

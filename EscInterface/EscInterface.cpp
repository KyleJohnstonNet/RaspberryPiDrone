/*
	Copyright 2019 Kyle Johnston

*/

#include <pthread.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <iostream>
#include <string>

#include "Common/Util.h"

#include "EscInterface.h"

bool EscInterface::init() {
	int err;

	printf("EscInterface %u: Writting value %u to %s\n", channel, channel, "/sys/class/pwm/pwmchip0/export");
	
	err = write_file("/sys/class/pwm/pwmchip0/export", "%u", channel);
	if (err >= 0 || err == -EBUSY)
	{
		return true;
	}
	else 
	{
		isActive = false;
		printf("Can't init channel %u. Error value: %i\n", channel, err);
		return false;
	}
	return true;
}

bool EscInterface::close() {
	//return false;
	int err;
	
	printf("EscInterface %u: Writting value %u to %s\n", channel, channel, "/sys/class/pwm/pwmchip0/unexport");
	
	err = write_file("/sys/class/pwm/pwmchip0/unexport", "%u", 1);
	if (err >= 0 || err == -EBUSY)
	{
		return true;
	}
	else 
	{
		isActive = false;
		printf("EscInterface: Can't close channel %u\n", channel);
		return false;
	}

	usleep(1e6 / 10);

	return true;
}

bool EscInterface::enable() {
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/enable", channel);
	strcat(path, path_ch);
	
	printf("EscInterface %u: Writting value %i to %s\n", channel, 1, path);

	if (write_file(path, "1") < 0)
	{
		isActive = false;
		printf("Can't enable channel %u\n", channel);
		return false;
	}
	return true;
}

bool EscInterface::disable() {
	//return false;
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/enable", channel);
	strcat(path, path_ch);
	
	printf("EscInterface %u: Writting value %i to %s\n", channel, 0, path);

	if (write_file(path, "0") < 0)
	{
		isActive = false;
		printf("Can't disable channel %u\n", channel);
		return false;
	}
	return true;
}

bool EscInterface::setFrequency(unsigned int freq) {
	int period_ns;
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/period", channel);
	strcat(path, path_ch);

	period_ns = 1e9 / freq;

	printf("EscInterface %u: Writting value %u to %s\n", channel, period_ns, path);

	if (write_file(path, "%u", period_ns) < 0)
	{
		isActive = false;
		printf("Can't set period to channel %u\n", channel);
		return false;
	}
	return true;
}

inline void EscInterface::refreshOutput() {
	setPulseWidth();
	usleep(19500);

	return;
}

void* EscInterface::refreshThreadMain(void* arg) {
	EscInterface* pThis = static_cast<EscInterface*>(arg);
	
	while (pThis->isActive) {
		pThis->refreshOutput();
	}

	pthread_exit(0);

	return nullptr;
}

EscInterface::EscInterface(unsigned int _channel, unsigned int _min, unsigned int _max) 
	: EscInterface(_channel)
{ 
	setMinPulseWidth(_min);
	setMaxPulseWidth(_max);
}

EscInterface::EscInterface(unsigned int _channel) {
	channel = _channel;
	close(); // In case channel wasn't properly closed out previously
	init();
	setFrequency(50);
	setPercentage(0);
	//setPulseWidth();
	enable();
	//startRefresherThread();
}

EscInterface::~EscInterface() {
	stopRefresherThread();
	usleep(1e6 /5);
	disable();
	close();
}

int EscInterface::startRefresherThread() {
	isActive = true;
	pthread_create(&refresherThread, NULL, refreshThreadMain, (void*) this);

	return 0;

}

int EscInterface::stopRefresherThread() {
	isActive = false;
	setPulseWidth( percentageToWidth(0) );

	return 0;
}

inline int EscInterface::percentageToWidth(int percentage) {
	int width = minPulseWidth + (int) ( ((float) percentage / 100.0f) * (float) (maxPulseWidth - minPulseWidth) );

	return width;
}

int EscInterface::setPercentage(int percentage) {
	if (percentage >= 0 && percentage <= 100) {
	   commandedWidth = percentageToWidth(percentage);
	   printf("EscInterface %u: Commanded width updated to %i percent or %i ns\n", channel, percentage, commandedWidth);
	} else {
		printf("EscInterface %u: Ignorming commanded percentage of %i\n", channel, percentage);
	}

	return 0;
}

void EscInterface::setMinPulseWidth(int x) {
	minPulseWidth = x;

	return;
}

void EscInterface::setMaxPulseWidth(int x) {
	maxPulseWidth = x;

	return;
}

bool EscInterface::setPulseWidth() {
	return setPulseWidth(commandedWidth);
}

bool EscInterface::setPulseWidth(int width) {
	int period_ns;
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/duty_cycle", channel);
	strcat(path, path_ch);

	//printf("EscInterface: Set pulse width.\n");

	period_ns = width;

	// printf("EscInterface: Writting value %u to %s\n", period_ns, path);

	if (write_file(path, "%u", period_ns) < 0)
	{
		isActive = false;
		printf("EscInterface %u: Can't set duty cycle to channel %u\n", channel, channel);
		return false;
	}
	return true;
}
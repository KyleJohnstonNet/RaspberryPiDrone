/*
	Copyright 2019 Kyle Johnston

*/

#include <pthread.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "EscInterface.h"
#include "Common/Util.h"

using namespace std;

bool EscInterface::init() {
	int err;
	err = write_file("/sys/class/pwm/pwmchip0/export", "%u", this->channel);
	if (err >= 0 || err == -EBUSY)
	{
		return true;
	}
	else 
	{
		printf("Can't init channel %u\n", this->channel);
		return false;
	}
	return true;
}

bool EscInterface::enable() {
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/enable", this->channel);
	strcat(path, path_ch);
	
	if (write_file(path, "1") < 0)
	{
		printf("Can't enable channel %u\n", this->channel);
		return false;
	}
	return true;
}

bool EscInterface::setFrequency(unsigned int freq) {
	int period_ns;
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/period", this->channel);
	strcat(path, path_ch);

	period_ns = 1e9 / freq;
	if (write_file(path, "%u", period_ns) < 0)
	{
		printf("Can't set period to channel %u\n", this->channel);
		return false;
	}
	return true;

}

bool EscInterface::setPulseWidth() {
	int period_ns;
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/duty_cycle", this->channel);
	strcat(path, path_ch);

	period_ns = this->commandedWidth * 1e6;
	if (write_file(path, "%u", period_ns) < 0)
	{
		printf("Can't set duty cycle to channel %u\n", this->channel);
		return false;
	}
	return true;
}

void EscInterface::refreshOutput() {
	while (this->refreshContinue) {
		setPulseWidth();
		usleep(19500);
	}

	pthread_exit(0);

	return;
}
static void refreshOutputPointer(void *context) {
	 return ((EscInterface *)context)->refreshOutput();
}

EscInterface::EscInterface(unsigned int _channel) {
	this->channel = _channel;
	this->init();
	this->enable();
	this->setFrequency(50);
	this->setPercentage(0);
	this->setPulseWidth();
}
EscInterface::~EscInterface() {
	this->stop();
}
int EscInterface::start() {
	this->refreshContinue = true;
	pthread_create(&refresherThread, NULL, EscInterface::refreshOutputPointer, &this);

	return 0;

}
int EscInterface::stop() {
	this->refreshContinue = false;

	return 0;
}
int EscInterface::setPercentage(int percentage) {
	this->commandedWidth = (percentage / 100) * (maxPulseWidth - minPulseWidth);

	return 0;
}
void EscInterface::setMinPulseWidth(int x) {
	this->minPulseWidth = x;

	return;
}
void EscInterface::setMaxPulseWidth(int x) {
	this->maxPulseWidth = x;

	return;
}
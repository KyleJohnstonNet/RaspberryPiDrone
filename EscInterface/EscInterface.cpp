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

	printf("EscInterface: Writting value %u to %s\n", this->channel, "/sys/class/pwm/pwmchip0/export");
	
	err = write_file("/sys/class/pwm/pwmchip0/export", "%u", this->channel);
	if (err >= 0 || err == -EBUSY)
	{
		return true;
	}
	else 
	{
		this->isActive = false;
		printf("Can't init channel %u. Error value: %i\n", this->channel, err);
		return false;
	}
	return true;
}

bool EscInterface::close() {
	return false;
	int err;
	
	printf("EscInterface: Writting value %u to %s\n", this->channel, "/sys/class/pwm/pwmchip0/unexport");
	
	err = write_file("/sys/class/pwm/pwmchip0/unexport", "%u", 1);
	if (err >= 0 || err == -EBUSY)
	{
		return true;
	}
	else 
	{
		this->isActive = false;
		printf("EscInterface: Can't close channel %u\n", this->channel);
		return false;
	}
	return true;
}

bool EscInterface::enable() {
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/enable", this->channel);
	strcat(path, path_ch);
	
	printf("EscInterface: Writting value %i to %s\n", 1, path);

	if (write_file(path, "1") < 0)
	{
		this->isActive = false;
		printf("Can't enable channel %u\n", this->channel);
		return false;
	}
	return true;
}

bool EscInterface::disable() {
	return false;
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/enable", this->channel);
	strcat(path, path_ch);
	
	printf("EscInterface: Writting value %i to %s\n", 0, path);

	if (write_file(path, "0") < 0)
	{
		this->isActive = false;
		printf("Can't disable channel %u\n", this->channel);
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

	printf("EscInterface: Writting value %u to %s\n", period_ns, path);

	if (write_file(path, "%u", period_ns) < 0)
	{
		this->isActive = false;
		printf("Can't set period to channel %u\n", this->channel);
		return false;
	}
	return true;
}

void EscInterface::refreshOutput() {
	setPulseWidth();
	usleep(19500);

	return;
}

void EscInterface::refreshThreadMain(void* arg) {
	EscInterface* pThis = static_cast<EscInterface*>(arg);
	while (pThis->isActive) {
		pThis->refreshOutput();
	}

	pthread_exit(0);

	return;
}

EscInterface::EscInterface(unsigned int _channel) {
	this->channel = _channel;
	this->init();
	this->enable();
	this->setFrequency(50);
	this->enable();
	this->setPercentage(0);
	this->setPulseWidth();
	this->disable();
	this->close();
}

EscInterface::~EscInterface() {
	//this->disable();
	//this->close();
	this->stop();
}

int EscInterface::start() {
	this->isActive = true;
	pthread_create(&refresherThread, NULL, (void* (*)(void*)) &refreshThreadMain, (void*) this);

	return 0;

}

int EscInterface::stop() {
	this->isActive = false;

	return 0;
}

int EscInterface::setPercentage(int percentage) {
	if (percentage >= 0 && percentage <= 100) {
	   this->commandedWidth = minPulseWidth + (int) ( ((float) percentage / 100.0) * (float) (maxPulseWidth - minPulseWidth) );
	   printf("EscInterface: Commanded width updated to %i percentage or %i ns\n", percentage, this->commandedWidth);
	} else {
		printf("EscInterface: Ignorming commanded percentage of %i\n", percentage);
	}
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

bool EscInterface::setPulseWidth() {
	int period_ns;
	char path[60] = "/sys/class/pwm/pwmchip0";
	char path_ch[20];
	sprintf(path_ch, "/pwm%u/duty_cycle", this->channel);
	strcat(path, path_ch);

	//printf("EscInterface: Set pulse width.\n");

	period_ns = this->commandedWidth;

	// printf("EscInterface: Writting value %u to %s\n", period_ns, path);

	if (write_file(path, "%u", period_ns) < 0)
	{
		this->isActive = false;
		printf("Can't set duty cycle to channel %u\n", this->channel);
		return false;
	}
	return true;
}
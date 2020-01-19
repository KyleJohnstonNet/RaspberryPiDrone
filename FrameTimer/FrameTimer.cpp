/*
	Copyright 2019 Kyle Johnston

	FrameTimer class accepts a frame rate in Hertz and provides a function which will block untill it is time to start
	the next frame.
*/

#include <thread>

#include "FrameTimer.h"

FrameTimer::FrameTimer(unsigned int rate) {
	rate_ = rate;
	nanosecondsPerFrame_ = std::chrono::nanoseconds( (unsigned int) (1e9 / rate) );

	nextFrameTime_ = std::chrono::high_resolution_clock::now() + nanosecondsPerFrame_;
}

void FrameTimer::nextFrame() {
	now_ = std::chrono::high_resolution_clock::now();
	unsigned long long sleepNs;
	unsigned long long catchUpNs;

	sleepNs = nextFrameTime_.time_since_epoch().count() - now_.time_since_epoch().count();

	// If we missed frame(s) then return setFrameTime such that we don't drift from our frame periods
	// and then return immediately. This should minimize frame misalignment for occasional overruns.
	if (sleepNs < 0) {
		catchUpNs = nanosecondsPerFrame_ * (1 + (-sleepNs / nanosecondsPerFrame_));
		nextFrameTime_ += catchUpNs;
	}
	else {
		nextFrameTime_ += nanosecondsPerFrame_;
		std::this_thread::sleep_for( std::chrono::nanoseconds(sleepNs) );
	}

	return;
}
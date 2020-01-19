/*
	Copyright 2019 Kyle Johnston

	FrameTimer class accepts a frame rate in Hertz and provides a function which will block untill it is time to start
	the next frame.
*/

#include <chrono>

class FrameTimer
{
public:
	FrameTimer(unsigned int rate);
	~FrameTimer() {};
	void nextFrame();
private:
	unsigned int rate_;
	std::chrono::nanoseconds nanosecondsPerFrame_;
	std::chrono::high_resolution_clock::time_point nextFrameTime_;
	std::chrono::high_resolution_clock::time_point now_;
};
#ifndef _TIMER_H
#define _TIMER_H

#include "../pch.h"

class Timer{
public:
	
	Timer();
	~Timer();
	
	void Start();
	
	std::chrono::time_point<std::chrono::steady_clock> GetCurrentTime();
	double GetElapsedTime();
	
private:
	std::chrono::time_point<std::chrono::steady_clock> last_loop_time;
};

#endif

#ifndef _TIMER_H
#define _TIMER_H

#include <chrono>

class Timer{
public:
	
	Timer();
	~Timer();
	
	void Start();
	
	std::chrono::time_point<std::chrono::system_clock> GetCurrentTime();
	float GetElapsedTime();
	
private:
	std::chrono::time_point<std::chrono::system_clock> last_loop_time;
};

#endif

#include "pch.h"
#include "Timer.h"

Timer::Timer(){
}

Timer::~Timer(){
}

void Timer::Start(){
	last_loop_time = GetCurrentTime();
}

std::chrono::time_point<std::chrono::steady_clock> Timer::GetCurrentTime(){
	return std::chrono::steady_clock::now();
}

double Timer::GetElapsedTime(){
	std::chrono::time_point<std::chrono::steady_clock> current_time = GetCurrentTime();
	std::chrono::duration<double, std::ratio<1,1000>> diff = current_time-last_loop_time;
	double elapsed = diff.count();
	last_loop_time = current_time;
	return elapsed;
}

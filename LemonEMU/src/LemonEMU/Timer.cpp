#include "pch.h"
#include "Timer.h"

Timer::Timer(){
}

Timer::~Timer(){
}

void Timer::Start(){
	last_loop_time = GetCurrentTime();
}

std::chrono::time_point<std::chrono::system_clock> Timer::GetCurrentTime(){
	return std::chrono::system_clock::now();
}

float Timer::GetElapsedTime(){
	std::chrono::time_point<std::chrono::system_clock> current_time = GetCurrentTime();
	std::chrono::duration<float,std::ratio<1, 1000>> diff = current_time-last_loop_time;
	float elapsed = diff.count();
	last_loop_time = current_time;
	return elapsed;
}

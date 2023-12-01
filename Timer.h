#pragma once

#include "Core.h"
#include <chrono>

#define SECONDS_TO_NANOSECONDS 1000000000.0
#define MILISECONDS_TO_NANOSECONDS 1000000.0
#define SECONDS_TO_MILISECONDS 1000.0
#define NANOSECONDS_TO_SECONDS 1.0/1000000000.0
#define NANOSECONDS_TO_MILISECONDS 1.0/1000000.0
#define MILISECONDS_TO_SECONDS 1.0/1000.0

class Timer{
		std::chrono::high_resolution_clock::time_point start_time;

		public:
		Timer();

		void reset();

		void start();

		void stop();

		float ms() const;

		float ns() const;
};

Timer::Timer() {
	start_time = std::chrono::high_resolution_clock::now();
}

void Timer::stop() {
	//todo
}

void Timer::start() {
	//todo
}

void Timer::reset() {

	start_time = std::chrono::high_resolution_clock::now();
}

float Timer::ms() const {
	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
	return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
}

float Timer::ns() const {
	std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
	return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
}

#pragma once

#include <chrono>

#define SECONDS_TO_NANOSECONDS 1000000000.0
#define MILISECONDS_TO_NANOSECONDS 1000000.0
#define SECONDS_TO_MILISECONDS 1000.0
#define NANOSECONDS_TO_SECONDS (1.0/1000000000.0)
#define NANOSECONDS_TO_MILISECONDS (1.0/1000000.0)
#define MILISECONDS_TO_SECONDS (1.0/1000.0)

class Timer {
	std::chrono::high_resolution_clock::time_point start_time;

public:
	Timer() {
		start_time = std::chrono::high_resolution_clock::now();
	}

	void reset() {

		start_time = std::chrono::high_resolution_clock::now();
	}

	void startstart() {
		//todo
	}
	void stop() {
		//todo
	}

	float ms() const {
		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
		return static_cast<float>(std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());
	}

	float ns() const {
		std::chrono::duration<float> duration = std::chrono::high_resolution_clock::now() - start_time;
		return static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count());
	}
};
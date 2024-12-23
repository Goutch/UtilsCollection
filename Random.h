#pragma once

#include <cstdlib>

namespace utils {
	class Random {
	public:
		static float floatRange(float min, float max) {
			return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		}

		static int intRange(int min, int max) {
			return min + (rand() % (max - min));
		}

		static uint uintRange(uint min, uint max) {
			return min + (rand() % (max - min));
		}
	};
}

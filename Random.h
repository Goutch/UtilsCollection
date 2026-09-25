#pragma once

#include <cstdlib>

namespace HBE {
	class Random {
	public:

		static float floatRange(float min, float max) {
			return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
		}

		static vec3 vec3Range(vec3 min, vec3 max) {
			return vec3(floatRange(min.x, max.x), floatRange(min.y, max.y), floatRange(min.z, max.z));
		}

		static int intRange(int min, int max) {
			return min + (rand() % (max - min));
		}

		static uint uintRange(uint min, uint max) {
			return min + (rand() % (max - min));
		}
	};
}

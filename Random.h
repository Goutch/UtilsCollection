#pragma once

#include <cstdlib>

namespace utils {
	class Random {
	public:
		static float floatRange(float min, float max);

		static int intRange(int min, int max);
	};
}

#pragma once
#include <limits>
#include <cstdint>
#define HBE_NULL_HANDLE std::numeric_limits<uint64_t>::max();

namespace HBE
{
    typedef uint64_t hbe_handle_t;
	using ImageHandle = hbe_handle_t;
	typedef hbe_handle_t Handle;
}

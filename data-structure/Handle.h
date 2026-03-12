#pragma once
#include <limits>
#include <cstdint>

namespace HBE
{
    typedef uint64_t hbe_handle_t;
	typedef hbe_handle_t Handle;
	constexpr Handle HBE_NULL_HANDLE = std::numeric_limits<uint64_t>::max();
}

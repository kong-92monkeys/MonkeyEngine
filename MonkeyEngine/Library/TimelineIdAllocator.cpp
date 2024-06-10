#include "TimelineIdAllocator.h"

namespace Lib
{
	uint64_t TimelineIdAllocator<true>::allocate() noexcept
	{
		return __timeline.fetch_add(1ULL, std::memory_order::acq_rel);
	}

	uint64_t TimelineIdAllocator<false>::allocate() noexcept
	{
		return __timeline++;
	}
}
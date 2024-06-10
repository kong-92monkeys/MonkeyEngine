#pragma once

#include <atomic>

namespace Lib
{
	template <bool $threadSafe>
	class TimelineIdAllocator;

	template <>
	class TimelineIdAllocator<true>
	{
	public:
		[[nodiscard]]
		uint64_t allocate() noexcept;

	private:
		std::atomic<uint64_t> __timeline{ 0ULL };
	};

	template <>
	class TimelineIdAllocator<false>
	{
	public:
		[[nodiscard]]
		uint64_t allocate() noexcept;

	private:
		uint64_t __timeline{ };
	};
}

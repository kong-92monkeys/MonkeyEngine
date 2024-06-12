export module ntmonkeys.com.Lib.TimelineIdAllocator;

export import <atomic>;

namespace Lib
{
	export template <bool $threadSafe>
	class TimelineIdAllocator;

	export template <>
	class TimelineIdAllocator<true>
	{
	public:
		[[nodiscard]]
		uint64_t allocate() noexcept;

	private:
		std::atomic<uint64_t> __timeline{ 0ULL };
	};

	export template <>
	class TimelineIdAllocator<false>
	{
	public:
		[[nodiscard]]
		uint64_t allocate() noexcept;

	private:
		uint64_t __timeline{ };
	};
}

module: private;

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
export module ntmonkeys.com.Lib.RegionAllocator;

import ntmonkeys.com.Lib.MathUtil;
import <optional>;
import <map>;

namespace Lib
{
	export class RegionAllocator
	{
	public:
		RegionAllocator(const size_t size) noexcept;

		[[nodiscard]]
		std::optional<size_t> allocate(const size_t size, const size_t alignment) noexcept;
		void free(const size_t offset) noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

	private:
		const size_t __size;
		std::map<size_t, size_t> __regions;
	};
}

module: private;

namespace Lib
{
	RegionAllocator::RegionAllocator(const size_t size) noexcept :
		__size{ size }
	{}

	std::optional<size_t> RegionAllocator::allocate(const size_t size, const size_t alignment) noexcept
	{
		std::optional<size_t> retVal;
		const size_t alignedSize{ MathUtil::ceilAlign(size, alignment) };

		if (__regions.empty() && (__size >= alignedSize))
			retVal = 0ULL;
		else
		{
			for (auto regionIt{ __regions.begin() }; regionIt != __regions.end(); )
			{
				auto nextIt{ regionIt };
				++nextIt;

				const auto [itOffset, itSize]{ *regionIt };

				const size_t from	{ MathUtil::ceilAlign(itOffset + itSize, alignment) };
				const size_t to		{ (nextIt == __regions.end()) ? __size : nextIt->first };

				if ((to - from) >= alignedSize)
				{
					retVal = from;
					break;
				}

				regionIt = nextIt;
			}
		}

		if (retVal.has_value())
			__regions.emplace(retVal.value(), alignedSize);

		return retVal;
	}

	void RegionAllocator::free(const size_t offset) noexcept
	{
		__regions.erase(offset);
	}

	bool RegionAllocator::isEmpty() const noexcept
	{
		return __regions.empty();
	}
}
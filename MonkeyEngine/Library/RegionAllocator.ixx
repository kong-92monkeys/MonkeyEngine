export module ntmonkeys.com.Lib.RegionAllocator;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.MathUtil;
import <optional>;
import <map>;
import <stdexcept>;

namespace Lib
{
	export class RegionAllocator : public Unique
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

	export class Region : public Unique
	{
	public:
		Region(RegionAllocator &allocator, const size_t size, const size_t alignment);
		virtual ~Region() noexcept;

		[[nodiscard]]
		constexpr size_t getSize() const noexcept;

		[[nodiscard]]
		constexpr size_t getOffset() const noexcept;

	private:
		RegionAllocator &__allocator;
		const size_t __size;
		const size_t __alignment;

		size_t __offset{ };

		void __create();
	};

	constexpr size_t Region::getSize() const noexcept
	{
		return __size;
	}

	constexpr size_t Region::getOffset() const noexcept
	{
		return __offset;
	}
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

		if (__regions.empty() && (__size >= size))
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

				if ((to - from) >= size)
				{
					retVal = from;
					break;
				}

				regionIt = nextIt;
			}
		}

		if (retVal.has_value())
			__regions.emplace(retVal.value(), size);

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

	Region::Region(RegionAllocator &allocator, const size_t size, const size_t alignment) :
		__allocator		{ allocator },
		__size			{ size },
		__alignment		{ alignment }
	{
		__create();
	}

	Region::~Region() noexcept
	{
		__allocator.free(__offset);
	}

	void Region::__create()
	{
		const auto offset{ __allocator.allocate(__size, __alignment) };
		if (offset.has_value())
		{
			__offset = offset.value();
			return;
		}

		throw std::bad_alloc{ };
	}
}
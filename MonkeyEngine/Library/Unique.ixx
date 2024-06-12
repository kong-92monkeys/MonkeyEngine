export module ntmonkeys.com.Lib.Unique;

import ntmonkeys.com.Lib.TimelineIdAllocator;

namespace Lib
{
	export class Unique
	{
	public:
		Unique() = default;
		Unique(const Unique &) = delete;
		Unique(Unique &&) = delete;

		virtual ~Unique() noexcept = default;

		Unique &operator=(const Unique &) = delete;
		Unique &operator=(Unique &&) = delete;
	};

	export class UniqueId
	{
	public:
		UniqueId() noexcept;
		UniqueId(const UniqueId &) = delete;
		UniqueId(UniqueId &&) = delete;

		virtual ~UniqueId() noexcept = default;

		UniqueId &operator=(const UniqueId &) = delete;
		UniqueId &operator=(UniqueId &&) = delete;

		[[nodiscard]]
		constexpr uint64_t getUID() const noexcept;

	private:
		const uint64_t __uid;
		static inline TimelineIdAllocator<true> __idAllocator;
	};

	constexpr uint64_t UniqueId::getUID() const noexcept
	{
		return __uid;
	}
}

module: private;

namespace Lib
{
	UniqueId::UniqueId() noexcept : __uid{ __idAllocator.allocate() }
	{}
}
export module ntmonkeys.com.Lib.MathUtil;

import <concepts>;

export namespace Lib::MathUtil
{
	template <std::integral $T>
	constexpr $T getGCDOf(const $T lhs, const $T rhs) noexcept
	{
		$T lIter{ lhs };
		$T rIter{ rhs };

		while (rIter)
		{
			const $T remainder{ lIter % rIter };
			lIter = rIter;
			rIter = remainder;
		}

		return lIter;
	}

	template <std::integral $T>
	constexpr $T getLCMOf(const $T lhs, const $T rhs) noexcept
	{
		return (rhs * (lhs / getGCDOf(lhs, rhs)));
	}

	template <std::integral $T>
	constexpr $T ceilAlign(const $T value, const $T alignment) noexcept
	{
		return (value + ((alignment - (value % alignment)) % alignment));
	}

	template <std::integral $T>
	constexpr $T floorAlign(const $T value, const $T alignment) noexcept
	{
		return (value - (value % alignment));
	}
}
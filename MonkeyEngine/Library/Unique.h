#pragma once

#include "TimelineIdAllocator.h"

namespace Lib
{
	class Unique
	{
	public:
		Unique() = default;
		Unique(const Unique &) = delete;
		Unique(Unique &&) = delete;

		virtual ~Unique() noexcept = default;

		Unique &operator=(const Unique &) = delete;
		Unique &operator=(Unique &&) = delete;
	};

	class UniqueId
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
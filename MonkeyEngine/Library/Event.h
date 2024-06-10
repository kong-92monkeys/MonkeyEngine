#pragma once

#include "Unique.h"
#include <functional>
#include <memory>
#include <unordered_map>
#include <stdexcept>

namespace Lib
{
	template <typename ...$Args>
	class EventListener;

	template <typename ...$Args>
	using EventListenerPtr = std::shared_ptr<EventListener<$Args...>>;

	template <typename ...$Args>
	using EventListenerWPtr = std::weak_ptr<EventListener<$Args...>>;

	template <typename ...$Args>
	using EventCallback = std::function<void(const $Args &...)>;

	template <typename ...$Args>
	class EventListener : public UniqueId
	{
	public:
		EventListener(EventCallback<$Args...> &&callback) noexcept;
		virtual ~EventListener() noexcept = default;

		void send(const $Args &...args);

		[[nodiscard]]
		static EventListenerPtr<$Args...> make(EventCallback<$Args...> &&callback) noexcept;

		template <typename ...$Params>
		[[nodiscard]]
		static EventListenerPtr<$Args...> bind($Params &&...params) noexcept;

	private:
		const EventCallback<$Args...> __callbackFunc;
	};

	template <typename ...$Args>
	class EventView : public Unique
	{
	public:
		virtual void addListener(const EventListenerPtr<$Args...> &pListener) noexcept = 0;
		virtual void removeListener(const EventListenerPtr<$Args...> &pListener) = 0;

		EventView &operator+=(const EventListenerPtr<$Args...> &pListener) noexcept;
		EventView &operator-=(const EventListenerPtr<$Args...> &pListener);
	};

	template <typename ...$Args>
	class Event : public EventView<$Args...>
	{
	public:
		virtual void addListener(const EventListenerPtr<$Args...> &pListener) noexcept override;
		virtual void removeListener(const EventListenerPtr<$Args...> &pListener) override;

		void invoke(const $Args &...args);

	private:
		std::unordered_map<uint64_t, std::pair<size_t, EventListenerWPtr<$Args...>>> __refMap;
	};

	template <typename ...$Args>
	EventListener<$Args...>::EventListener(EventCallback<$Args...> &&callback) noexcept :
		__callbackFunc{ std::move(callback) }
	{}

	template <typename ...$Args>
	void EventListener<$Args...>::send(const $Args &...args)
	{
		__callbackFunc(args...);
	}

	template <typename ...$Args>
	EventListenerPtr<$Args...> EventListener<$Args...>::make(EventCallback<$Args...> &&callback) noexcept
	{
		return std::make_shared<EventListener<$Args...>>(std::move(callback));
	}

	template <typename ...$Args>
	template <typename ...$Params>
	static EventListenerPtr<$Args...> EventListener<$Args...>::bind($Params &&...params) noexcept
	{
		return make(std::bind(std::forward<$Params>(params)...));
	}

	template <typename ...$Args>
	EventView<$Args...> &EventView<$Args...>::operator+=(const EventListenerPtr<$Args...> &pListener) noexcept
	{
		addListener(pListener);
		return *this;
	}

	template <typename ...$Args>
	EventView<$Args...> &EventView<$Args...>::operator-=(const EventListenerPtr<$Args...> &pListener)
	{
		removeListener(pListener);
		return *this;
	}

	template <typename ...$Args>
	void Event<$Args...>::addListener(const EventListenerPtr<$Args...> &pListener) noexcept
	{
		auto &[refCount, ref]{ __refMap[pListener->getUID()] };
		++refCount;
		ref = pListener;
	}

	template <typename ...$Args>
	void Event<$Args...>::removeListener(const EventListenerPtr<$Args...> &pListener)
	{
		auto &[refCount, ref] { __refMap.at(pListener->getUID()) };
		if (!refCount)
			throw std::runtime_error{ "The listener is already removed." };

		--refCount;
	}

	template <typename ...$Args>
	void Event<$Args...>::invoke(const $Args &...args)
	{
		for (auto it{ __refMap.begin() }; it != __refMap.end(); )
		{
			const auto &[refCount, ref]{ it->second };
			
			if (!refCount || ref.expired())
			{
				it = __refMap.erase(it);
				continue;
			}

			const auto pListener{ ref.lock() };
			pListener->send(args...);
			it++;
		}
	}
}
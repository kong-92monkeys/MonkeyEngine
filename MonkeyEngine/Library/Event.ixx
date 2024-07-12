export module ntmonkeys.com.Lib.Event;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.WeakReferenceSet;
import <functional>;
import <memory>;
import <stdexcept>;

namespace Lib
{
	template <typename ...$Args>
	class EventListener;

	export template <typename ...$Args>
	using EventListenerPtr = std::shared_ptr<EventListener<$Args...>>;

	export template <typename ...$Args>
	using EventCallback = std::function<void(const $Args &...)>;

	export template <typename ...$Args>
	class EventListener : public UniqueId
	{
	public:
		EventListener(EventCallback<$Args...> &&callback) noexcept;
		virtual ~EventListener() noexcept = default;

		void send(const $Args &...args) const;

		[[nodiscard]]
		static EventListenerPtr<$Args...> make(EventCallback<$Args...> &&callback) noexcept;

		template <typename ...$Params>
		[[nodiscard]]
		static EventListenerPtr<$Args...> bind($Params &&...params) noexcept;

	private:
		const EventCallback<$Args...> __callbackFunc;
	};

	export template <typename ...$Args>
	class EventView : public Unique
	{
	public:
		virtual void addListener(const EventListenerPtr<$Args...> &pListener) noexcept = 0;
		virtual void removeListener(const EventListenerPtr<$Args...> &pListener) = 0;

		EventView &operator+=(const EventListenerPtr<$Args...> &pListener) noexcept;
		EventView &operator-=(const EventListenerPtr<$Args...> &pListener);
	};

	export template <typename ...$Args>
	class Event : public EventView<$Args...>
	{
	public:
		virtual void addListener(const EventListenerPtr<$Args...> &pListener) noexcept override;
		virtual void removeListener(const EventListenerPtr<$Args...> &pListener) override;

		void invoke(const $Args &...args);

	private:
		WeakReferenceSet<EventListener<$Args...>> __listeners;
	};

	template <typename ...$Args>
	template <typename ...$Params>
	EventListenerPtr<$Args...> EventListener<$Args...>::bind($Params &&...params) noexcept
	{
		return make(std::bind(std::forward<$Params>(params)...));
	}
}

module: private;

namespace Lib
{
	template <typename ...$Args>
	EventListener<$Args...>::EventListener(EventCallback<$Args...> &&callback) noexcept :
		__callbackFunc{ std::move(callback) }
	{}

	template <typename ...$Args>
	void EventListener<$Args...>::send(const $Args &...args) const
	{
		__callbackFunc(args...);
	}

	template <typename ...$Args>
	EventListenerPtr<$Args...> EventListener<$Args...>::make(EventCallback<$Args...> &&callback) noexcept
	{
		return std::make_shared<EventListener<$Args...>>(std::move(callback));
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
		__listeners.emplace(pListener);
	}

	template <typename ...$Args>
	void Event<$Args...>::removeListener(const EventListenerPtr<$Args...> &pListener)
	{
		__listeners.erase(pListener);
	}

	template <typename ...$Args>
	void Event<$Args...>::invoke(const $Args &...args)
	{
		for (const auto &listener : __listeners)
			listener.send(args...);
	}
}
export module ntmonkeys.com.Lib.Stateful;

import ntmonkeys.com.Lib.Event;

namespace Lib
{
	export template <typename $T>
	class Stateful
	{
	public:
		void validate();

		[[nodiscard]]
		constexpr Lib::EventView<const $T *> &getInvalidateEvent() const noexcept;

	protected:
		void _invalidate() noexcept;
		virtual void _onValidate();

	private:
		bool __invalidated{ };

		mutable Lib::Event<const $T *> __invalidateEvent;
	};

	template <typename $T>
	void Stateful<$T>::validate()
	{
		if (!__invalidated)
			return;

		_onValidate();
		__invalidated = false;
	}

	template <typename $T>
	constexpr Lib::EventView<const $T *> &Stateful<$T>::getInvalidateEvent() const noexcept
	{
		return __invalidateEvent;
	}

	template <typename $T>
	void Stateful<$T>::_invalidate() noexcept
	{
		__invalidated = true;
		__invalidateEvent.invoke(static_cast<$T *>(this));
	}

	template <typename $T>
	void Stateful<$T>::_onValidate() {}
}
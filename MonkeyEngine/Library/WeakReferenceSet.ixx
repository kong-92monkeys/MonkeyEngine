export module ntmonkeys.com.Lib.WeakReferenceSet;

import <unordered_map>;
import <memory>;
import <stdexcept>;

namespace Lib
{
	export template <typename $T>
	class WeakReferenceSet
	{
	public:
		using Container = std::unordered_map<$T *, std::weak_ptr<$T>>;

		class Iterator
		{
		public:
			Iterator(Container &references, Container::iterator &&iter) noexcept;

			void operator++();

			[[nodiscard]]
			$T &operator*() const noexcept;

			[[nodiscard]]
			constexpr bool operator!=(const Iterator &another) const noexcept;

		private:
			Container &__references;
			Container::iterator __iter;
		};

		void emplace(const std::shared_ptr<$T> &ptr);
		void erase(const std::shared_ptr<$T> &ptr);

		[[nodiscard]]
		Iterator begin() noexcept;

		[[nodiscard]]
		Iterator end() noexcept;

	private:
		Container __references;
	};

	template <typename $T>
	void WeakReferenceSet<$T>::emplace(const std::shared_ptr<$T> &ptr)
	{
		__references.emplace(ptr.get(), ptr);
	}

	template <typename $T>
	void WeakReferenceSet<$T>::erase(const std::shared_ptr<$T> &ptr)
	{
		__references.erase(ptr.get());
	}

	template <typename $T>
	WeakReferenceSet<$T>::Iterator WeakReferenceSet<$T>::begin() noexcept
	{
		return { __references, __references.begin() };
	}

	template <typename $T>
	WeakReferenceSet<$T>::Iterator WeakReferenceSet<$T>::end() noexcept
	{
		return { __references, __references.end() };
	}

	template <typename $T>
	WeakReferenceSet<$T>::Iterator::Iterator(Container &references, Container::iterator &&iter) noexcept :
		__references{ references }, __iter{ iter }
	{}

	template <typename $T>
	void WeakReferenceSet<$T>::Iterator::operator++()
	{
		if (__iter == __references.end())
			throw std::out_of_range{ "Out of range" };

		while (__iter != __references.end())
		{
			const auto &[ptr, weakPtr]{ *__iter };

			if (weakPtr.expired())
				__iter = __references.erase(__iter);
			else
			{
				++__iter;
				break;
			}
		}
	}

	template <typename $T>
	$T &WeakReferenceSet<$T>::Iterator::operator*() const noexcept
	{
		return *(__iter->first);
	}

	template <typename $T>
	constexpr bool WeakReferenceSet<$T>::Iterator::operator!=(const Iterator &another) const noexcept
	{
		return (__iter != another.__iter);
	}
}
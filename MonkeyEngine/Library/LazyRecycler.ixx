export module ntmonkeys.com.Lib.LazyRecycler;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.LazyDeleter;
import <memory>;
import <deque>;
import <functional>;

namespace Lib
{
	export template <typename $T>
	class LazyRecycler : public Unique
	{
	public:
		explicit LazyRecycler(LazyDeleter &deleter) noexcept;

		[[nodiscard]]
		std::shared_ptr<$T> retrieve() noexcept;

		[[nodiscard]]
		std::shared_ptr<$T> retrieveWhere(std::function<bool(const $T &)> &&test) noexcept;

		void recycle(std::shared_ptr<$T> &&pResource) noexcept;

	private:
		class __ResourceHolder : public Unique
		{
		public:
			__ResourceHolder(std::deque<std::shared_ptr<$T>> &resources, std::shared_ptr<$T> &&resource) noexcept;
			virtual ~__ResourceHolder() noexcept override;

		private:
			std::deque<std::shared_ptr<$T>> &__resources;
			std::shared_ptr<$T> __pResource;
		};

		LazyDeleter &__deleter;
		std::deque<std::shared_ptr<$T>> __resources;
	};

	template <typename $T>
	LazyRecycler<$T>::LazyRecycler(LazyDeleter &deleter) noexcept :
		__deleter{ deleter }
	{}

	template <typename $T>
	std::shared_ptr<$T> LazyRecycler<$T>::retrieve() noexcept
	{
		if (__resources.empty())
			return nullptr;

		auto pFront{ std::move(__resources.front()) };
		__resources.pop_front();

		return pFront;
	}

	template <typename $T>
	std::shared_ptr<$T> LazyRecycler<$T>::retrieveWhere(std::function<bool(const $T &)> &&test) noexcept
	{
		if (__resources.empty())
			return nullptr;

		std::shared_ptr<$T> pRetVal;

		for (auto iter{ __resources.begin() }; iter != __resources.end(); ++iter)
		{
			if (!(test(*iter)))
				continue;

			pRetVal = std::move(*iter);
			__resources.erase(iter);
			break;
		}

		return pRetVal;
	}

	template <typename $T>
	void LazyRecycler<$T>::recycle(std::shared_ptr<$T> &&pResource) noexcept
	{
		__deleter.reserve(std::make_shared<__ResourceHolder>(__resources, std::move(pResource)));
	}

	template <typename $T>
	LazyRecycler<$T>::__ResourceHolder::__ResourceHolder(
		std::deque<std::shared_ptr<$T>> &resources, std::shared_ptr<$T> &&pResource) noexcept :
		__resources{ resources }, __pResource{ std::move(pResource) }
	{}

	template <typename $T>
	LazyRecycler<$T>::__ResourceHolder::~__ResourceHolder() noexcept
	{
		__resources.emplace_back(std::move(__pResource));
	}
}
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
		using __ResourceContainer = std::deque<std::shared_ptr<$T>>;

		class __ResourceHolder : public Unique
		{
		public:
			__ResourceHolder(const std::weak_ptr<__ResourceContainer> &pContainer, std::shared_ptr<$T> &&resource) noexcept;
			virtual ~__ResourceHolder() noexcept override;

		private:
			const std::weak_ptr<__ResourceContainer> __pContainer;
			std::shared_ptr<$T> __pResource;
		};

		LazyDeleter &__deleter;
		std::shared_ptr<__ResourceContainer> __pResourceContainer;
	};

	template <typename $T>
	LazyRecycler<$T>::LazyRecycler(LazyDeleter &deleter) noexcept :
		__deleter{ deleter }
	{
		__pResourceContainer = std::make_shared<std::deque<std::shared_ptr<$T>>>();
	}

	template <typename $T>
	std::shared_ptr<$T> LazyRecycler<$T>::retrieve() noexcept
	{
		if (__pResourceContainer->empty())
			return nullptr;

		auto pFront{ std::move(__pResourceContainer->front()) };
		__pResourceContainer->pop_front();

		return pFront;
	}

	template <typename $T>
	std::shared_ptr<$T> LazyRecycler<$T>::retrieveWhere(std::function<bool(const $T &)> &&test) noexcept
	{
		if (__pResourceContainer->empty())
			return nullptr;

		std::shared_ptr<$T> pRetVal;

		for (auto iter{ __pResourceContainer->begin() }; iter != __pResourceContainer->end(); ++iter)
		{
			if (!(test(*(*iter))))
				continue;

			pRetVal = std::move(*iter);
			__pResourceContainer->erase(iter);
			break;
		}

		return pRetVal;
	}

	template <typename $T>
	void LazyRecycler<$T>::recycle(std::shared_ptr<$T> &&pResource) noexcept
	{
		__deleter.reserve(std::make_shared<__ResourceHolder>(__pResourceContainer, std::move(pResource)));
	}

	template <typename $T>
	LazyRecycler<$T>::__ResourceHolder::__ResourceHolder(
		const std::weak_ptr<__ResourceContainer> &pContainer, std::shared_ptr<$T> &&pResource) noexcept :
		__pContainer{ pContainer }, __pResource{ std::move(pResource) }
	{}

	template <typename $T>
	LazyRecycler<$T>::__ResourceHolder::~__ResourceHolder() noexcept
	{
		const auto pContainer{ __pContainer.lock() };
		if (!pContainer)
			return;

		pContainer->emplace_back(std::move(__pResource));
	}
}
export module ntmonkeys.com.Frameworks.RenderInterface;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.ThreadPool;
import <functional>;
import <cstdlib>;

namespace Frameworks
{
	export template <typename $Impl>
	class RenderInterface : public Lib::Unique
	{
	public:
		struct InitInfo
		{
		public:
			Lib::ThreadPool *pRcmdExecutor{ };
			std::function<void(void *)> implInstantiator;
		};

		virtual ~RenderInterface() noexcept override;

		void init(const InitInfo &initInfo);

		[[nodiscard]]
		constexpr $Impl &getImpl() noexcept;

		[[nodiscard]]
		constexpr const $Impl &getImpl() const noexcept;

	protected:
		void _withImpl(std::function<void($Impl &)> &&logic);
		void _withImpl(std::function<void(const $Impl &)> &&logic) const;

	private:
		Lib::ThreadPool *__pRcmdExecutor{ };
		void *const __pImpl{ _aligned_malloc(sizeof($Impl), sizeof($Impl)) };
	};

	template <typename $Impl>
	RenderInterface<$Impl>::~RenderInterface() noexcept
	{
		__pRcmdExecutor->silentRun([pImpl{ __pImpl }]
		{
			$Impl *const pCasted{ reinterpret_cast<$Impl *>(pImpl) };
			pCasted->~$Impl();
			_aligned_free(pImpl);
		});
	}

	template <typename $Impl>
	constexpr $Impl &RenderInterface<$Impl>::getImpl() noexcept
	{
		return *(reinterpret_cast<$Impl *>(__pImpl));
	}

	template <typename $Impl>
	constexpr const $Impl &RenderInterface<$Impl>::getImpl() const noexcept
	{
		return *(reinterpret_cast<$Impl *>(__pImpl));
	}

	template <typename $Impl>
	void RenderInterface<$Impl>::_withImpl(std::function<void($Impl &)> &&logic)
	{
		__pRcmdExecutor->silentRun([logic{ std::move(logic) }, &impl{ getImpl() }]
		{
			logic(impl);
		});
	}

	template <typename $Impl>
	void RenderInterface<$Impl>::_withImpl(std::function<void(const $Impl &)> &&logic) const
	{
		__pRcmdExecutor->silentRun([logic{ std::move(logic) }, &impl{ getImpl() }]
		{
			logic(impl);
		});
	}

	template <typename $Impl>
	void RenderInterface<$Impl>::init(const InitInfo &initInfo)
	{
		__pRcmdExecutor = initInfo.pRcmdExecutor;
		__pRcmdExecutor->silentRun([implInstantiator{ std::move(initInfo.implInstantiator) }, pImpl{ __pImpl }]
		{
			implInstantiator(pImpl);
		});
	}
}
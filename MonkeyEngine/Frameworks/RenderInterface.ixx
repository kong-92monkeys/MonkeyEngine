export module ntmonkeys.com.Frameworks.RenderInterface;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.ThreadPool;
import <functional>;
import <cstdlib>;

namespace Frameworks
{
	export struct RenderInterfaceInitInfo
	{
	public:
		Lib::ThreadPool *pRcmdExecutor{ };
	};

	export template <typename $Impl>
	class RenderInterface : public Lib::Unique
	{
	public:
		virtual ~RenderInterface() noexcept override;

		void init(const RenderInterfaceInitInfo &initInfo);

		[[nodiscard]]
		constexpr $Impl &getImpl() noexcept;

		[[nodiscard]]
		constexpr const $Impl &getImpl() const noexcept;

	protected:
		void _initImpl(std::function<void(void *)> &&logic);

		void _withImpl(std::function<void($Impl &)> &&logic);
		void _withImpl(std::function<void(const $Impl &)> &&logic) const;

		virtual void _onInit() = 0;

	private:
		Lib::ThreadPool *__pRcmdExecutor{ };
		void *const __pImplPlaceholder{ _aligned_malloc(sizeof($Impl), sizeof($Impl)) };
	};

	template <typename $Impl>
	RenderInterface<$Impl>::~RenderInterface() noexcept
	{
		__pRcmdExecutor->silentRun([pImplPlaceholder{ __pImplPlaceholder }]
		{
			$Impl *const pImpl{ reinterpret_cast<$Impl *>(pImplPlaceholder) };
			pImpl->~$Impl();
			_aligned_free(pImplPlaceholder);
		});
	}

	template <typename $Impl>
	constexpr $Impl &RenderInterface<$Impl>::getImpl() noexcept
	{
		$Impl *const pImpl{ reinterpret_cast<$Impl *>(__pImplPlaceholder) };
		return *pImpl;
	}

	template <typename $Impl>
	constexpr const $Impl &RenderInterface<$Impl>::getImpl() const noexcept
	{
		$Impl *const pImpl{ reinterpret_cast<$Impl *>(__pImplPlaceholder) };
		return *pImpl;
	}

	template <typename $Impl>
	void RenderInterface<$Impl>::_initImpl(std::function<void(void *)> &&logic)
	{
		__pRcmdExecutor->silentRun([logic{ std::move(logic) }, pImplPlaceholder{ __pImplPlaceholder }]
		{
			logic(pImplPlaceholder);
		});
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
	void RenderInterface<$Impl>::init(const RenderInterfaceInitInfo &initInfo)
	{
		__pRcmdExecutor = initInfo.pRcmdExecutor;
		_onInit();
	}
}
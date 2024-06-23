module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderTarget;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Surface;
import ntmonkeys.com.Graphics.Swapchain;
import <memory>;

namespace Engine
{
	export class RenderTarget : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			Graphics::LogicalDevice *pLogicalDevice{ };
			HINSTANCE hinstance{ };
			HWND hwnd{ };
		};

		explicit RenderTarget(const CreateInfo &createInfo);
		virtual ~RenderTarget() noexcept override;

		void sync();

		[[nodiscard]]
		constexpr uint32_t getWidth() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getHeight() const noexcept;

		[[nodiscard]]
		constexpr bool isPresentable() const noexcept;

	private:
		Graphics::LogicalDevice &__logicalDevice;
		std::unique_ptr<Graphics::Surface> __pSurface;
		std::unique_ptr<Graphics::Swapchain> __pSwapchain;

		void __validateSwapchainDependencies();
	};

	constexpr uint32_t RenderTarget::getWidth() const noexcept
	{
		return __pSurface->getWidth();
	}

	constexpr uint32_t RenderTarget::getHeight() const noexcept
	{
		return __pSurface->getHeight();
	}

	constexpr bool RenderTarget::isPresentable() const noexcept
	{
		return (getWidth() && getHeight());
	}
}

module: private;

namespace Engine
{
	RenderTarget::RenderTarget(const CreateInfo &createInfo) :
		__logicalDevice{ *(createInfo.pLogicalDevice) }
	{
		__pSurface = __logicalDevice.createSurface(createInfo.hinstance, createInfo.hwnd);
		__validateSwapchainDependencies();
	}

	RenderTarget::~RenderTarget() noexcept
	{
		__pSwapchain = nullptr;
		__pSurface = nullptr;
	}

	void RenderTarget::sync()
	{
		__pSurface->sync();
		__validateSwapchainDependencies();
	}

	void RenderTarget::__validateSwapchainDependencies()
	{
		auto pOldSwapchain{ std::move(__pSwapchain) };

		if (!(isPresentable()))
			return;

		__pSwapchain = __pSurface->createSwapchain(VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT, true, std::move(pOldSwapchain));
	}
}
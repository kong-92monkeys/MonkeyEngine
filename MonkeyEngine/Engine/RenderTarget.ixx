module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderTarget;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Surface;
import ntmonkeys.com.Graphics.Swapchain;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.FramebufferFactory;
import ntmonkeys.com.Engine.SemaphoreCirculator;
import ntmonkeys.com.Engine.Layer;
import <memory>;
import <unordered_set>;
import <limits>;

namespace Engine
{
	export class RenderTarget : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			Graphics::LogicalDevice *pLogicalDevice{ };
			const RenderPassFactory *pRenderPassFactory{ };
			HINSTANCE hinstance{ };
			HWND hwnd{ };
		};

		explicit RenderTarget(const CreateInfo &createInfo);
		virtual ~RenderTarget() noexcept override;

		[[nodiscard]]
		constexpr uint32_t getWidth() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getHeight() const noexcept;

		[[nodiscard]]
		constexpr bool isPresentable() const noexcept;

		void addLayer(Layer &layer) noexcept;
		void removeLayer(Layer &layer) noexcept;

		void sync();
		void draw(Graphics::CommandBuffer &commandBuffer);

	private:
		Graphics::LogicalDevice &__logicalDevice;
		const RenderPassFactory &__renderPassFactory;

		std::unique_ptr<Graphics::Surface> __pSurface;
		std::unique_ptr<Graphics::Swapchain> __pSwapchain;

		std::unique_ptr<FramebufferFactory> __pFramebufferFactory;
		std::unique_ptr<SemaphoreCirculator> __pSemaphoreCirculator;

		std::unordered_set<Layer *> __layers;

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
		__logicalDevice		{ *(createInfo.pLogicalDevice) },
		__renderPassFactory	{ *(createInfo.pRenderPassFactory) }
	{
		__pSurface = std::unique_ptr<Graphics::Surface>{ __logicalDevice.createSurface(createInfo.hinstance, createInfo.hwnd) };

		__pFramebufferFactory = std::make_unique<FramebufferFactory>(__logicalDevice, __renderPassFactory);
		__pSemaphoreCirculator = std::make_unique<SemaphoreCirculator>(__logicalDevice, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, 10ULL);

		__validateSwapchainDependencies();
	}

	RenderTarget::~RenderTarget() noexcept
	{
		__pSemaphoreCirculator = nullptr;
		__pFramebufferFactory = nullptr;
		__pSwapchain = nullptr;
		__pSurface = nullptr;
	}

	void RenderTarget::addLayer(Layer &layer) noexcept
	{
		__layers.emplace(&layer);
	}

	void RenderTarget::removeLayer(Layer &layer) noexcept
	{
		__layers.erase(&layer);
	}

	void RenderTarget::sync()
	{
		__pSurface->sync();
		__validateSwapchainDependencies();
	}

	void RenderTarget::draw(Graphics::CommandBuffer &commandBuffer)
	{
		auto &imageAcquireSemaphore{ __pSemaphoreCirculator->getNext() };

		const uint32_t imageIdx
		{
			__pSwapchain->acquireNextImage(
				UINT64_MAX, imageAcquireSemaphore.getHandle(), VK_NULL_HANDLE)
		};

		auto &imageView{ __pSwapchain->getImageView(imageIdx) };

		const VkRect2D renderArea
		{
			.offset	{ 0, 0 },
			.extent	{ getWidth(), getHeight() }
		};

		const Layer::DrawInfo drawInfo
		{
			.pImageView					{ &imageView },
			.renderArea					{ renderArea },
			.pFramebufferFactory		{ __pFramebufferFactory.get() }
		};

		for (const auto pLayer : __layers)
			pLayer->draw(drawInfo, commandBuffer);
	}

	void RenderTarget::__validateSwapchainDependencies()
	{
		auto pOldSwapchain{ std::move(__pSwapchain) };

		if (!(isPresentable()))
			return;

		__pSwapchain = std::unique_ptr<Graphics::Swapchain>
		{
			__pSurface->createSwapchain(
				VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				true, pOldSwapchain.get())
		};

		__pFramebufferFactory->invalidate(getWidth(), getHeight());
	}
}
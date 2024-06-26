module;

#include "../Library/GLM.h"
#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderTarget;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.WeakReferenceSet;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Surface;
import ntmonkeys.com.Graphics.Swapchain;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.Semaphore;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.FramebufferFactory;
import ntmonkeys.com.Engine.SemaphoreCirculator;
import ntmonkeys.com.Engine.Layer;
import ntmonkeys.com.Engine.Constants;
import <memory>;
import <unordered_map>;
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

		struct DrawResult
		{
		public:
			Graphics::Semaphore *pImgAcquireSemaphore{ };
			uint32_t imageIndex{ };
		};

		struct PresentInfo
		{
		public:
			uint32_t waitSemaphoreCount{ };
			const VkSemaphore *pWaitSemaphores{ };
			uint32_t imageIdx{ };
		};

		explicit RenderTarget(const CreateInfo &createInfo);
		virtual ~RenderTarget() noexcept override;

		[[nodiscard]]
		constexpr uint32_t getWidth() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getHeight() const noexcept;

		[[nodiscard]]
		constexpr bool isPresentable() const noexcept;

		[[nodiscard]]
		constexpr const VkSwapchainKHR &getSwapchainHandle() noexcept;

		void addLayer(const std::shared_ptr<Layer> &pLayer) noexcept;
		void removeLayer(const std::shared_ptr<Layer> &pLayer) noexcept;

		constexpr void setBackgroundColor(const glm::vec4 &color) noexcept;

		void sync();

		[[nodiscard]]
		DrawResult draw(Graphics::CommandBuffer &commandBuffer);

	private:
		Graphics::LogicalDevice &__logicalDevice;
		const RenderPassFactory &__renderPassFactory;

		std::unique_ptr<Graphics::Surface> __pSurface;
		std::unique_ptr<Graphics::Swapchain> __pSwapchain;

		std::unique_ptr<FramebufferFactory> __pFramebufferFactory;
		std::unique_ptr<SemaphoreCirculator> __pImgAcquireSemaphoreCirculator;

		Lib::WeakReferenceSet<Layer> __layers;

		glm::vec4 __backgroundColor{ 0.01f, 0.01f, 0.01f, 1.0f };

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

	constexpr const VkSwapchainKHR &RenderTarget::getSwapchainHandle() noexcept
	{
		return __pSwapchain->getHandle();
	}

	constexpr void RenderTarget::setBackgroundColor(const glm::vec4 &color) noexcept
	{
		__backgroundColor = color;
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

		// max in-flight 도달 이후 다음 프레임까지 acquire 요청할 수 있으므로 limit에 1 추가해야 함.
		__pImgAcquireSemaphoreCirculator = std::make_unique<SemaphoreCirculator>(
			__logicalDevice, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT + 1ULL);

		__validateSwapchainDependencies();
	}

	RenderTarget::~RenderTarget() noexcept
	{
		auto &queue{ __logicalDevice.getQueue() };
		queue.waitIdle();

		__pImgAcquireSemaphoreCirculator = nullptr;
		__pFramebufferFactory = nullptr;
		__pSwapchain = nullptr;
		__pSurface = nullptr;
	}

	void RenderTarget::addLayer(const std::shared_ptr<Layer> &pLayer) noexcept
	{
		__layers.emplace(pLayer);
	}

	void RenderTarget::removeLayer(const std::shared_ptr<Layer> &pLayer) noexcept
	{
		__layers.erase(pLayer);
	}

	void RenderTarget::sync()
	{
		auto &queue{ __logicalDevice.getQueue() };
		queue.waitIdle();

		__pSurface->sync();
		__validateSwapchainDependencies();
	}

	RenderTarget::DrawResult RenderTarget::draw(Graphics::CommandBuffer &commandBuffer)
	{
		auto &imgAcquireSemaphore{ __pImgAcquireSemaphoreCirculator->getNext() };

		const uint32_t imageIdx
		{
			__pSwapchain->acquireNextImage(
				UINT64_MAX, imgAcquireSemaphore.getHandle(), VK_NULL_HANDLE)
		};

		auto &imageView{ __pSwapchain->getImageViewOf(imageIdx) };

		const VkRect2D renderArea
		{
			.offset	{ 0, 0 },
			.extent	{ getWidth(), getHeight() }
		};

		const Layer::DrawInfo drawInfo
		{
			.pImageView				{ &imageView },
			.renderArea				{ renderArea },
			.pClearColor			{ &__backgroundColor },
			.pFramebufferFactory	{ __pFramebufferFactory.get() }
		};

		const VkViewport viewport
		{
			.x			{ 0.0f },
			.y			{ 0.0f },
			.width		{ static_cast<float>(getWidth()) },
			.height		{ static_cast<float>(getHeight()) },
			.minDepth	{ 0.0f },
			.maxDepth	{ 1.0f }
		};

		commandBuffer.setViewport(0U, 1U, &viewport);
		commandBuffer.setScissor(0U, 1U, &renderArea);

		for (auto &layer : __layers)
			layer.draw(drawInfo, commandBuffer);

		return { &imgAcquireSemaphore, imageIdx };
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
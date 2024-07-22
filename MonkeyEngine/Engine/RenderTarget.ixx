module;

#include "../Library/GLM.h"
#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderTarget;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Lib.WeakReferenceSet;
import ntmonkeys.com.Sys.Environment;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Surface;
import ntmonkeys.com.Graphics.Swapchain;
import ntmonkeys.com.Graphics.Image;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.Semaphore;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.FramebufferFactory;
import ntmonkeys.com.Engine.CommandBufferCirculator;
import ntmonkeys.com.Engine.SemaphoreCirculator;
import ntmonkeys.com.Engine.Layer;
import ntmonkeys.com.Engine.Renderer;
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

		void validate();

		[[nodiscard]]
		DrawResult draw(Graphics::CommandBuffer &commandBuffer);

		[[nodiscard]]
		constexpr Lib::Event<const RenderTarget *> &getNeedRedrawEvent() const noexcept;

	private:
		Graphics::LogicalDevice &__logicalDevice;
		const RenderPassFactory &__renderPassFactory;

		std::unique_ptr<Graphics::Surface> __pSurface;
		std::unique_ptr<Graphics::Swapchain> __pSwapchain;

		std::unique_ptr<FramebufferFactory> __pFramebufferFactory;
		std::unique_ptr<SemaphoreCirculator> __pImgAcquireSemaphoreCirculator;

		std::vector<std::unique_ptr<CommandBufferCirculator>> __secondaryCBCirculators;

		Lib::WeakReferenceSet<Layer> __layers;

		glm::vec4 __backgroundColor{ 0.01f, 0.01f, 0.01f, 1.0f };

		Lib::EventListenerPtr<const Layer *> __pLayerNeedRedrawListener;

		mutable Lib::Event<const RenderTarget *> __needRedrawEvent;

		void __createSecondaryCBCirculators();

		void __validateSwapchainDependencies();
		void __clearImageColor(Graphics::CommandBuffer &commandBuffer, const Graphics::ImageView &imageView);
		void __transitImageToPresent(Graphics::CommandBuffer &commandBuffer, const Graphics::Image &image);

		void __onLayerRedrawNeeded() const noexcept;
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

	constexpr Lib::Event<const RenderTarget *> &RenderTarget::getNeedRedrawEvent() const noexcept
	{
		return __needRedrawEvent;
	}
}

module: private;

namespace Engine
{
	RenderTarget::RenderTarget(const CreateInfo &createInfo) :
		__logicalDevice		{ *(createInfo.pLogicalDevice) },
		__renderPassFactory	{ *(createInfo.pRenderPassFactory) }
	{
		__pLayerNeedRedrawListener = Lib::EventListener<const Layer *>::bind(&RenderTarget::__onLayerRedrawNeeded, this);

		__pSurface = std::unique_ptr<Graphics::Surface>{ __logicalDevice.createSurface(createInfo.hinstance, createInfo.hwnd) };
		__pFramebufferFactory = std::make_unique<FramebufferFactory>(__logicalDevice, __renderPassFactory);

		// max in-flight 도달 이후 다음 프레임까지 acquire 요청할 수 있으므로 limit에 1 추가해야 함.
		__pImgAcquireSemaphoreCirculator = std::make_unique<SemaphoreCirculator>(
			__logicalDevice, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT + 1ULL);

		__createSecondaryCBCirculators();
		__validateSwapchainDependencies();
	}

	RenderTarget::~RenderTarget() noexcept
	{
		auto &queue{ __logicalDevice.getQueue() };
		queue.waitIdle();

		__secondaryCBCirculators.clear();
		__pImgAcquireSemaphoreCirculator = nullptr;
		__pFramebufferFactory = nullptr;
		__pSwapchain = nullptr;
		__pSurface = nullptr;
	}

	void RenderTarget::addLayer(const std::shared_ptr<Layer> &pLayer) noexcept
	{
		__layers.emplace(pLayer);
		pLayer->getNeedRedrawEvent() += __pLayerNeedRedrawListener;
	}

	void RenderTarget::removeLayer(const std::shared_ptr<Layer> &pLayer) noexcept
	{
		__layers.erase(pLayer);
		pLayer->getNeedRedrawEvent() -= __pLayerNeedRedrawListener;
	}

	void RenderTarget::sync()
	{
		auto &queue{ __logicalDevice.getQueue() };
		queue.waitIdle();

		__pSurface->sync();
		__validateSwapchainDependencies();
	}

	void RenderTarget::validate()
	{
		for (auto &layer : __layers)
			layer.validate();
	}

	RenderTarget::DrawResult RenderTarget::draw(Graphics::CommandBuffer &commandBuffer)
	{
		auto &imgAcquireSemaphore{ __pImgAcquireSemaphoreCirculator->getNext() };

		const uint32_t imageIdx
		{
			__pSwapchain->acquireNextImage(
				UINT64_MAX, imgAcquireSemaphore.getHandle(), VK_NULL_HANDLE)
		};

		const auto &swapchainImage		{ __pSwapchain->getImageOf(imageIdx) };
		const auto &swapchainImageView	{ __pSwapchain->getImageViewOf(imageIdx) };

		__clearImageColor(commandBuffer, swapchainImageView);

		const VkRect2D renderArea
		{
			.offset	{ 0, 0 },
			.extent	{ getWidth(), getHeight() }
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

		const Renderer::BeginInfo rendererBeginInfo
		{
			.pSwapchainImageView	{ &swapchainImageView },
			.pRenderArea			{ &renderArea },
			.pFramebufferFactory	{ __pFramebufferFactory.get() }
		};

		for (const auto &layer : __layers)
			layer.draw(commandBuffer, rendererBeginInfo);

		__transitImageToPresent(commandBuffer, swapchainImage);
		return { &imgAcquireSemaphore, imageIdx };
	}

	void RenderTarget::__createSecondaryCBCirculators()
	{
		auto &env{ Sys::Environment::getInstance() };
		auto &threadPool{ env.getThreadPool() };

		const uint32_t threadCount{ threadPool.getThreadCount() };
		for (uint32_t threadIt{ }; threadIt < threadCount; ++threadIt)
		{
			__secondaryCBCirculators.emplace_back(
				std::make_unique<CommandBufferCirculator>(
					__logicalDevice, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_SECONDARY, 2U, 30U));
		}
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

	void RenderTarget::__clearImageColor(Graphics::CommandBuffer &commandBuffer, const Graphics::ImageView &imageView)
	{
		const auto &renderPass		{ __renderPassFactory.getInstance(Engine::RenderPassType::CLEAR_COLOR) };
		const auto &framebuffer		{ __pFramebufferFactory->getInstance(Engine::RenderPassType::CLEAR_COLOR) };

		const VkRenderPassAttachmentBeginInfo attachmentInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO },
			.attachmentCount	{ 1U },
			.pAttachments		{ &(imageView.getHandle()) }
		};

		VkClearValue clearValue{ };
		std::memcpy(&clearValue, &__backgroundColor, sizeof(glm::vec4));

		VkRenderPassBeginInfo renderPassBeginInfo{ };
		renderPassBeginInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.pNext				= &attachmentInfo;
		renderPassBeginInfo.renderPass			= renderPass.getHandle();
		renderPassBeginInfo.framebuffer			= framebuffer.getHandle();
		renderPassBeginInfo.clearValueCount		= 1U;
		renderPassBeginInfo.pClearValues		= &clearValue;

		auto &renderArea{ renderPassBeginInfo.renderArea };
		renderArea.offset.x			= 0;
		renderArea.offset.y			= 0;
		renderArea.extent.width		= getWidth();
		renderArea.extent.height	= getHeight();

		static constexpr VkSubpassBeginInfo subpassBeginInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO },
			.contents	{ VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE }
		};

		static constexpr VkSubpassEndInfo subpassEndInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		commandBuffer.beginRenderPass(renderPassBeginInfo, subpassBeginInfo);
		commandBuffer.endRenderPass(subpassEndInfo);
	}

	void RenderTarget::__transitImageToPresent(Graphics::CommandBuffer &commandBuffer, const Graphics::Image &image)
	{
		/*
			When transitioning the image to VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR or VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
			there is no need to delay subsequent processing, or perform any visibility operations
			(as vkQueuePresentKHR performs automatic visibility operations).
			
			To achieve this, the dstAccessMask member of the VkImageMemoryBarrier should be set to 0,
			and the dstStageMask parameter should be set to VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT.
		*/

		const VkImageMemoryBarrier2 imageBarrier
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 },
			.srcStageMask			{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.srcAccessMask			{ VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT },
			.dstStageMask			{ VK_PIPELINE_STAGE_2_NONE },
			.dstAccessMask			{ VK_ACCESS_2_NONE },
			.oldLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			.newLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR },
			.image					{ image.getHandle() },
			.subresourceRange		{
				.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
				.baseMipLevel		{ 0U },
				.levelCount			{ 1U },
				.baseArrayLayer		{ 0U },
				.layerCount			{ 1U }
			}
		};

		const VkDependencyInfo dependencyInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
			.imageMemoryBarrierCount	{ 1U },
			.pImageMemoryBarriers		{ &imageBarrier }
		};

		commandBuffer.pipelineBarrier(dependencyInfo);
	}

	void RenderTarget::__onLayerRedrawNeeded() const noexcept
	{
		__needRedrawEvent.invoke(this);
	}
}
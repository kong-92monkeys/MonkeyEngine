module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderingEngine;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Lib.AssetManager;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Framebuffer;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.Fence;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.RenderTarget;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Engine.LayerResourcePool;
import ntmonkeys.com.Engine.CommandExecutor;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.Layer;
import ntmonkeys.com.Engine.Mesh;
import ntmonkeys.com.Engine.RenderObject;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.CommandBufferCirculator;
import ntmonkeys.com.Engine.FenceCirculator;
import ntmonkeys.com.Engine.SemaphoreCirculator;
import ntmonkeys.com.Engine.Constants;
import <stdexcept>;
import <memory>;
import <unordered_set>;
import <concepts>;
import <format>;

namespace Engine
{
	export class RenderingEngine : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const Graphics::PhysicalDevice *pPhysicalDevice{ };
			const Lib::AssetManager *pAssetManager{ };
		};

		explicit RenderingEngine(const CreateInfo &createInfo);
		virtual ~RenderingEngine() noexcept override;

		[[nodiscard]]
		RenderTarget *createRenderTarget(const HINSTANCE hinstance, const HWND hwnd);

		[[nodiscard]]
		Layer *createLayer();

		[[nodiscard]]
		Mesh *createMesh() noexcept;

		template <std::derived_from<Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		$Renderer *createRenderer($Args &&...args);

		[[nodiscard]]
		RenderObject *createRenderObject() noexcept;

		void setMaxInFlightFrameCount(const size_t count);

		void render(RenderTarget &renderTarget);

	private:
		const Graphics::PhysicalDevice &__physicalDevice;
		const Lib::AssetManager &__assetManager;

		EngineContext __context;
		Lib::LazyDeleter __lazyDeleter{ 5ULL };
		CommandExecutor __commandExecutor;

		std::unique_ptr<Graphics::LogicalDevice> __pLogicalDevice;
		std::unique_ptr<MemoryAllocator> __pMemoryAllocator;
		std::unique_ptr<LayerResourcePool> __pLayerResourcePool;
		std::unique_ptr<RenderPassFactory> __pRenderPassFactory;

		std::unique_ptr<CommandBufferCirculator> __pCBCirculator;
		std::unique_ptr<FenceCirculator> __pSubmitFenceCirculator;
		std::unique_ptr<SemaphoreCirculator> __pSubmitSemaphoreCirculator;

		std::unique_ptr<Graphics::DescriptorSetLayout> __pRenderTargetDescSetLayout;

		size_t __maxInFlightFrameCount{ 3U };
		std::unordered_set<Graphics::Fence *> __inFlightFences;

		void __createRenderTargetDescSetLayout();

		[[nodiscard]]
		Graphics::Fence &__getSubmitFence() noexcept;
	};

	template <std::derived_from<Renderer> $Renderer, typename ...$Args>
	$Renderer *RenderingEngine::createRenderer($Args &&...args)
	{
		const Renderer::InitInfo initInfo
		{
			.pDevice						{ __pLogicalDevice.get() },
			.pAssetManager					{ &__assetManager },
			.pRenderPassFactory				{ __pRenderPassFactory.get() },
			.pRenderTargetDescSetLayout		{ __pRenderTargetDescSetLayout.get() }
		};

		const auto pRetVal{ new $Renderer{ std::forward<$Args>(args)... } };
		pRetVal->init(initInfo);
		return pRetVal;
	}
}

module: private;

namespace Engine
{
	RenderingEngine::RenderingEngine(const CreateInfo &createInfo) :
		__physicalDevice	{ *(createInfo.pPhysicalDevice) },
		__assetManager		{ *(createInfo.pAssetManager) }
	{
		const auto &deviceLimits{ __physicalDevice.get10Props().limits };

		__pLogicalDevice = std::unique_ptr<Graphics::LogicalDevice>{ __physicalDevice.createLogicalDevice() };
		
		__pMemoryAllocator = std::make_unique<MemoryAllocator>(
			__physicalDevice, *__pLogicalDevice,
			Constants::DEFAULT_MEMORY_BLOCK_SIZE, Constants::DEFAULT_BUFFER_BLOCK_SIZE,
			deviceLimits.minUniformBufferOffsetAlignment, deviceLimits.minStorageBufferOffsetAlignment);
		
		__pLayerResourcePool = std::make_unique<LayerResourcePool>(__lazyDeleter, *__pMemoryAllocator);
		__pRenderPassFactory = std::make_unique<RenderPassFactory>(*__pLogicalDevice);

		__pCBCirculator = std::make_unique<CommandBufferCirculator>(
			*__pLogicalDevice, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 30U);

		__pSubmitFenceCirculator = std::make_unique<FenceCirculator>(*__pLogicalDevice, Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT);
		__pSubmitSemaphoreCirculator = std::make_unique<SemaphoreCirculator>(
			*__pLogicalDevice, VkSemaphoreType::VK_SEMAPHORE_TYPE_BINARY, Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT);

		__createRenderTargetDescSetLayout();

		__context.pLazyDeleter				= &__lazyDeleter;
		__context.pLogicalDevice			= __pLogicalDevice.get();
		__context.pCommandExecutor			= &__commandExecutor;
		__context.pMemoryAllocator			= __pMemoryAllocator.get();
		__context.pLayerResourcePool		= __pLayerResourcePool.get();
		__context.pRenderPassFactory		= __pRenderPassFactory.get();
	}

	RenderingEngine::~RenderingEngine() noexcept
	{
		__lazyDeleter.flush();
		__pLogicalDevice->waitIdle();

		__pRenderTargetDescSetLayout = nullptr;
		__pSubmitSemaphoreCirculator = nullptr;
		__pSubmitFenceCirculator = nullptr;
		__pCBCirculator = nullptr;
		__pRenderPassFactory = nullptr;
		__pLayerResourcePool = nullptr;
		__pMemoryAllocator = nullptr;
		__pLogicalDevice = nullptr;
	}

	RenderTarget *RenderingEngine::createRenderTarget(const HINSTANCE hinstance, const HWND hwnd)
	{
		const RenderTarget::CreateInfo createInfo
		{
			.pLogicalDevice		{ __pLogicalDevice.get() },
			.pRenderPassFactory	{ __pRenderPassFactory.get() },
			.hinstance			{ hinstance },
			.hwnd				{ hwnd }
		};

		return new RenderTarget{ createInfo };
	}

	Layer *RenderingEngine::createLayer()
	{
		return new Layer{ __context };
	}

	Mesh *RenderingEngine::createMesh() noexcept
	{
		return new Mesh{ __context };
	}

	RenderObject *RenderingEngine::createRenderObject() noexcept
	{
		return new RenderObject{ };
	}

	void RenderingEngine::setMaxInFlightFrameCount(const size_t count)
	{
		if (count > Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT)
			throw std::runtime_error{ std::format("The count cannot be greater than {}.", Constants::MAX_IN_FLIGHT_FRAME_COUNT_LIMIT) };

		__maxInFlightFrameCount = count;
	}

	void RenderingEngine::render(RenderTarget &renderTarget)
	{
		if (!(renderTarget.isPresentable()))
			return;

		auto commandBuffer{ __pCBCirculator->getNext() };

		const VkCommandBufferBeginInfo cbBeginInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
		};

		commandBuffer.begin(cbBeginInfo);
		__commandExecutor.execute(commandBuffer);

		const auto drawResult{ renderTarget.draw(commandBuffer) };
		commandBuffer.end();

		auto &queue{ __pLogicalDevice->getQueue() };

		// semaphore wait는 모든 memory visible barrier가 암묵적 내장되어 있음
		const VkSemaphoreSubmitInfo waitSemaphoreInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO },
			.semaphore		{ drawResult.pImgAcquireSemaphore->getHandle() },
			.stageMask		{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT }
		};

		const VkCommandBufferSubmitInfo commandBufferInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO },
			.commandBuffer	{ commandBuffer.getHandle() }
		};

		auto &submitSemaphore{ __pSubmitSemaphoreCirculator->getNext() };

		// semaphore or fence signal은 걸려있는 모든 previous command의 실행 완료, 모든 memory available가 암묵적 내장되어 있음
		const VkSemaphoreSubmitInfo signalSemaphoreInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO },
			.semaphore		{ submitSemaphore.getHandle() },
			.stageMask		{ VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT }
		};

		const VkSubmitInfo2 submitInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO_2 },
			.waitSemaphoreInfoCount		{ 1U },
			.pWaitSemaphoreInfos		{ &waitSemaphoreInfo },
			.commandBufferInfoCount		{ 1U },
			.pCommandBufferInfos		{ &commandBufferInfo },
			.signalSemaphoreInfoCount	{ 1U },
			.pSignalSemaphoreInfos		{ &signalSemaphoreInfo }
		};

		auto &fence{ __getSubmitFence() };
		fence.reset();

		// vkQueueSubmit은 모든 host-visible memory의 full memory barrier (available - visible) 보장
		queue.submit(1U, &submitInfo, fence.getHandle());
		__lazyDeleter.advance();

		const VkPresentInfoKHR presentInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_PRESENT_INFO_KHR },
			.waitSemaphoreCount		{ 1U },
			.pWaitSemaphores		{ &(submitSemaphore.getHandle()) },
			.swapchainCount			{ 1U },
			.pSwapchains			{ &(renderTarget.getSwapchainHandle()) },
			.pImageIndices			{ &(drawResult.imageIndex) }
		};

		queue.present(presentInfo);
	}

	void RenderingEngine::__createRenderTargetDescSetLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;
		// TODO

		__pRenderTargetDescSetLayout = std::unique_ptr<Graphics::DescriptorSetLayout>
		{
			__pLogicalDevice->createDescriptorSetLayout(
				0U, static_cast<uint32_t>(bindings.size()), bindings.data())
		};
	}

	Graphics::Fence &RenderingEngine::__getSubmitFence() noexcept
	{
		for (auto it{ __inFlightFences.begin() }; it != __inFlightFences.end(); ++it)
		{
			const auto pFence{ *it };
			if (pFence->wait(0ULL) == VK_SUCCESS)
			{
				__inFlightFences.erase(it);
				return *pFence;
			}
		}

		if (__inFlightFences.size() < __maxInFlightFrameCount)
		{
			auto &nextFence{ __pSubmitFenceCirculator->getNext() };
			__inFlightFences.emplace(&nextFence);
			return nextFence;
		}

		Graphics::Fence *pRetVal{ };
		while (!pRetVal)
		{
			for (auto it{ __inFlightFences.begin() }; it != __inFlightFences.end(); ++it)
			{
				const auto pFence{ *it };

				// 1ms
				if (pFence->wait(1'000'000ULL) == VK_SUCCESS)
				{
					__inFlightFences.erase(it);
					pRetVal = pFence;
					break;
				}
			}
		}

		return *pRetVal;
	}
}
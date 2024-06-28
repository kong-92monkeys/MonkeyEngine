module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderingEngine;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.AssetManager;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Framebuffer;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Engine.RenderTarget;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.Layer;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.CommandBufferCirculator;
import <stdexcept>;
import <memory>;
import <concepts>;

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

		template <std::derived_from<Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		$Renderer *createRenderer($Args &&...args);

		void render(RenderTarget &renderTarget);

	private:
		const Graphics::PhysicalDevice &__physicalDevice;
		const Lib::AssetManager &__assetManager;

		std::unique_ptr<Graphics::LogicalDevice> __pLogicalDevice;
		std::unique_ptr<RenderPassFactory> __pRenderPassFactory;
		std::unique_ptr<CommandBufferCirculator> __pCBCirculator;
	};

	template <std::derived_from<Renderer> $Renderer, typename ...$Args>
	$Renderer *RenderingEngine::createRenderer($Args &&...args)
	{
		const Renderer::InitInfo initInfo
		{
			.pDevice			{ __pLogicalDevice.get() },
			.pAssetManager		{ &__assetManager },
			.pRenderPassFactory	{ __pRenderPassFactory.get() }
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
		__pLogicalDevice = std::unique_ptr<Graphics::LogicalDevice>{ __physicalDevice.createLogicalDevice() };
		__pRenderPassFactory = std::make_unique<RenderPassFactory>(*__pLogicalDevice);

		__pCBCirculator = std::make_unique<CommandBufferCirculator>(
			*__pLogicalDevice, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 100U);
	}

	RenderingEngine::~RenderingEngine() noexcept
	{
		__pCBCirculator = nullptr;
		__pRenderPassFactory = nullptr;
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
		return new Layer{ *__pRenderPassFactory };
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

		renderTarget.draw(commandBuffer);

		commandBuffer.end();
	}
}
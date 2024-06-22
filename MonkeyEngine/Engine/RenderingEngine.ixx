module;

#include "../Vulkan/Vulkan.h"
#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.RenderingEngine;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Engine.AssetManager;
import ntmonkeys.com.Engine.RenderTarget;
import ntmonkeys.com.Engine.Renderer;
import <optional>;
import <stdexcept>;
import <memory>;
import <format>;
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
			const AssetManager *pAssetManager{ };
		};

		explicit RenderingEngine(const CreateInfo &createInfo);
		virtual ~RenderingEngine() noexcept override;

		[[nodiscard]]
		std::unique_ptr<RenderTarget> createRenderTarget(const HINSTANCE hinstance, const HWND hwnd);

		[[nodiscard]]
		std::unique_ptr<Graphics::RenderPass> createRenderPass(
			const uint32_t attachmentCount, const VkAttachmentDescription2 *const pAttachments,
			const uint32_t subpassCount, const VkSubpassDescription2 *const pSubpasses,
			const uint32_t dependencyCount, const VkSubpassDependency2 *const pDependencies);

		template <std::derived_from<Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		std::unique_ptr<$Renderer> createRenderer($Args &&...args);

	private:
		const Graphics::PhysicalDevice &__physicalDevice;
		const AssetManager &__assetManager;

		std::unique_ptr<Graphics::LogicalDevice> __pLogicalDevice;
	};

	template <std::derived_from<Renderer> $Renderer, typename ...$Args>
	std::unique_ptr<$Renderer> RenderingEngine::createRenderer($Args &&...args)
	{
		const Renderer::DependencyInfo dependencyInfo
		{
			.pLogicalDevice	{ __pLogicalDevice.get() },
			.pAssetManager	{ &__assetManager }
		};

		auto retVal{ std::make_unique<$Renderer>(std::forward<$Args>(args)...) };
		retVal->injectDependencies(dependencyInfo);
		return retVal;
	}
}

module: private;

namespace Engine
{
	RenderingEngine::RenderingEngine(const CreateInfo &createInfo) :
		__physicalDevice	{ *(createInfo.pPhysicalDevice) },
		__assetManager		{ *(createInfo.pAssetManager) }
	{
		__pLogicalDevice = __physicalDevice.createLogicalDevice();
	}

	RenderingEngine::~RenderingEngine() noexcept
	{
		__pLogicalDevice = nullptr;
	}

	std::unique_ptr<RenderTarget> RenderingEngine::createRenderTarget(const HINSTANCE hinstance, const HWND hwnd)
	{
		const RenderTarget::CreateInfo createInfo
		{
			.pLogicalDevice	{ __pLogicalDevice.get() },
			.hinstance		{ hinstance },
			.hwnd			{ hwnd }
		};

		return std::make_unique<RenderTarget>(createInfo);
	}

	std::unique_ptr<Graphics::RenderPass> RenderingEngine::createRenderPass(
		const uint32_t attachmentCount, const VkAttachmentDescription2 *const pAttachments,
		const uint32_t subpassCount, const VkSubpassDescription2 *const pSubpasses,
		const uint32_t dependencyCount, const VkSubpassDependency2 *const pDependencies)
	{
		return __pLogicalDevice->createRenderPass(
			attachmentCount, pAttachments,
			subpassCount, pSubpasses,
			dependencyCount, pDependencies);
	}
}
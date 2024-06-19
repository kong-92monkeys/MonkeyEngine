module;

#include "../Vulkan/Vulkan.h"
#include <shaderc/shaderc.hpp>

export module ntmonkeys.com.Engine.RenderingEngine;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Engine.RenderTarget;
import ntmonkeys.com.Engine.Renderer;
import <optional>;
import <stdexcept>;
import <memory>;
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
		};

		explicit RenderingEngine(const CreateInfo &createInfo);
		virtual ~RenderingEngine() noexcept override;

		[[nodiscard]]
		std::unique_ptr<RenderTarget> createRenderTarget(const HINSTANCE hinstance, const HWND hwnd);

		[[nodiscard]]
		std::unique_ptr<Renderer> createRenderer(const Renderer::ShaderInfoMap &shaderInfoMap);

	private:
		const Graphics::PhysicalDevice &__physicalDevice;

		std::unique_ptr<Graphics::LogicalDevice> __pLogicalDevice;
	};
}

module: private;

namespace Engine
{
	RenderingEngine::RenderingEngine(const CreateInfo &createInfo) :
		__physicalDevice{ *(createInfo.pPhysicalDevice) }
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

	std::unique_ptr<Renderer> RenderingEngine::createRenderer(const Renderer::ShaderInfoMap &shaderInfoMap)
	{
		const Renderer::CreateInfo createInfo
		{
			.pLogicalDevice	{ __pLogicalDevice.get() },
			.pShaderInfoMap	{ &shaderInfoMap }
		};

		return std::make_unique<Renderer>(createInfo);
	}
}
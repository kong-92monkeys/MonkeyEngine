module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.FrameworkRenderer;

import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Frameworks.RenderPassFactory;

namespace Frameworks
{
	export class FrameworkRenderer : public Engine::Renderer
	{
	public:
		struct DependencyInfo
		{
		public:
			const RenderPassFactory *pRenderPassFactory{ };
		};

		FrameworkRenderer() = default;
		virtual ~FrameworkRenderer() noexcept override = default;

		constexpr void injectFrameworkDependencies(const DependencyInfo &info) noexcept;

	protected:
		[[nodiscard]]
		const Graphics::RenderPass &_getRenderPass(const RenderPassType type) const noexcept;

	private:
		const RenderPassFactory *__pRenderPassFactory{ };
	};

	constexpr void FrameworkRenderer::injectFrameworkDependencies(const DependencyInfo &info) noexcept
	{
		__pRenderPassFactory = info.pRenderPassFactory;
	}
}

module: private;

namespace Frameworks
{
	const Graphics::RenderPass &FrameworkRenderer::_getRenderPass(const RenderPassType type) const noexcept
	{
		return __pRenderPassFactory->getInstance(type);
	}
}
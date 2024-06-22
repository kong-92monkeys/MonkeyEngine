module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.FrameworkRenderer;

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
		constexpr const RenderPassFactory &_getRenderPassFactory() const noexcept;

	private:
		const RenderPassFactory *__pRenderPassFactory{ };
	};

	constexpr void FrameworkRenderer::injectFrameworkDependencies(const DependencyInfo &info) noexcept
	{
		__pRenderPassFactory = info.pRenderPassFactory;
	}

	constexpr const RenderPassFactory &FrameworkRenderer::_getRenderPassFactory() const noexcept
	{
		return *__pRenderPassFactory;
	}
}

module: private;

namespace Frameworks
{
	
}
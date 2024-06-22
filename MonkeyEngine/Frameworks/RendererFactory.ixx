module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.RendererFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Frameworks.FrameworkRenderer;
import ntmonkeys.com.Frameworks.RenderPassFactory;
import <concepts>;
import <memory>;

namespace Frameworks
{
	export class RendererFactory : public Lib::Unique
	{
	public:
		RendererFactory(Engine::RenderingEngine &engine, const RenderPassFactory &renderPassFactory) noexcept;
		virtual ~RendererFactory() noexcept override = default;

		template <std::derived_from<FrameworkRenderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		std::unique_ptr<$Renderer> createRenderer($Args &&...args);

	private:
		Engine::RenderingEngine &__engine;
		const RenderPassFactory &__renderPassFactory;
	};

	template <std::derived_from<FrameworkRenderer> $Renderer, typename ...$Args>
	std::unique_ptr<$Renderer> RendererFactory::createRenderer($Args &&...args)
	{
		const FrameworkRenderer::DependencyInfo dependencyInfo
		{
			.pRenderPassFactory{ &__renderPassFactory }
		};

		auto retVal{ __engine.createRenderer<$Renderer>(std::forward<$Args>(args)...) };
		retVal->injectFrameworkDependencies(dependencyInfo);
		retVal->init();

		return retVal;
	}
}

module: private;

namespace Frameworks
{
	RendererFactory::RendererFactory(Engine::RenderingEngine &engine, const RenderPassFactory &renderPassFactory) noexcept :
		__engine{ engine }, __renderPassFactory{ renderPassFactory }
	{}
}
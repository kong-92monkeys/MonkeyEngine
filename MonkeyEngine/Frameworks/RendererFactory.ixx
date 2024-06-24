module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.RendererFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Frameworks.Renderer;
import ntmonkeys.com.Frameworks.AssetManager;
import ntmonkeys.com.Frameworks.RenderPassFactory;
import <concepts>;
import <memory>;

namespace Frameworks
{
	export class RendererFactory : public Lib::Unique
	{
	public:
		RendererFactory(
			Engine::RenderingEngine &engine,
			const AssetManager &assetManager,
			const RenderPassFactory &renderPassFactory) noexcept;

		virtual ~RendererFactory() noexcept override = default;

		template <std::derived_from<Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		$Renderer *createRenderer($Args &&...args);

	private:
		Engine::RenderingEngine &__engine;
		const AssetManager &__assetManager;
		const RenderPassFactory &__renderPassFactory;
	};

	template <std::derived_from<Renderer> $Renderer, typename ...$Args>
	$Renderer *RendererFactory::createRenderer($Args &&...args)
	{
		const Renderer::InitInfo initInfo
		{
			.pEngine			{ &__engine },
			.pAssetManager		{ &__assetManager },
			.pRenderPassFactory	{ &__renderPassFactory }
		};

		const auto pRetVal{ new $Renderer{ std::forward<$Args>(args)... } };
		pRetVal->init(initInfo);
		return pRetVal;
	}
}

module: private;

namespace Frameworks
{
	RendererFactory::RendererFactory(
		Engine::RenderingEngine &engine,
		const AssetManager &assetManager,
		const RenderPassFactory &renderPassFactory) noexcept :
		__engine			{ engine },
		__assetManager		{ assetManager },
		__renderPassFactory	{ renderPassFactory }
	{}
}
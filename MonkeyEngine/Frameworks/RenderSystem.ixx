module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.RenderSystem;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Frameworks.AssetManager;
import ntmonkeys.com.Frameworks.Renderer;
import ntmonkeys.com.Frameworks.RenderPassFactory;
import ntmonkeys.com.Frameworks.RendererFactory;
import ntmonkeys.com.Frameworks.Window;
import <memory>;

namespace Frameworks
{
	export class RenderSystem : public Lib::Unique
	{
	public:
		RenderSystem(Engine::RenderingEngine &engine, const AssetManager &assetManager);
		virtual ~RenderSystem() noexcept override;

		template <std::derived_from<Renderer> $Renderer, typename ...$Args>
		[[nodiscard]]
		$Renderer *createRenderer($Args &&...args);

		[[nodiscard]]
		Window *createWindow(const HINSTANCE hinstance, const HWND hwnd);

	private:
		Engine::RenderingEngine &__engine;
		std::unique_ptr<RenderPassFactory> __pRenderPassFactory;
		std::unique_ptr<RendererFactory> __pRendererFactory;
	};

	template <std::derived_from<Renderer> $Renderer, typename ...$Args>
	$Renderer *RenderSystem::createRenderer($Args &&...args)
	{
		return __pRendererFactory->createRenderer<$Renderer>(std::forward<$Args>(args)...);
	}
}

module: private;

namespace Frameworks
{
	RenderSystem::RenderSystem(Engine::RenderingEngine &engine, const AssetManager &assetManager) :
		__engine{ engine }
	{
		__pRenderPassFactory = std::make_unique<RenderPassFactory>(engine);
		__pRendererFactory = std::make_unique<RendererFactory>(engine, assetManager, *__pRenderPassFactory);
	}

	RenderSystem::~RenderSystem() noexcept
	{
		__pRendererFactory = nullptr;
		__pRenderPassFactory = nullptr;
	}

	Window *RenderSystem::createWindow(const HINSTANCE hinstance, const HWND hwnd)
	{
		return new Window{ __engine, hinstance, hwnd };
	}
}
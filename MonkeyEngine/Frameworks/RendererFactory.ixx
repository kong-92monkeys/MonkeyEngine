export module ntmonkeys.com.Frameworks.RendererFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Frameworks.SimpleRenderer;
import <unordered_map>;
import <memory>;

namespace Frameworks
{
	export enum class RendererType
	{
		SIMPLE
	};

	export class RendererFactory : public Lib::Unique
	{
	public:
		RendererFactory(Engine::RenderingEngine &engine) noexcept;

		[[nodiscard]]
		const Engine::Renderer &getInstance(const RendererType type) noexcept;

	private:
		std::unordered_map<RendererType, std::unique_ptr<Engine::Renderer>> __instanceMap;
	};
}

module: private;

namespace Frameworks
{
	RendererFactory::RendererFactory(Engine::RenderingEngine &engine) noexcept
	{
		__instanceMap[RendererType::SIMPLE] = engine.createRenderer<SimpleRenderer>():
	}

	const Engine::Renderer &RendererFactory::getInstance(const RendererType type) noexcept
	{
		return *(__instanceMap.at(type));
	}
}
export module ntmonkeys.com.Frameworks.RenderSystem:Impl;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Engine.Core;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Frameworks.RenderSystemContext;
import ntmonkeys.com.Frameworks.SceneObject;
import ntmonkeys.com.Frameworks.RendererFactory;
import <string>;
import <memory>;

namespace Frameworks
{
	class RenderSystemImpl : public Lib::Unique
	{
	public:
		RenderSystemImpl(const std::string &appName, const Lib::Version &appVersion);
		virtual ~RenderSystemImpl() noexcept;

		void createSceneObject(void *const pPlaceholder);

	private:
		RenderSystemContext __context;

		std::unique_ptr<Engine::Core> __pCore;
		std::unique_ptr<Engine::RenderingEngine> __pEngine;

		std::unique_ptr<RendererFactory> __pRendererFactory;

		void __createCore(const std::string &appName, const Lib::Version &appVersion);
		void __createEngine();
	};

	RenderSystemImpl::RenderSystemImpl(const std::string &appName, const Lib::Version &appVersion)
	{
		__createCore(appName, appVersion);
		__createEngine();
		__pRendererFactory = std::make_unique<RendererFactory>(*__pEngine);

		__context.pEngine = __pEngine.get();
		__context.pRendererFactory = __pRendererFactory.get();
	}

	RenderSystemImpl::~RenderSystemImpl() noexcept
	{
		__pEngine = nullptr;
		__pCore = nullptr;
	}

	void RenderSystemImpl::createSceneObject(void *const pPlaceholder)
	{
		new (pPlaceholder) SceneObjectImpl{ __context };
	}

	void RenderSystemImpl::__createCore(const std::string &appName, const Lib::Version &appVersion)
	{
		const Engine::Core::CreateInfo coreCreateInfo
		{
			.vulkanLoaderLibName	{ "vulkan_loader_dedicated-1.dll" },
			.appName				{ appName },
			.appVersion				{ appVersion },
			.engineName				{ "MonkeyEngine" },
			.engineVersion			{ 1U, 0U, 0U, 0U }
		};

		__pCore = std::make_unique<Engine::Core>(coreCreateInfo);
	}

	void RenderSystemImpl::__createEngine()
	{
		__pEngine = std::unique_ptr<Engine::RenderingEngine>{ __pCore->createEngine() };
	}
}
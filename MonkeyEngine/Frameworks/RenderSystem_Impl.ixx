export module ntmonkeys.com.Frameworks.RenderSystem:Impl;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Engine.Core;
import ntmonkeys.com.Engine.RenderingEngine;
import <string>;
import <memory>;

namespace Frameworks
{
	class RenderSystemImpl : public Lib::Unique
	{
	public:
		RenderSystemImpl(const std::string &appName, const Lib::Version &appVersion);
		virtual ~RenderSystemImpl() noexcept;

	private:
		std::unique_ptr<Engine::Core> __pCore;
		std::unique_ptr<Engine::RenderingEngine> __pEngine;

		void __createCore(const std::string &appName, const Lib::Version &appVersion);
		void __createEngine();
	};

	RenderSystemImpl::RenderSystemImpl(const std::string &appName, const Lib::Version &appVersion)
	{
		__createCore(appName, appVersion);
		__createEngine();
	}

	RenderSystemImpl::~RenderSystemImpl() noexcept
	{
		__pEngine = nullptr;
		__pCore = nullptr;
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
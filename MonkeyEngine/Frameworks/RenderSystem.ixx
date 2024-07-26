export module ntmonkeys.com.Frameworks.RenderSystem;

import :Impl;
import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.ThreadPool;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Frameworks.SceneObject;
import <string>;
import <functional>;

namespace Frameworks
{
	export class RenderSystem : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			std::string appName;
			Lib::Version appVersion;
		};

		RenderSystem(const CreateInfo &createInfo);
		virtual ~RenderSystem() noexcept override;

		[[nodiscard]]
		SceneObject *createSceneObject();

	private:
		Lib::ThreadPool __rcmdExecutor{ 1ULL };
		RenderSystemImpl *__pImpl{ };
	};
}

module: private;

namespace Frameworks
{
	RenderSystem::RenderSystem(const CreateInfo &createInfo)
	{
		__rcmdExecutor.silentRun([this, createInfo]
		{
			__pImpl = new RenderSystemImpl{ createInfo.appName, createInfo.appVersion };
		});
	}

	RenderSystem::~RenderSystem() noexcept
	{
		__rcmdExecutor.run([this]
		{
			delete __pImpl;
		}).wait();
	}

	SceneObject *RenderSystem::createSceneObject()
	{
		SceneObject::InitInfo initInfo;

		initInfo.pRcmdExecutor = &__rcmdExecutor;
		initInfo.implInstantiator = [this] (void *const pPlaceholder)
		{
			__pImpl->createSceneObject(pPlaceholder);
		};

		SceneObject *const pRetVal{ new SceneObject };
		pRetVal->init(initInfo);
		return pRetVal;
	}
}
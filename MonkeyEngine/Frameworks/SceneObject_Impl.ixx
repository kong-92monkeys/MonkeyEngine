export module ntmonkeys.com.Frameworks.SceneObject:Impl;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.RenderObject;
import ntmonkeys.com.Frameworks.RenderSystemContext;
import <memory>;

namespace Frameworks
{
	export class SceneObjectImpl : public Lib::Unique
	{
	public:
		SceneObjectImpl(const RenderSystemContext &context);

	private:
		std::shared_ptr<Engine::RenderObject> __pRenderObject;
	};

	SceneObjectImpl::SceneObjectImpl(const RenderSystemContext &context)
	{
		__pRenderObject = std::shared_ptr<Engine::RenderObject>{ context.pEngine->createRenderObject() };
	}
}
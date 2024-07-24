export module ntmonkeys.com.Frameworks.SceneObject;

export import :Impl;
import ntmonkeys.com.Frameworks.RenderInterface;

namespace Frameworks
{
	export class SceneObject : public RenderInterface<SceneObjectImpl>
	{
	public:

	protected:
		virtual void _onInit() override;

	private:

	};
}

module: private;

namespace Frameworks
{
	void SceneObject::_onInit()
	{
		_initImpl([] (void *const pImplPlaceholder)
		{
			SceneObjectImpl *const pImpl{ new (pImplPlaceholder) SceneObjectImpl };
		});
	}
}
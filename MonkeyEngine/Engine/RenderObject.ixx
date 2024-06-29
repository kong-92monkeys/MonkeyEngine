export module ntmonkeys.com.Engine.RenderObject;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.Renderer;
import <memory>;

namespace Engine
{
	export class RenderObject : public Lib::Unique
	{
	public:
		[[nodiscard]]
		constexpr const std::shared_ptr<Renderer> &getRenderer() const noexcept;
		void setRenderer(const std::shared_ptr<Renderer> &pRenderer) noexcept;

	private:
		std::shared_ptr<Renderer> __pRenderer;
	};

	constexpr const std::shared_ptr<Renderer> &RenderObject::getRenderer() const noexcept
	{
		return __pRenderer;
	}
}

module: private;

namespace Engine
{
	void RenderObject::setRenderer(const std::shared_ptr<Renderer> &pRenderer) noexcept
	{
		if (__pRenderer == pRenderer)
			return;

		__pRenderer = pRenderer;
	}
}
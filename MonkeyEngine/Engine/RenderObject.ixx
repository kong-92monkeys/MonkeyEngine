export module ntmonkeys.com.Engine.RenderObject;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.Mesh;
import ntmonkeys.com.Engine.DrawParam;
import <memory>;

namespace Engine
{
	export class RenderObject : public Lib::Unique
	{
	public:
		[[nodiscard]]
		const Renderer *getRenderer() const noexcept;
		void setRenderer(const std::shared_ptr<const Renderer> &pRenderer) noexcept;

		[[nodiscard]]
		const Mesh *getMesh() const noexcept;
		void setMesh(const std::shared_ptr<const Mesh> &pMesh) noexcept;

		void setDrawParam(const std::shared_ptr<const DrawParam> &pDrawParam) noexcept;

		void draw(Graphics::CommandBuffer &commandBuffer) const noexcept;

	private:
		std::shared_ptr<const Renderer> __pRenderer;
		std::shared_ptr<const Mesh> __pMesh;
		std::shared_ptr<const DrawParam> __pDrawParam;
	};
}

module: private;

namespace Engine
{
	const Renderer *RenderObject::getRenderer() const noexcept
	{
		return __pRenderer.get();
	}

	void RenderObject::setRenderer(const std::shared_ptr<const Renderer> &pRenderer) noexcept
	{
		if (__pRenderer == pRenderer)
			return;

		__pRenderer = pRenderer;
	}

	const Mesh *RenderObject::getMesh() const noexcept
	{
		return __pMesh.get();
	}

	void RenderObject::setMesh(const std::shared_ptr<const Mesh> &pMesh) noexcept
	{
		if (__pMesh == pMesh)
			return;

		__pMesh = pMesh;
	}

	void RenderObject::setDrawParam(const std::shared_ptr<const DrawParam> &pDrawParam) noexcept
	{
		if (__pDrawParam == pDrawParam)
			return;

		__pDrawParam = pDrawParam;
	}

	void RenderObject::draw(Graphics::CommandBuffer &commandBuffer) const noexcept
	{
		__pDrawParam->draw(commandBuffer, 1U, 0U);
	}
}
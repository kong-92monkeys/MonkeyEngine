module;

#include "../Library/GLM.h"
#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Layer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.WeakReferenceSet;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.Semaphore;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.FramebufferFactory;
import ntmonkeys.com.Engine.RenderObject;
import ntmonkeys.com.Engine.Renderer;
import <memory>;

namespace Engine
{
	export class Layer : public Lib::Unique
	{
	public:
		Layer(const RenderPassFactory &renderPassFactory) noexcept;
		virtual ~Layer() noexcept override;

		void addRenderObject(const std::shared_ptr<RenderObject> &pObject) noexcept;
		void removeRenderObject(const std::shared_ptr<RenderObject> &pObject) noexcept;

		void draw(Graphics::CommandBuffer &commandBuffer, const Renderer::BeginInfo &rendererBeginInfo);

	private:
		const RenderPassFactory &__renderPassFactory;

		Lib::WeakReferenceSet<RenderObject> __renderObjects;
	};
}

module: private;

namespace Engine
{
	Layer::Layer(const RenderPassFactory &renderPassFactory) noexcept :
		__renderPassFactory{ renderPassFactory }
	{}

	Layer::~Layer() noexcept
	{
	}

	void Layer::addRenderObject(const std::shared_ptr<RenderObject> &pObject) noexcept
	{
		__renderObjects.emplace(pObject);
	}

	void Layer::removeRenderObject(const std::shared_ptr<RenderObject> &pObject) noexcept
	{
		__renderObjects.erase(pObject);
	}

	void Layer::draw(Graphics::CommandBuffer &commandBuffer, const Renderer::BeginInfo &rendererBeginInfo)
	{
		for (const auto &renderObject : __renderObjects)
		{
			const auto pRenderer{ renderObject.getRenderer() };

			pRenderer->begin(commandBuffer, rendererBeginInfo);

			renderObject.getMesh()->bind(commandBuffer);
			renderObject.draw(commandBuffer);

			pRenderer->end(commandBuffer);
		}
	}
}
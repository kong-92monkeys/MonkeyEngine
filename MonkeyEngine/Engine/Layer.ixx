 module;

#include "../Library/GLM.h"
#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Layer;

export import :LayerDrawInfo;
import :SubLayer;
import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Stateful;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.RenderObject;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.CommandBufferCirculator;
import <unordered_set>;
import <unordered_map>;
import <memory>;
import <vector>;

namespace Engine
{
	export class Layer : public Lib::Unique, public Lib::Stateful<Layer>
	{
	public:
		Layer(const EngineContext &context) noexcept;
		virtual ~Layer() noexcept override = default;

		[[nodiscard]]
		constexpr int getPriority() const noexcept;
		void setPriority(const int priority) noexcept;

		void addRenderObject(const std::shared_ptr<const RenderObject> &pObject);
		void removeRenderObject(const std::shared_ptr<const RenderObject> &pObject);

		[[nodiscard]]
		bool isEmpty() const noexcept;

		void draw(Graphics::CommandBuffer &commandBuffer, const LayerDrawInfo &drawInfo) const;

		[[nodiscard]]
		constexpr Lib::Event<const Layer *, int, int> &getPriorityChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::Event<const Layer *> &getNeedRedrawEvent() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		const EngineContext &__context;

		std::unordered_set<std::shared_ptr<const RenderObject>> __renderObjects;
		std::unordered_map<const Renderer *, std::unique_ptr<SubLayer>> __subLayerMap;
		std::unordered_map<const RenderObject *, SubLayer *> __object2SubLayerMap;

		int __priority{ };

		std::unordered_set<SubLayer *> __invalidatedSubLayers;

		Lib::EventListenerPtr<const RenderObject *, const Renderer *, const Renderer *> __pObjectRendererChangeListener;
		Lib::EventListenerPtr<SubLayer *> __pSubLayerInvalidatedListener;
		Lib::EventListenerPtr<const SubLayer *> __pSubLayerNeedRedrawListener;

		mutable Lib::Event<const Layer *, int, int> __priorityChangeEvent;
		mutable Lib::Event<const Layer *> __needRedrawEvent;

		void __registerObject(const RenderObject *const pObject) noexcept;
		void __unregisterObject(const RenderObject *const pObject) noexcept;

		[[nodiscard]]
		SubLayer *__getSubLayerOf(const Renderer *const pRenderer) noexcept;

		void __onObjectRendererChanged(const RenderObject *const pObject, const Renderer *const pPrev, const Renderer *const pCur) noexcept;
		void __onSubLayerInvalidated(SubLayer *const pSubLayer) noexcept;
		void __onSubLayerRedrawNeeded() const noexcept;
	};

	constexpr int Layer::getPriority() const noexcept
	{
		return __priority;
	}

	constexpr Lib::Event<const Layer *, int, int> &Layer::getPriorityChangeEvent() const noexcept
	{
		return __priorityChangeEvent;
	}

	constexpr Lib::Event<const Layer *> &Layer::getNeedRedrawEvent() const noexcept
	{
		return __needRedrawEvent;
	}
}

module: private;

namespace Engine
{
	Layer::Layer(const EngineContext &context) noexcept :
		__context{ context }
	{
		__pObjectRendererChangeListener =
			Lib::EventListener<const RenderObject *, const Renderer *, const Renderer *>::bind(
				&Layer::__onObjectRendererChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pSubLayerInvalidatedListener =
			Lib::EventListener<SubLayer *>::bind(&Layer::__onSubLayerInvalidated, this, std::placeholders::_1);

		__pSubLayerNeedRedrawListener = Lib::EventListener<const SubLayer *>::bind(&Layer::__onSubLayerRedrawNeeded, this);
	}

	void Layer::setPriority(const int priority) noexcept
	{
		if (__priority == priority)
			return;

		const int prevPriority{ __priority };
		__priority = priority;

		__priorityChangeEvent.invoke(this, prevPriority, priority);
		__needRedrawEvent.invoke(this);
	}

	void Layer::addRenderObject(const std::shared_ptr<const RenderObject> &pObject)
	{
		const bool inserted{ __renderObjects.emplace(pObject).second };
		if (!inserted)
			throw std::runtime_error{ "The object is already added." };

		pObject->getRendererChangeEvent() += __pObjectRendererChangeListener;

		if (pObject->getRenderer())
			__registerObject(pObject.get());
	}

	void Layer::removeRenderObject(const std::shared_ptr<const RenderObject> &pObject)
	{
		const bool erased{ static_cast<bool>(__renderObjects.erase(pObject)) };
		if (!erased)
			throw std::runtime_error{ "The object is not added yet." };

		pObject->getRendererChangeEvent() -= __pObjectRendererChangeListener;

		if (pObject->getRenderer())
			__unregisterObject(pObject.get());
	}

	bool Layer::isEmpty() const noexcept
	{
		return __renderObjects.empty();
	}

	void Layer::draw(Graphics::CommandBuffer &commandBuffer, const LayerDrawInfo &drawInfo) const
	{
		for (const auto &[_, pSubLayer] : __subLayerMap)
		{
			if (pSubLayer->isEmpty())
				continue;

			pSubLayer->draw(commandBuffer, drawInfo);
		}
	}

	void Layer::_onValidate()
	{
		for (const auto pSubLayer : __invalidatedSubLayers)
			pSubLayer->validate();

		__invalidatedSubLayers.clear();
	}

	void Layer::__registerObject(const RenderObject *const pObject) noexcept
	{
		const auto pSubLayer{ __getSubLayerOf(pObject->getRenderer()) };
		pSubLayer->addRenderObject(pObject);

		__object2SubLayerMap[pObject] = pSubLayer;
	}

	void Layer::__unregisterObject(const RenderObject *const pObject) noexcept
	{
		const auto pSubLayer{ __object2SubLayerMap.extract(pObject).mapped() };
		pSubLayer->removeRenderObject(pObject);

		if (pSubLayer->isEmpty())
		{
			__invalidatedSubLayers.erase(pSubLayer);
			__subLayerMap.erase(pSubLayer->getRenderer());
		}
	}

	SubLayer *Layer::__getSubLayerOf(const Renderer *const pRenderer) noexcept
	{
		auto &pRetVal{ __subLayerMap[pRenderer] };
		if (!pRetVal)
		{
			pRetVal = std::make_unique<SubLayer>(__context, pRenderer);
			pRetVal->getInvalidateEvent() += __pSubLayerInvalidatedListener;
			pRetVal->getNeedRedrawEvent() += __pSubLayerNeedRedrawListener;
		}

		return pRetVal.get();
	}

	void Layer::__onObjectRendererChanged(
		const RenderObject *const pObject, const Renderer *const pPrev, const Renderer *const pCur) noexcept
	{
		if (pPrev)
			__unregisterObject(pObject);

		if (pCur)
			__registerObject(pObject);
	}

	void Layer::__onSubLayerInvalidated(SubLayer *const pSubLayer) noexcept
	{
		__invalidatedSubLayers.emplace(pSubLayer);
		_invalidate();
	}

	void Layer::__onSubLayerRedrawNeeded() const noexcept
	{
		__needRedrawEvent.invoke(this);
	}
}
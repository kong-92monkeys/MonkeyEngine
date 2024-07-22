module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Layer:SubLayer;

import :MaterialBufferBuilder;
import :TextureReferenceManager;
import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Stateful;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Lib.RegionAllocator;
import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Lib.LazyRecycler;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Graphics.DescriptorSet;
import ntmonkeys.com.Engine.Constants;
import ntmonkeys.com.Engine.ShaderDataStructures;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.Mesh;
import ntmonkeys.com.Engine.DrawParam;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.Texture;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.RenderObject;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Engine.LayerResourcePool;
import ntmonkeys.com.Engine.DescriptorUpdater;
import <cstdint>;
import <unordered_map>;
import <unordered_set>;
import <memory>;
import <typeindex>;

namespace Engine
{
	export class SubLayer : public Lib::Unique, public Lib::Stateful<SubLayer>
	{
	public:
		SubLayer(const EngineContext &context, const Renderer *const pRenderer) noexcept;
		virtual ~SubLayer() noexcept override;

		[[nodiscard]]
		constexpr const Renderer *getRenderer() const noexcept;

		void addRenderObject(const RenderObject *const pObject) noexcept;
		void removeRenderObject(const RenderObject *const pObject) noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

		void draw(Graphics::CommandBuffer &commandBuffer) const;

		[[nodiscard]]
		constexpr Lib::Event<const SubLayer *> &getNeedRedrawEvent() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		const EngineContext &__context;
		const Renderer *const __pRenderer;

		Lib::RegionAllocator __objectRegionAllocator{ UINT32_MAX };
		TextureReferenceManager __textureReferenceManager;

		std::unordered_map<const RenderObject *, std::unique_ptr<Lib::Region>> __object2Region;
		std::unordered_map<const Mesh *, std::unordered_set<const RenderObject *>> __mesh2Objects;

		bool __instanceInfoBufferInvalidated{ };
		std::vector<InstanceInfo> __instanceInfoHostBuffer;
		std::shared_ptr<BufferChunk> __pInstanceInfoBuffer;

		std::unordered_map<std::type_index, std::unique_ptr<MaterialBufferBuilder>> __materialDataBufferBuilders;
		std::unordered_set<MaterialBufferBuilder *> __invalidatedMaterialBufferBuilders;

		DescriptorUpdater __descUpdater;
		std::unique_ptr<Graphics::DescriptorSet> __pSubLayerDescSet;

		Lib::EventListenerPtr<const RenderObject *, const Mesh *, const Mesh *> __pObjectMeshChangeListener;
		Lib::EventListenerPtr<const RenderObject *, uint32_t, std::type_index, const Material *, const Material *> __pObjectMaterialChangeListener;
		Lib::EventListenerPtr<const RenderObject *, uint32_t, uint32_t> __pObjectInstanceCountChangeListener;
		Lib::EventListenerPtr<const RenderObject *, bool> __pObjectDrawableChangeListener;

		Lib::EventListenerPtr<MaterialBufferBuilder *> __pMaterialBufferBuilderInvalidateListener;

		mutable Lib::Event<const SubLayer *> __needRedrawEvent;

		void __registerObject(const RenderObject *const pObject);
		void __unregisterObject(const RenderObject *const pObject);

		void __registerMesh(const RenderObject *const pObject, const Mesh *const pMesh);
		void __unregisterMesh(const RenderObject *const pObject, const Mesh *const pMesh);

		void __registerMaterial(const Material *const pMaterial);
		void __unregisterMaterial(const Material *const pMaterial);

		void __validateInstanceInfoHostBuffer(const RenderObject *const pObject);
		void __validateInstanceInfoHostBuffer(
			const RenderObject *const pObject, const uint32_t instanceIndex,
			const std::type_index &materialType, const Material *const pMaterial);

		void __validateInstanceInfoBuffer();
		void __validateMaterialBufferBuilders();
		void __validateDescriptorSet();

		void __onObjectMeshChanged(
			const RenderObject *const pObject,
			const Mesh *const pPrev, const Mesh *const pCur) noexcept;

		void __onObjectMaterialChanged(
			const RenderObject *const pObject, const uint32_t instanceIndex,
			const std::type_index &type, const Material *const pPrev, const Material *const pCur) noexcept;
		
		void __onObjectInstanceCountChanged(
			const RenderObject *const pObject, const uint32_t prev, const uint32_t cur) noexcept;

		void __onObjectDrawableChanged(const RenderObject *const pObject, const bool cur) noexcept;

		void __onMaterialBufferBuilderInvalidated(MaterialBufferBuilder *const pBuilder) noexcept;
	};

	SubLayer::SubLayer(const EngineContext &context, const Renderer *const pRenderer) noexcept :
		__context{ context }, __pRenderer{ pRenderer }, __descUpdater{ *(context.pLogicalDevice) }
	{
		__pObjectMeshChangeListener =
			Lib::EventListener<const RenderObject *, const Mesh *, const Mesh *>::bind(
				&SubLayer::__onObjectMeshChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectMaterialChangeListener =
			Lib::EventListener<const RenderObject *, uint32_t, std::type_index, const Material *, const Material *>::bind(
				&SubLayer::__onObjectMaterialChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5);

		__pObjectInstanceCountChangeListener =
			Lib::EventListener<const RenderObject *, uint32_t, uint32_t>::bind(
				&SubLayer::__onObjectInstanceCountChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectDrawableChangeListener = 
			Lib::EventListener<const RenderObject *, bool>::bind(
				&SubLayer::__onObjectDrawableChanged, this, std::placeholders::_1, std::placeholders::_2);

		__pMaterialBufferBuilderInvalidateListener =
			Lib::EventListener<MaterialBufferBuilder *>::bind(&SubLayer::__onMaterialBufferBuilderInvalidated, this, std::placeholders::_1);
	}

	SubLayer::~SubLayer() noexcept
	{
		const auto pLayerResourcePool	{ __context.pLayerResourcePool };

		if (__pInstanceInfoBuffer)
			pLayerResourcePool->recycleStorageBuffer(std::move(__pInstanceInfoBuffer));
	}

	constexpr const Renderer *SubLayer::getRenderer() const noexcept
	{
		return __pRenderer;
	}

	constexpr Lib::Event<const SubLayer *> &SubLayer::getNeedRedrawEvent() const noexcept
	{
		return __needRedrawEvent;
	}

	void SubLayer::addRenderObject(const RenderObject *const pObject) noexcept
	{
		pObject->getDrawableChangeEvent() += __pObjectDrawableChangeListener;
		if (pObject->isDrawable())
			__registerObject(pObject);
	}

	void SubLayer::removeRenderObject(const RenderObject *const pObject) noexcept
	{
		pObject->getDrawableChangeEvent() -= __pObjectDrawableChangeListener;
		if (pObject->isDrawable())
			__unregisterObject(pObject);
	}

	bool SubLayer::isEmpty() const noexcept
	{
		return __object2Region.empty();
	}

	void SubLayer::draw(Graphics::CommandBuffer &commandBuffer) const
	{
		if (isEmpty())
			return;

		if (__pSubLayerDescSet)
		{
			commandBuffer.bindDescriptorSets(
				VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
				__pRenderer->getPipelineLayout().getHandle(),
				Constants::SUB_LAYER_DESC_SET_LOCATION, 1U, &(__pSubLayerDescSet->getHandle()), 0U, nullptr);
		}

		const Mesh *pBoundMesh{ };
		for (const auto &[pMesh, objects] : __mesh2Objects)
		{
			if (pBoundMesh != pMesh)
			{
				pBoundMesh = pMesh;
				pMesh->bind(commandBuffer);
			}

			for (const auto pObject : objects)
			{
				const uint32_t baseId{ static_cast<uint32_t>(__object2Region.at(pObject)->getOffset()) };
				pObject->draw(commandBuffer, baseId);
			}
		}
	}

	void SubLayer::_onValidate()
	{
		if (isEmpty())
			return;

		__validateInstanceInfoBuffer();
		__validateMaterialBufferBuilders();
		__validateDescriptorSet();
	}

	void SubLayer::__registerObject(const RenderObject *const pObject)
	{
		pObject->getMeshChangeEvent() += __pObjectMeshChangeListener;
		pObject->getMaterialChangeEvent() += __pObjectMaterialChangeListener;
		pObject->getInstanceCountChangeEvent() += __pObjectInstanceCountChangeListener;

		const uint32_t instanceCount{ pObject->getInstanceCount() };
		__object2Region[pObject] = std::make_unique<Lib::Region>(__objectRegionAllocator, 1U, instanceCount);

		__registerMesh(pObject, pObject->getMesh());

		for (uint32_t instanceIter{ }; instanceIter < instanceCount; ++instanceIter)
		{
			for (const auto pMaterial : pObject->getMaterialPack(instanceIter))
				__registerMaterial(pMaterial);
		}

		__validateInstanceInfoHostBuffer(pObject);
		__instanceInfoBufferInvalidated = true;

		_invalidate();
		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__unregisterObject(const RenderObject *const pObject)
	{
		pObject->getMeshChangeEvent() -= __pObjectMeshChangeListener;
		pObject->getMaterialChangeEvent() -= __pObjectMaterialChangeListener;
		pObject->getInstanceCountChangeEvent() -= __pObjectInstanceCountChangeListener;

		__object2Region.erase(pObject);

		const auto pMesh{ pObject->getMesh() };
		if (pMesh)
			__unregisterMesh(pObject, pMesh);

		for (uint32_t instanceIter{ }; instanceIter < pObject->getInstanceCount(); ++instanceIter)
		{
			for (const auto pMaterial : pObject->getMaterialPack(instanceIter))
				__unregisterMaterial(pMaterial);
		}

		_invalidate();
		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__registerMesh(const RenderObject *const pObject, const Mesh *const pMesh)
	{
		__mesh2Objects[pMesh].emplace(pObject);
	}

	void SubLayer::__unregisterMesh(const RenderObject *const pObject, const Mesh *const pMesh)
	{
		auto &objects{ __mesh2Objects.at(pMesh) };
		objects.erase(pObject);

		if (objects.empty())
			__mesh2Objects.erase(pMesh);
	}

	void SubLayer::__registerMaterial(const Material *const pMaterial)
	{
		auto &pBufferBuilder{ __materialDataBufferBuilders[typeid(*pMaterial)] };
		if (!pBufferBuilder)
		{
			pBufferBuilder = std::make_unique<MaterialBufferBuilder>(*(__context.pLayerResourcePool), __textureReferenceManager);
			pBufferBuilder->getInvalidateEvent() += __pMaterialBufferBuilderInvalidateListener;
		}

		pBufferBuilder->registerMaterial(pMaterial);
	}

	void SubLayer::__unregisterMaterial(const Material *const pMaterial)
	{
		const auto &pBufferBuilder{ __materialDataBufferBuilders[typeid(*pMaterial)] };
		pBufferBuilder->unregisterMaterial(pMaterial);
	}

	void SubLayer::__validateInstanceInfoHostBuffer(const RenderObject *const pObject)
	{
		const auto &pRegion{ __object2Region.at(pObject) };

		const uint32_t instanceCount	{ pObject->getInstanceCount() };
		const size_t baseId				{ pRegion->getOffset() };
		const size_t maxId				{ baseId + instanceCount };

		if (maxId >= __instanceInfoHostBuffer.size())
			__instanceInfoHostBuffer.resize(maxId);

		for (uint32_t instanceIter{ }; instanceIter < instanceCount; ++instanceIter)
		{
			auto &instanceInfo{ __instanceInfoHostBuffer[baseId + instanceIter] };
			instanceInfo.reset();

			for (const auto pMaterial : pObject->getMaterialPack(instanceIter))
			{
				const std::type_index materialType{ typeid(*pMaterial) };

				const auto descLocation{ __pRenderer->getMaterialDescLocationOf(materialType) };
				if (descLocation.has_value())
				{
					const uint32_t slotIndex	{ descLocation.value() - Constants::SUB_LAYER_MATERIAL_DESC_LOCATION0 };
					const auto &pBufferBuilder	{ __materialDataBufferBuilders.at(materialType) };

					instanceInfo.materialIds[slotIndex] = pBufferBuilder->getIdOf(pMaterial);
				}
			}
		}
	}

	void SubLayer::__validateInstanceInfoHostBuffer(
		const RenderObject *const pObject, const uint32_t instanceIndex,
		const std::type_index &materialType, const Material *const pMaterial)
	{
		const auto &pRegion{ __object2Region.at(pObject) };

		const size_t baseId			{ pRegion->getOffset() };
		const size_t instanceId		{ baseId + instanceIndex };

		if (instanceId >= __instanceInfoHostBuffer.size())
			__instanceInfoHostBuffer.resize(instanceId);

		auto &instanceInfo{ __instanceInfoHostBuffer[instanceId] };

		const auto descLocation{ __pRenderer->getMaterialDescLocationOf(materialType) };
		if (descLocation.has_value())
		{
			const uint32_t slotIndex{ descLocation.value() - Constants::SUB_LAYER_MATERIAL_DESC_LOCATION0 };

			if (pMaterial)
			{
				const auto &pBufferBuilder{ __materialDataBufferBuilders.at(materialType) };
				instanceInfo.materialIds[slotIndex] = pBufferBuilder->getIdOf(pMaterial);
			}
			else
				instanceInfo.materialIds[slotIndex] = -1;
		}
	}

	void SubLayer::__validateInstanceInfoBuffer()
	{
		if (!__instanceInfoBufferInvalidated)
			return;

		const auto pLayerResourcePool{ __context.pLayerResourcePool };

		const size_t bufferSize{ __instanceInfoHostBuffer.size() * sizeof(InstanceInfo) };

		if (__pInstanceInfoBuffer)
			pLayerResourcePool->recycleStorageBuffer(std::move(__pInstanceInfoBuffer));

		__pInstanceInfoBuffer = pLayerResourcePool->getStorageBuffer(bufferSize);
		std::memcpy(__pInstanceInfoBuffer->getMappedMemory(), __instanceInfoHostBuffer.data(), bufferSize);

		__instanceInfoBufferInvalidated = false;
	}

	void SubLayer::__validateMaterialBufferBuilders()
	{
		for (const auto pBuilder : __invalidatedMaterialBufferBuilders)
			pBuilder->validate();

		__invalidatedMaterialBufferBuilders.clear();
	}

	void SubLayer::__validateDescriptorSet()
	{
		const auto pSubLayerDescSetLayout{ __pRenderer->getSubLayerDescSetLayout() };
		if (!pSubLayerDescSetLayout)
			return;

		const auto pDevice					{ __context.pLogicalDevice };
		const auto pDescriptorSetFactory	{ __context.pDescriptorSetFactory };

		std::vector<const Texture *> textureSlots;

		for (const auto &[pTexture, id] : __textureReferenceManager.getTextures())
		{
			if (id >= static_cast<uint32_t>(textureSlots.size()))
				textureSlots.resize(id + 1U);

			textureSlots[id] = pTexture;
		}

		const uint32_t textureSlotCount{ static_cast<uint32_t>(textureSlots.size()) };

		__pSubLayerDescSet = std::unique_ptr<Graphics::DescriptorSet>
		{
			pDescriptorSetFactory->createInstance(
				pSubLayerDescSetLayout->getHandle(), &textureSlotCount)
		};

		__descUpdater.reset();

		// Renderer infos
		__pRenderer->loadSubLayerDescInfos(__descUpdater, __pSubLayerDescSet->getHandle());

		// Instance info
		const auto pInstanceInfoBuffer{ __pInstanceInfoBuffer.get() };

		__descUpdater.addBufferInfo(
			__pSubLayerDescSet->getHandle(), Constants::SUB_LAYER_INSTANCE_INFO_LOCATION,
			0U, 1U, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &pInstanceInfoBuffer);

		// Material infos
		for (const auto &[type, pBuilder] : __materialDataBufferBuilders)
		{
			const auto pMaterialBuffer		{ &(pBuilder->getMaterialBuffer()) };
			const auto descLocation			{ __pRenderer->getMaterialDescLocationOf(type) };

			__descUpdater.addBufferInfo(
				__pSubLayerDescSet->getHandle(), descLocation.value(),
				0U, 1U, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, &pMaterialBuffer);
		}

		// Texture infos
		if (!(textureSlots.empty()))
		{
			__descUpdater.addTextureInfo(
				__pSubLayerDescSet->getHandle(), Constants::SUB_LAYER_TEXTURES_LOCATION,
				0U, textureSlotCount, VkDescriptorType::VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, textureSlots.data());
		}

		__descUpdater.update();
	}

	void SubLayer::__onObjectMeshChanged(const RenderObject *const pObject, const Mesh *const pPrev, const Mesh *const pCur) noexcept
	{
		__unregisterMesh(pObject, pPrev);

		if (pCur)
		{
			__registerMesh(pObject, pCur);
			__needRedrawEvent.invoke(this);
		}
	}

	void SubLayer::__onObjectMaterialChanged(
		const RenderObject *const pObject, const uint32_t instanceIndex,
		const std::type_index &type, const Material *const pPrev, const Material *const pCur) noexcept
	{
		if (pPrev)
			__unregisterMaterial(pPrev);

		if (pCur)
		{
			__registerMaterial(pCur);
			__validateInstanceInfoHostBuffer(pObject, instanceIndex, type, pCur);

			__instanceInfoBufferInvalidated = true;
			_invalidate();
			__needRedrawEvent.invoke(this);
		}
	}

	void SubLayer::__onObjectInstanceCountChanged(
		const RenderObject *const pObject, const uint32_t prev, const uint32_t cur) noexcept
	{
		auto &pRegion{ __object2Region.at(pObject) };
		pRegion.reset();
		pRegion = std::make_unique<Lib::Region>(__objectRegionAllocator, 1U, cur);

		__needRedrawEvent.invoke(this);
	}

	void SubLayer::__onObjectDrawableChanged(const RenderObject *const pObject, const bool cur) noexcept
	{
		if (cur)
			__registerObject(pObject);
		else
			__unregisterObject(pObject);
	}

	void SubLayer::__onMaterialBufferBuilderInvalidated(MaterialBufferBuilder *const pBuilder) noexcept
	{
		__invalidatedMaterialBufferBuilders.emplace(pBuilder);
		_invalidate();
		__needRedrawEvent.invoke(this);
	}
}
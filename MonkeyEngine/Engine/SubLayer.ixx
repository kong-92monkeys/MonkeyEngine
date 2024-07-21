module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Layer:SubLayer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Stateful;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Lib.GenericBuffer;
import ntmonkeys.com.Lib.IdAllocator;
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
import ntmonkeys.com.Engine.DescriptorSetCirculator;
import <cstdint>;
import <unordered_map>;
import <unordered_set>;
import <memory>;
import <typeindex>;

namespace Engine
{
	class MaterialBufferBuilder : public Lib::Unique, public Lib::Stateful<MaterialBufferBuilder>
	{
	public:
		MaterialBufferBuilder(LayerResourcePool &resourcePool) noexcept;
		virtual ~MaterialBufferBuilder() noexcept override;

		void registerMaterial(const Material *const pMaterial) noexcept;
		void unregisterMaterial(const Material *const pMaterial) noexcept;

		[[nodiscard]]
		uint32_t getIdOf(const Material *const pMaterial) const noexcept;

		[[nodiscard]]
		const BufferChunk &getBuffer() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		LayerResourcePool &__resourcePool;

		Lib::IdAllocator<uint32_t> __materialIdAllocator;
		std::unordered_map<const Material *, std::pair<size_t, uint32_t>> __materialRefIdMap;

		Lib::IdAllocator<uint32_t> __textureIdAllocator;
		std::unordered_map<const Texture *, std::pair<size_t, uint32_t>> __textureRefIdMap;

		Lib::GenericBuffer __materialHostBuffer;
		std::shared_ptr<BufferChunk> __pMaterialBuffer;

		std::vector<int> __textureLUTHostBuffer;

		Lib::EventListenerPtr<const Material *> __pMaterialUpdateListener;
		Lib::EventListenerPtr<const Material *, uint32_t, const Texture *, const Texture *> __pMaterialTextureChangeListener;

		void __registerTexture(const Texture *const pTexture) noexcept;
		void __unregisterTexture(const Texture *const pTexture) noexcept;

		void __validateMaterialHostBuffer(const Material *const pMaterial) noexcept;
		void __validateMaterialBuffer();

		void __validateTextureLUTHostBuffer(const Material *const pMaterial) noexcept;
		void __validateTextureLUTHostBuffer(const Material *const pMaterial, const uint32_t slotIndex, const Texture *const pTexture) noexcept;

		void __onMaterialUpdated(const Material *const pMaterial) noexcept;
		void __onMaterialTextureChanged(
			const Material *const pMaterial, const uint32_t slotIndex,
			const Texture *const pPrev, const Texture *const pCur) noexcept;
	};

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

		std::unordered_map<const RenderObject *, std::unique_ptr<Lib::Region>> __object2Region;
		std::unordered_map<const Mesh *, std::unordered_set<const RenderObject *>> __mesh2Objects;

		bool __instanceInfoBufferInvalidated{ };
		std::vector<InstanceInfo> __instanceInfoHostBuffer;
		std::shared_ptr<BufferChunk> __pInstanceInfoBuffer;

		std::unordered_map<std::type_index, std::unique_ptr<MaterialBufferBuilder>> __materialDataBufferBuilders;
		std::unordered_set<MaterialBufferBuilder *> __invalidatedMaterialBufferBuilders;

		std::shared_ptr<DescriptorSetCirculator> __pDescSetCirculator;
		Graphics::DescriptorSet *__pDescSet{ };

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

	MaterialBufferBuilder::MaterialBufferBuilder(LayerResourcePool &resourcePool) noexcept :
		__resourcePool{ resourcePool }
	{
		__pMaterialUpdateListener =
			Lib::EventListener<const Material *>::bind(&MaterialBufferBuilder::__onMaterialUpdated, this, std::placeholders::_1);

		__pMaterialTextureChangeListener =
			Lib::EventListener<const Material *, uint32_t, const Texture *, const Texture *>::bind(
				&MaterialBufferBuilder::__onMaterialTextureChanged, this,
				std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);
	}

	MaterialBufferBuilder::~MaterialBufferBuilder() noexcept
	{
		if (__pMaterialBuffer)
			__resourcePool.recycleStorageBuffer(std::move(__pMaterialBuffer));
	}

	void MaterialBufferBuilder::registerMaterial(const Material *const pMaterial) noexcept
	{
		auto &[ref, id]{ __materialRefIdMap[pMaterial] };
		if (!ref)
		{
			pMaterial->getUpdateEvent() += __pMaterialUpdateListener;
			pMaterial->getTextureChangeEvent() += __pMaterialTextureChangeListener;

			for (const auto &[_, pTexture] : pMaterial->getTextures())
			{
				if (pTexture)
					__registerTexture(pTexture);
			}

			id = __materialIdAllocator.allocate();
			__validateMaterialHostBuffer(pMaterial);
			__validateTextureLUTHostBuffer(pMaterial);

			_invalidate();
		}

		++ref;
	}

	void MaterialBufferBuilder::unregisterMaterial(const Material *const pMaterial) noexcept
	{
		auto &[ref, id] { __materialRefIdMap[pMaterial] };
		--ref;

		if (!ref)
		{
			pMaterial->getUpdateEvent() -= __pMaterialUpdateListener;
			pMaterial->getTextureChangeEvent() -= __pMaterialTextureChangeListener;

			for (const auto &[_, pTexture] : pMaterial->getTextures())
			{
				if (pTexture)
					__unregisterTexture(pTexture);
			}

			__materialIdAllocator.free(id);
			__materialRefIdMap.erase(pMaterial);
		}
	}

	uint32_t MaterialBufferBuilder::getIdOf(const Material *const pMaterial) const noexcept
	{
		return __materialRefIdMap.at(pMaterial).second;
	}

	const BufferChunk &MaterialBufferBuilder::getBuffer() const noexcept
	{
		return *__pMaterialBuffer;
	}

	void MaterialBufferBuilder::_onValidate()
	{
		__validateMaterialBuffer();
	}

	void MaterialBufferBuilder::__validateTextureLUTHostBuffer(const Material *const pMaterial) noexcept
	{
		const uint32_t slotCount{ pMaterial->getTextureSlotCount() };
		if (!slotCount)
			return;

		const uint32_t materialId		{ __materialRefIdMap.at(pMaterial).second };
		const uint32_t slotBaseIndex	{ materialId * slotCount };
		const uint32_t slotMaxIndex		{ slotBaseIndex + slotCount };

		if (slotMaxIndex >= static_cast<uint32_t>(__textureLUTHostBuffer.size()))
			__textureLUTHostBuffer.resize(slotMaxIndex);

		const auto &textures{ pMaterial->getTextures() };

		for (uint32_t slotIter{ }; slotIter < slotCount; ++slotIter)
		{
			const Texture *pTexture{ };

			const auto foundIt{ textures.find(slotIter) };
			if (foundIt != textures.end())
				pTexture = foundIt->second;

			int &textureId{ __textureLUTHostBuffer[slotBaseIndex + slotIter] };
			textureId = (pTexture ? static_cast<int>(__textureRefIdMap.at(pTexture).second) : -1);
		}
	}

	void MaterialBufferBuilder::__validateTextureLUTHostBuffer(
		const Material *const pMaterial, const uint32_t slotIndex, const Texture *const pTexture) noexcept
	{
		const uint32_t slotCount{ pMaterial->getTextureSlotCount() };
		if (!slotCount)
			return;

		const uint32_t materialId		{ __materialRefIdMap.at(pMaterial).second };
		const uint32_t slotBaseIndex	{ materialId * slotCount };
		const uint32_t slotMaxIndex		{ slotBaseIndex + slotCount };

		if (slotMaxIndex >= static_cast<uint32_t>(__textureLUTHostBuffer.size()))
			__textureLUTHostBuffer.resize(slotMaxIndex);

		int &textureId{ __textureLUTHostBuffer[slotBaseIndex + slotIndex] };
		textureId = (pTexture ? static_cast<int>(__textureRefIdMap.at(pTexture).second) : -1);
	}

	void MaterialBufferBuilder::__registerTexture(const Texture *const pTexture) noexcept
	{
		auto &[ref, id]{ __textureRefIdMap[pTexture] };
		if (!ref)
			id = __textureIdAllocator.allocate();

		++ref;
	}

	void MaterialBufferBuilder::__unregisterTexture(const Texture *const pTexture) noexcept
	{
		auto &[ref, id] { __textureRefIdMap[pTexture] };
		--ref;

		if (!ref)
		{
			__textureIdAllocator.free(id);
			__textureRefIdMap.erase(pTexture);
		}
	}

	void MaterialBufferBuilder::__validateMaterialHostBuffer(const Material *const pMaterial) noexcept
	{
		const uint32_t materialId	{ __materialRefIdMap.at(pMaterial).second };

		const size_t materialSize	{ pMaterial->getSize() };
		const size_t memOffset		{ materialId * materialSize };

		if (memOffset >= __materialHostBuffer.getSize())
			__materialHostBuffer.resize(memOffset + materialSize);

		__materialHostBuffer.set(memOffset, pMaterial->getData(), materialSize);
	}

	void MaterialBufferBuilder::__validateMaterialBuffer()
	{
		const size_t bufferSize{ __materialHostBuffer.getSize() };

		if (__pMaterialBuffer)
			__resourcePool.recycleStorageBuffer(std::move(__pMaterialBuffer));

		__pMaterialBuffer = __resourcePool.getStorageBuffer(bufferSize);
		std::memcpy(__pMaterialBuffer->getMappedMemory(), __materialHostBuffer.getData(), bufferSize);
	}

	void MaterialBufferBuilder::__onMaterialUpdated(const Material *const pMaterial) noexcept
	{
		__validateMaterialHostBuffer(pMaterial);
		_invalidate();
	}

	void MaterialBufferBuilder::__onMaterialTextureChanged(
		const Material *const pMaterial, const uint32_t slotIndex,
		const Texture *const pPrev, const Texture *const pCur) noexcept
	{
		if (pPrev)
			__unregisterTexture(pPrev);

		if (pCur)
			__registerTexture(pCur);

		__validateTextureLUTHostBuffer(pMaterial, slotIndex, pCur);
	}

	SubLayer::SubLayer(const EngineContext &context, const Renderer *const pRenderer) noexcept :
		__context{ context }, __pRenderer{ pRenderer }
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

		const auto pDescSetLayout{ pRenderer->getSubLayerDescSetLayout() };
		if (pDescSetLayout)
		{
			__pDescSetCirculator = std::make_shared<DescriptorSetCirculator>(
				*(context.pLogicalDevice), *pDescSetLayout, 10U);
		}
	}

	SubLayer::~SubLayer() noexcept
	{
		const auto pLayerResourcePool	{ __context.pLayerResourcePool };
		const auto pLazyDeleter			{ __context.pLazyDeleter };

		if (__pInstanceInfoBuffer)
			pLayerResourcePool->recycleStorageBuffer(std::move(__pInstanceInfoBuffer));

		if (__pDescSetCirculator)
			pLazyDeleter->reserve(std::move(__pDescSetCirculator));
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

		if (__pDescSet)
		{
			commandBuffer.bindDescriptorSets(
				VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS,
				__pRenderer->getPipelineLayout().getHandle(),
				Constants::SUB_LAYER_DESC_SET_LOCATION, 1U, &(__pDescSet->getHandle()), 0U, nullptr);
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
			pBufferBuilder = std::make_unique<MaterialBufferBuilder>(*(__context.pLayerResourcePool));
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

				const auto descLocation{ __pRenderer->getDescriptorLocationOf(materialType) };
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

		const auto descLocation{ __pRenderer->getDescriptorLocationOf(materialType) };
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
		if (!__pDescSetCirculator)
			return;

		const auto pDevice{ __context.pLogicalDevice };

		__pDescSet = &(__pDescSetCirculator->getNext());

		std::vector<VkWriteDescriptorSet> descWrites;
		std::vector<std::unique_ptr<VkDescriptorBufferInfo>> bufferInfos;

		auto &pInstanceInfoBufferInfo		{ bufferInfos.emplace_back(std::make_unique<VkDescriptorBufferInfo>()) };
		pInstanceInfoBufferInfo->buffer		= __pInstanceInfoBuffer->getBuffer().getHandle();
		pInstanceInfoBufferInfo->offset		= __pInstanceInfoBuffer->getOffset();
		pInstanceInfoBufferInfo->range		= __pInstanceInfoBuffer->getSize();

		auto &instanceInfoWrites			{ descWrites.emplace_back() };
		instanceInfoWrites.sType			= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		instanceInfoWrites.dstSet			= __pDescSet->getHandle();
		instanceInfoWrites.dstBinding		= Constants::SUB_LAYER_INSTANCE_INFO_LOCATION;
		instanceInfoWrites.dstArrayElement	= 0U;
		instanceInfoWrites.descriptorCount	= 1U;
		instanceInfoWrites.descriptorType	= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		instanceInfoWrites.pBufferInfo		= pInstanceInfoBufferInfo.get();

		for (const auto &[type, pBuilder] : __materialDataBufferBuilders)
		{
			const auto &materialBuffer		{ pBuilder->getBuffer() };
			const auto descLocation			{ __pRenderer->getDescriptorLocationOf(type) };

			auto &pMaterialBufferInfo		{ bufferInfos.emplace_back(std::make_unique<VkDescriptorBufferInfo>()) };
			pMaterialBufferInfo->buffer		= materialBuffer.getBuffer().getHandle();
			pMaterialBufferInfo->offset		= materialBuffer.getOffset();
			pMaterialBufferInfo->range		= materialBuffer.getSize();

			auto &materialWrites			{ descWrites.emplace_back() };
			materialWrites.sType			= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			materialWrites.dstSet			= __pDescSet->getHandle();
			materialWrites.dstBinding		= descLocation.value();
			materialWrites.dstArrayElement	= 0U;
			materialWrites.descriptorCount	= 1U;
			materialWrites.descriptorType	= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			materialWrites.pBufferInfo		= pMaterialBufferInfo.get();
		}

		pDevice->updateDescriptorSets(static_cast<uint32_t>(descWrites.size()), descWrites.data(), 0U, nullptr);
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
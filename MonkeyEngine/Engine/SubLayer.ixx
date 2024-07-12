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
import ntmonkeys.com.Engine.ShaderDataStructures;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.Mesh;
import ntmonkeys.com.Engine.DrawParam;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.RenderObject;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Engine.LayerResourcePool;
import <cstdint>;
import <unordered_map>;
import <unordered_set>;
import <memory>;
import <typeindex>;

namespace Engine
{
	class MaterialDependencies : public Lib::Unique, public Lib::Stateful<MaterialDependencies>
	{
	public:
		MaterialDependencies(LayerResourcePool &resourcePool) noexcept;

		void registerMaterial(const Material *const pMaterial) noexcept;
		void unregisterMaterial(const Material *const pMaterial) noexcept;

		[[nodiscard]]
		uint32_t getIdOf(const Material *const pMaterial) const noexcept;

		[[nodiscard]]
		const BufferChunk &getDataBuffer() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		LayerResourcePool &__resourcePool;

		Lib::IdAllocator<uint32_t> __idAllocator;
		std::unordered_map<const Material *, std::pair<size_t, uint32_t>> __refIdMap;

		Lib::GenericBuffer __hostBuffer;
		std::shared_ptr<BufferChunk> __pDataBuffer;

		Lib::EventListenerPtr<const Material *> __pMaterialInvalidateListener;

		void __validateHostBuffer(const Material *const pMaterial) noexcept;
		void __validateDataBuffer();

		void __onMaterialInvalidated(const Material *const pMaterial) noexcept;
	};

	export class SubLayer : public Lib::Unique, public Lib::Stateful<SubLayer>
	{
	public:
		SubLayer(const EngineContext &context, const Renderer *const pRenderer) noexcept;

		void addRenderObject(const RenderObject *const pObject) noexcept;
		void removeRenderObject(const RenderObject *const pObject) noexcept;

		[[nodiscard]]
		bool isEmpty() const noexcept;

		void draw(Graphics::CommandBuffer &commandBuffer);

	protected:
		virtual void _onValidate() override;

	private:
		const EngineContext &__context;
		const Renderer *const __pRenderer;

		Lib::RegionAllocator __objectRegionAllocator{ UINT32_MAX };

		std::unordered_map<const RenderObject *, std::unique_ptr<Lib::Region>> __object2Region;
		std::unordered_map<const Mesh *, std::unordered_set<const RenderObject *>> __mesh2Objects;
		std::unordered_map<const MaterialPack *, std::unordered_map<const RenderObject *, uint32_t>> __materialPack2ObjectInstances;

		bool __instanceInfoInvalidated{ };
		std::vector<InstanceInfo> __instanceInfoHostBuffer;
		std::shared_ptr<BufferChunk> __pInstanceInfoDataBuffer;

		std::unordered_map<std::type_index, std::unique_ptr<MaterialDependencies>> __materialDependencies;

		Lib::EventListenerPtr<const RenderObject *, const Mesh *, const Mesh *> __pObjectMeshChangeListener;
		Lib::EventListenerPtr<const RenderObject *, const DrawParam *, const DrawParam *> __pObjectDrawParamChangeListener;
		Lib::EventListenerPtr<const RenderObject *, uint32_t, const MaterialPack *, const MaterialPack *> __pObjectMaterialPackChangeListener;
		Lib::EventListenerPtr<const RenderObject *, uint32_t, uint32_t> __pObjectInstanceCountChangeListener;
		Lib::EventListenerPtr<const RenderObject *, bool, bool> __pObjectDrawableChangeListener;

		Lib::EventListenerPtr<const MaterialPack *, std::type_index, const Material *, const Material *> __pMaterialPackMaterialChangeListener;
		Lib::EventListenerPtr<const MaterialDependencies *> __pMaterialDependenciesInvalidateListener;

		void __registerObject(const RenderObject *const pObject);
		void __unregisterObject(const RenderObject *const pObject);

		void __registerMesh(const RenderObject *const pObject, const Mesh *const pMesh);
		void __unregisterMesh(const RenderObject *const pObject, const Mesh *const pMesh);

		void __registerMaterialPack(const RenderObject *const pObject, const MaterialPack *const pMaterialPack, const uint32_t instanceIndex);
		void __unregisterMaterialPack(const RenderObject *const pObject, const MaterialPack *const pMaterialPack);

		void __registerMaterial(const Material *const pMaterial);
		void __unregisterMaterial(const Material *const pMaterial);

		void __validateInstanceInfoHostBuffer(const RenderObject *const pObject);
		void __validateInstanceInfoHostBuffer(
			const RenderObject *const pObject, const uint32_t instanceIndex,
			const std::type_index &materialType, const Material *const pMaterial);

		void __validateInstanceInfoDataBuffer();
		void __validateMaterialDependencies();

		void __onObjectMeshChanged(
			const RenderObject *const pObject,
			const Mesh *const pPrev, const Mesh *const pCur) noexcept;

		void __onObjectDrawParamChanged() noexcept;
		
		void __onObjectMaterialPackChanged(
			const RenderObject *const pObject, const uint32_t instanceIndex,
			const MaterialPack *const pPrev, const MaterialPack *const pCur) noexcept;
		
		void __onObjectInstanceCountChanged(
			const RenderObject *const pObject, const uint32_t prev, const uint32_t cur) noexcept;

		void __onObjectDrawableChanged(
			const RenderObject *const pObject, const bool prev, const bool cur) noexcept;

		void __onMaterialPackMaterialChanged(
			const MaterialPack *const pMaterialPack, const std::type_index &type,
			const Material *const pPrev, const Material *const pCur) noexcept;

		void __onMaterialDependenciesInvalidated() noexcept;
	};

	MaterialDependencies::MaterialDependencies(LayerResourcePool &resourcePool) noexcept :
		__resourcePool{ resourcePool }
	{
		__pMaterialInvalidateListener =
			Lib::EventListener<const Material *>::bind(&MaterialDependencies::__onMaterialInvalidated, this, std::placeholders::_1);
	}

	void MaterialDependencies::registerMaterial(const Material *const pMaterial) noexcept
	{
		pMaterial->getInvalidateEvent() += __pMaterialInvalidateListener;
		
		auto &[ref, id]{ __refIdMap[pMaterial] };
		if (!ref)
		{
			id = __idAllocator.allocate();
			__validateHostBuffer(pMaterial);
			_invalidate();
		}

		++ref;
	}

	void MaterialDependencies::unregisterMaterial(const Material *const pMaterial) noexcept
	{
		pMaterial->getInvalidateEvent() -= __pMaterialInvalidateListener;

		auto &[ref, id] { __refIdMap[pMaterial] };
		--ref;

		if (!ref)
		{
			__idAllocator.free(id);
			__refIdMap.erase(pMaterial);
		}
	}

	uint32_t MaterialDependencies::getIdOf(const Material *const pMaterial) const noexcept
	{
		return __refIdMap.at(pMaterial).second;
	}

	const BufferChunk &MaterialDependencies::getDataBuffer() const noexcept
	{
		return *__pDataBuffer;
	}

	void MaterialDependencies::_onValidate()
	{
		__validateDataBuffer();
	}

	void MaterialDependencies::__validateHostBuffer(const Material *const pMaterial) noexcept
	{
		const uint32_t materialId	{ __refIdMap.at(pMaterial).second };

		const size_t materialSize	{ pMaterial->getSize() };
		const size_t memOffset		{ materialId * materialSize };

		if (memOffset >= __hostBuffer.getSize())
			__hostBuffer.resize(memOffset + materialSize);

		__hostBuffer.set(memOffset, pMaterial->getData(), materialSize);
	}

	void MaterialDependencies::__validateDataBuffer()
	{
		const size_t bufferSize{ __hostBuffer.getSize() };

		if (__pDataBuffer)
			__resourcePool.recycleStorageBuffer(std::move(__pDataBuffer));

		__pDataBuffer = __resourcePool.getStorageBuffer(bufferSize);
		std::memcpy(__pDataBuffer->getMappedMemory(), __hostBuffer.getData(), bufferSize);
	}

	void MaterialDependencies::__onMaterialInvalidated(const Material *const pMaterial) noexcept
	{
		__validateHostBuffer(pMaterial);
		_invalidate();
	}

	SubLayer::SubLayer(const EngineContext &context, const Renderer *const pRenderer) noexcept :
		__context{ context }, __pRenderer{ pRenderer }
	{
		__pObjectMeshChangeListener =
			Lib::EventListener<const RenderObject *, const Mesh *, const Mesh *>::bind(
				&SubLayer::__onObjectMeshChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectDrawParamChangeListener =
			Lib::EventListener<const RenderObject *, const DrawParam *, const DrawParam *>::bind(
				&SubLayer::__onObjectDrawParamChanged, this);

		__pObjectMaterialPackChangeListener =
			Lib::EventListener<const RenderObject *, uint32_t, const MaterialPack *, const MaterialPack *>::bind(
				&SubLayer::__onObjectMaterialPackChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		__pObjectInstanceCountChangeListener =
			Lib::EventListener<const RenderObject *, uint32_t, uint32_t>::bind(
				&SubLayer::__onObjectInstanceCountChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pObjectDrawableChangeListener = 
			Lib::EventListener<const RenderObject *, bool, bool>::bind(
				&SubLayer::__onObjectDrawableChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);

		__pMaterialPackMaterialChangeListener =
			Lib::EventListener<const MaterialPack *, std::type_index, const Material *, const Material *>::bind(
				&SubLayer::__onMaterialPackMaterialChanged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4);

		__pMaterialDependenciesInvalidateListener =
			Lib::EventListener<const MaterialDependencies *>::bind(&SubLayer::__onMaterialDependenciesInvalidated, this);
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

	void SubLayer::draw(Graphics::CommandBuffer &commandBuffer)
	{

	}

	void SubLayer::_onValidate()
	{
		__validateInstanceInfoDataBuffer();
		__validateMaterialDependencies();
	}

	void SubLayer::__registerObject(const RenderObject *const pObject)
	{
		pObject->getMeshChangeEvent() += __pObjectMeshChangeListener;
		pObject->getMaterialPackChangeEvent() += __pObjectMaterialPackChangeListener;
		pObject->getInstanceCountChangeEvent() += __pObjectInstanceCountChangeListener;

		const uint32_t instanceCount{ pObject->getInstanceCount() };
		__object2Region[pObject] = std::make_unique<Lib::Region>(__objectRegionAllocator, 1U, instanceCount);

		__registerMesh(pObject, pObject->getMesh());

		for (uint32_t instanceIter{ }; instanceIter < instanceCount; ++instanceIter)
			__registerMaterialPack(pObject, pObject->getMaterialPack(instanceIter), instanceIter);

		__validateInstanceInfoHostBuffer(pObject);
		__instanceInfoInvalidated = true;

		_invalidate();
	}

	void SubLayer::__unregisterObject(const RenderObject *const pObject)
	{
		pObject->getMeshChangeEvent() -= __pObjectMeshChangeListener;
		pObject->getMaterialPackChangeEvent() -= __pObjectMaterialPackChangeListener;
		pObject->getInstanceCountChangeEvent() -= __pObjectInstanceCountChangeListener;

		__object2Region.erase(pObject);

		const auto pMesh{ pObject->getMesh() };
		if (pMesh)
			__unregisterMesh(pObject, pMesh);

		for (uint32_t instanceIter{ }; instanceIter < pObject->getInstanceCount(); ++instanceIter)
		{
			const auto pMaterialPack{ pObject->getMaterialPack(instanceIter) };
			if (!pMaterialPack)
				continue;

			__unregisterMaterialPack(pObject, pMaterialPack);
		}

		_invalidate();
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

	void SubLayer::__registerMaterialPack(const RenderObject *const pObject, const MaterialPack *const pMaterialPack, const uint32_t instanceIndex)
	{
		pMaterialPack->getMaterialChangeEvent() += __pMaterialPackMaterialChangeListener;
		__materialPack2ObjectInstances[pMaterialPack][pObject] = instanceIndex;

		for (const auto pMaterial : *pMaterialPack)
			__registerMaterial(pMaterial);

	}

	void SubLayer::__unregisterMaterialPack(const RenderObject *const pObject, const MaterialPack *const pMaterialPack)
	{
		pMaterialPack->getMaterialChangeEvent() -= __pMaterialPackMaterialChangeListener;
		__materialPack2ObjectInstances[pMaterialPack].erase(pObject);

		for (const auto pMaterial : *pMaterialPack)
			__unregisterMaterial(pMaterial);
	}

	void SubLayer::__registerMaterial(const Material *const pMaterial)
	{
		auto &pDependencies{ __materialDependencies[typeid(*pMaterial)] };
		if (!pDependencies)
		{
			pDependencies = std::make_unique<MaterialDependencies>(*(__context.pLayerResourcePool));
			pDependencies->getInvalidateEvent() += __pMaterialDependenciesInvalidateListener;
		}

		pDependencies->registerMaterial(pMaterial);
	}

	void SubLayer::__unregisterMaterial(const Material *const pMaterial)
	{
		const auto &pDependencies{ __materialDependencies[typeid(*pMaterial)] };
		pDependencies->unregisterMaterial(pMaterial);
	}

	void SubLayer::__validateInstanceInfoHostBuffer(const RenderObject *const pObject)
	{
		const auto &pRegion{ __object2Region.at(pObject) };

		const size_t instanceCount	{ pObject->getInstanceCount() };
		const size_t baseId			{ pRegion->getOffset() };
		const size_t maxId			{ baseId + instanceCount };

		if (maxId >= __instanceInfoHostBuffer.size())
			__instanceInfoHostBuffer.resize(maxId);

		for (size_t instanceIter{ }; instanceIter < instanceCount; ++instanceIter)
		{
			auto &instanceInfo{ __instanceInfoHostBuffer[baseId + instanceIter] };
			instanceInfo.reset();

			const auto pMaterialPack{ pObject->getMaterialPack(static_cast<uint32_t>(instanceIter)) };
			if (!pMaterialPack)
				continue;

			for (const auto pMaterial : *pMaterialPack)
			{
				const std::type_index materialType{ typeid(*pMaterial) };

				const auto descLocation{ __pRenderer->getDescriptorLocationOf(materialType) };
				if (descLocation.has_value())
				{
					const uint32_t location		{ descLocation.value() };
					const auto &pDependencies	{ __materialDependencies.at(materialType) };
					instanceInfo.materialIds[location] = pDependencies->getIdOf(pMaterial);
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
			const uint32_t location{ descLocation.value() };

			if (pMaterial)
			{
				const auto &pDependencies{ __materialDependencies.at(materialType) };
				instanceInfo.materialIds[location] = pDependencies->getIdOf(pMaterial);
			}
			else
				instanceInfo.materialIds[location] = -1;
		}
	}

	void SubLayer::__validateInstanceInfoDataBuffer()
	{
		if (!__instanceInfoInvalidated)
			return;

		const auto pLayerResourcePool{ __context.pLayerResourcePool };

		const size_t bufferSize{ __instanceInfoHostBuffer.size() * sizeof(InstanceInfo) };

		if (__pInstanceInfoDataBuffer)
			pLayerResourcePool->recycleStorageBuffer(std::move(__pInstanceInfoDataBuffer));

		__pInstanceInfoDataBuffer = pLayerResourcePool->getStorageBuffer(bufferSize);
		std::memcpy(__pInstanceInfoDataBuffer->getMappedMemory(), __instanceInfoHostBuffer.data(), bufferSize);

		__instanceInfoInvalidated = false;
	}

	void SubLayer::__validateMaterialDependencies()
	{
		for (const auto &[_, pDependencies] : __materialDependencies)
			pDependencies->validate();
	}

	void SubLayer::__onObjectMeshChanged(
		const RenderObject *const pObject, const Mesh *const pPrev, const Mesh *const pCur) noexcept
	{
		if (pPrev)
			__unregisterMesh(pObject, pPrev);

		if (pCur)
			__registerMesh(pObject, pCur);
	}

	void SubLayer::__onObjectDrawParamChanged() noexcept
	{
		_invalidate();
	}

	void SubLayer::__onObjectMaterialPackChanged(
		const RenderObject *const pObject, const uint32_t instanceIndex,
		const MaterialPack *const pPrev, const MaterialPack *const pCur) noexcept
	{
		if (pPrev)
			__unregisterMaterialPack(pObject, pPrev);

		if (pCur)
			__registerMaterialPack(pObject, pCur, instanceIndex);
	}

	void SubLayer::__onObjectInstanceCountChanged(
		const RenderObject *const pObject, const uint32_t prev, const uint32_t cur) noexcept
	{
		auto &pRegion{ __object2Region.at(pObject) };
		pRegion.reset();
		pRegion = std::make_unique<Lib::Region>(__objectRegionAllocator, 1U, cur);

		__validateInstanceInfoHostBuffer(pObject);
		__instanceInfoInvalidated = true;
		_invalidate();
	}

	void SubLayer::__onObjectDrawableChanged(
		const RenderObject *pObject, const bool prev, const bool cur) noexcept
	{
		if (cur)
			__registerObject(pObject);
		else
			__unregisterObject(pObject);
	}

	void SubLayer::__onMaterialPackMaterialChanged(
		const MaterialPack *const pMaterialPack, const std::type_index &type,
		const Material *const pPrev, const Material *const pCur) noexcept
	{
		if (pPrev)
			__unregisterMaterial(pPrev);

		if (pCur)
			__registerMaterial(pCur);

		const auto &objectInstances{ __materialPack2ObjectInstances.at(pMaterialPack) };
		for (const auto &[pObject, instanceIndex] : objectInstances)
			__validateInstanceInfoHostBuffer(pObject, instanceIndex, type, pCur);

		__instanceInfoInvalidated = true;
		_invalidate();
	}

	void SubLayer::__onMaterialDependenciesInvalidated() noexcept
	{
		_invalidate();
	}
}
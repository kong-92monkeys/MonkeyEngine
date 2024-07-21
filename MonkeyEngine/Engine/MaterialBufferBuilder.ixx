export module ntmonkeys.com.Engine.Layer:MaterialBufferBuilder;

import :TextureReferenceManager;
import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Stateful;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Lib.GenericBuffer;
import ntmonkeys.com.Lib.IdAllocator;
import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Lib.LazyRecycler;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.Texture;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Engine.LayerResourcePool;
import <cstdint>;
import <unordered_map>;
import <memory>;

namespace Engine
{
	class MaterialBufferBuilder : public Lib::Unique, public Lib::Stateful<MaterialBufferBuilder>
	{
	public:
		MaterialBufferBuilder(
			LayerResourcePool &resourcePool,
			TextureReferenceManager &textureReferenceManager) noexcept;

		virtual ~MaterialBufferBuilder() noexcept override;

		void registerMaterial(const Material *const pMaterial) noexcept;
		void unregisterMaterial(const Material *const pMaterial) noexcept;

		[[nodiscard]]
		uint32_t getIdOf(const Material *const pMaterial) const noexcept;

		[[nodiscard]]
		const BufferChunk &getMaterialBuffer() const noexcept;

		[[nodiscard]]
		const BufferChunk *getTextureLUTBuffer() const noexcept;

	protected:
		virtual void _onValidate() override;

	private:
		LayerResourcePool &__resourcePool;
		TextureReferenceManager &__textureReferenceManager;

		Lib::IdAllocator<uint32_t> __materialIdAllocator;
		std::unordered_map<const Material *, std::pair<size_t, uint32_t>> __materialRefIdMap;

		Lib::GenericBuffer __materialHostBuffer;
		std::shared_ptr<BufferChunk> __pMaterialBuffer;
		bool __materialBufferInvalidated{ };

		std::vector<int> __textureLUTHostBuffer;
		std::shared_ptr<BufferChunk> __pTextureLUTBuffer;
		bool __textureLUTBufferInvalidated{ };

		Lib::EventListenerPtr<const Material *> __pMaterialUpdateListener;
		Lib::EventListenerPtr<const Material *, uint32_t, const Texture *, const Texture *> __pMaterialTextureChangeListener;

		void __validateMaterialHostBuffer(const Material *const pMaterial) noexcept;
		void __validateMaterialBuffer();

		void __validateTextureLUTHostBuffer(const Material *const pMaterial) noexcept;
		void __validateTextureLUTHostBuffer(const Material *const pMaterial, const uint32_t slotIndex, const Texture *const pTexture) noexcept;
		void __validateTextureLUTBuffer();

		void __onMaterialUpdated(const Material *const pMaterial) noexcept;
		void __onMaterialTextureChanged(
			const Material *const pMaterial, const uint32_t slotIndex,
			const Texture *const pPrev, const Texture *const pCur) noexcept;
	};

	MaterialBufferBuilder::MaterialBufferBuilder(
		LayerResourcePool &resourcePool,
		TextureReferenceManager &textureReferenceManager) noexcept :
		__resourcePool				{ resourcePool },
		__textureReferenceManager	{ textureReferenceManager }
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
					__textureReferenceManager.registerTexture(pTexture);
			}

			id = __materialIdAllocator.allocate();

			__validateMaterialHostBuffer(pMaterial);
			__materialBufferInvalidated = true;

			__validateTextureLUTHostBuffer(pMaterial);
			__textureLUTBufferInvalidated = true;

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
					__textureReferenceManager.unregisterTexture(pTexture);
			}

			__materialIdAllocator.free(id);
			__materialRefIdMap.erase(pMaterial);
		}
	}

	uint32_t MaterialBufferBuilder::getIdOf(const Material *const pMaterial) const noexcept
	{
		return __materialRefIdMap.at(pMaterial).second;
	}

	const BufferChunk &MaterialBufferBuilder::getMaterialBuffer() const noexcept
	{
		return *__pMaterialBuffer;
	}

	const BufferChunk *MaterialBufferBuilder::getTextureLUTBuffer() const noexcept
	{
		return __pTextureLUTBuffer.get();
	}

	void MaterialBufferBuilder::_onValidate()
	{
		__validateMaterialBuffer();
		__validateTextureLUTBuffer();
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
		if (!__materialBufferInvalidated)
			return;

		const size_t bufferSize{ __materialHostBuffer.getSize() };

		if (__pMaterialBuffer)
			__resourcePool.recycleStorageBuffer(std::move(__pMaterialBuffer));

		__pMaterialBuffer = __resourcePool.getStorageBuffer(bufferSize);
		std::memcpy(__pMaterialBuffer->getMappedMemory(), __materialHostBuffer.getData(), bufferSize);

		__materialBufferInvalidated = false;
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

			__textureLUTHostBuffer[slotBaseIndex + slotIter] =
				(pTexture ? static_cast<int>(__textureReferenceManager.getTextures().at(pTexture)) : -1);
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

		__textureLUTHostBuffer[slotBaseIndex + slotIndex] =
			(pTexture ? static_cast<int>(__textureReferenceManager.getTextures().at(pTexture)) : -1);
	}

	void MaterialBufferBuilder::__validateTextureLUTBuffer()
	{
		if (!__textureLUTBufferInvalidated)
			return;

		const size_t bufferSize{ __textureLUTHostBuffer.size() * sizeof(int) };

		if (__pTextureLUTBuffer)
			__resourcePool.recycleStorageBuffer(std::move(__pTextureLUTBuffer));

		__pTextureLUTBuffer = __resourcePool.getStorageBuffer(bufferSize);
		std::memcpy(__pTextureLUTBuffer->getMappedMemory(), __textureLUTHostBuffer.data(), bufferSize);

		__textureLUTBufferInvalidated = false;
	}

	void MaterialBufferBuilder::__onMaterialUpdated(const Material *const pMaterial) noexcept
	{
		__validateMaterialHostBuffer(pMaterial);
		__materialBufferInvalidated = true;
		_invalidate();
	}

	void MaterialBufferBuilder::__onMaterialTextureChanged(
		const Material *const pMaterial, const uint32_t slotIndex,
		const Texture *const pPrev, const Texture *const pCur) noexcept
	{
		if (pPrev)
			__textureReferenceManager.unregisterTexture(pPrev);

		if (pCur)
			__textureReferenceManager.registerTexture(pCur);

		__validateTextureLUTHostBuffer(pMaterial, slotIndex, pCur);
		__textureLUTBufferInvalidated = true;
		_invalidate();
	}
}
export module ntmonkeys.com.Engine.Layer:TextureReferenceManager;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Lib.IdAllocator;
import ntmonkeys.com.Engine.Texture;
import <cstdint>;
import <unordered_map>;

namespace Engine
{
	class TextureReferenceManager : public Lib::Unique
	{
	public:
		void registerTexture(const Texture *const pTexture) noexcept;
		void unregisterTexture(const Texture *const pTexture) noexcept;

		[[nodiscard]]
		constexpr const std::unordered_map<const Texture *, uint32_t> &getTextures() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const TextureReferenceManager *> &getUpdateEvent() const noexcept;

	private:
		Lib::IdAllocator<uint32_t> __idAllocator;
		std::unordered_map<const Texture *, size_t> __refMap;
		std::unordered_map<const Texture *, uint32_t> __idMap;

		mutable Lib::Event<const TextureReferenceManager *> __updateEvent;
	};

	void TextureReferenceManager::registerTexture(const Texture *const pTexture) noexcept
	{
		auto &ref{ __refMap[pTexture] };
		if (!ref)
		{
			__idMap[pTexture] = __idAllocator.allocate();
			__updateEvent.invoke(this);
		}

		++ref;
	}

	void TextureReferenceManager::unregisterTexture(const Texture *const pTexture) noexcept
	{
		auto &ref{ __refMap[pTexture] };
		--ref;

		if (!ref)
		{
			const uint32_t id{ __idMap.extract(pTexture).mapped() };
			__idAllocator.free(id);
			__updateEvent.invoke(this);
		}
	}

	constexpr const std::unordered_map<const Texture *, uint32_t> &TextureReferenceManager::getTextures() const noexcept
	{
		return __idMap;
	}

	constexpr Lib::EventView<const TextureReferenceManager *> &TextureReferenceManager::getUpdateEvent() const noexcept
	{
		return __updateEvent;
	}
}
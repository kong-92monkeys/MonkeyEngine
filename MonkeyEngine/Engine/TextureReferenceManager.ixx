export module ntmonkeys.com.Engine.Layer:TextureReferenceManager;

import ntmonkeys.com.Lib.Unique;
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
		int getIdOf(const Texture *const pTexture) const noexcept;

	private:
		Lib::IdAllocator<uint32_t> __idAllocator;
		std::unordered_map<const Texture *, std::pair<size_t, uint32_t>> __refIdMap;
	};

	int TextureReferenceManager::getIdOf(const Texture *const pTexture) const noexcept
	{
		if (!pTexture)
			return -1;

		return __refIdMap.at(pTexture).second;
	}

	void TextureReferenceManager::registerTexture(const Texture *const pTexture) noexcept
	{
		auto &[ref, id]{ __refIdMap[pTexture] };
		if (!ref)
			id = __idAllocator.allocate();

		++ref;
	}

	void TextureReferenceManager::unregisterTexture(const Texture *const pTexture) noexcept
	{
		auto &[ref, id] { __refIdMap[pTexture] };
		--ref;

		if (!ref)
		{
			__idAllocator.free(id);
			__refIdMap.erase(pTexture);
		}
	}
}
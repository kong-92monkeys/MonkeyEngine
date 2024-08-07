export module ntmonkeys.com.Engine.Material;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Stateful;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Engine.Texture;
import <memory>;
import <unordered_set>;
import <unordered_map>;
import <typeindex>;

namespace Engine
{
	export class Material : public Lib::Unique
	{
	public:
		[[nodiscard]]
		constexpr const std::unordered_map<uint32_t, const Texture *> &getTextures() const noexcept;

		[[nodiscard]]
		virtual const std::byte *getData() const noexcept = 0;

		[[nodiscard]]
		virtual size_t getSize() const noexcept = 0;

		[[nodiscard]]
		virtual uint32_t getTextureSlotCount() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const Material *> &getUpdateEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const Material *, uint32_t, const Texture *, const Texture *> &getTextureChangeEvent() const noexcept;

	protected:
		void _setTexture(const uint32_t slotIndex, const Texture *const pTexture) noexcept;
		void _invokeUpdateEvent() const noexcept;

	private:
		std::unordered_map<uint32_t, const Texture *> __textures;

		mutable Lib::Event<const Material *> __updateEvent;
		mutable Lib::Event<const Material *, uint32_t, const Texture *, const Texture *> __textureChangeEvent;
	};

	export template <typename $Data>
	class TypedMaterial : public Material
	{
	public:
		[[nodiscard]]
		virtual const std::byte *getData() const noexcept override final;

		[[nodiscard]]
		virtual size_t getSize() const noexcept override final;

	protected:
		[[nodiscard]]
		constexpr $Data &_getTypedData() noexcept;

		[[nodiscard]]
		constexpr const $Data &_getTypedData() const noexcept;

	private:
		$Data __data{ };
	};

	export class MaterialPack : public Lib::Unique
	{
	public:
		template <std::derived_from<Material> $M>
		bool hasMaterialOf() const noexcept;

		[[nodiscard]]
		bool hasMaterialOf(const std::type_index &type) const noexcept;

		template <std::derived_from<Material> $M>
		void setMaterial(const std::shared_ptr<$M> &pMaterial) noexcept;

		template <std::derived_from<Material> $M>
		void setMaterial(const std::shared_ptr<const $M> &pMaterial) noexcept;

		void setMaterial(const std::type_index &type, const std::shared_ptr<const Material> &pMaterial) noexcept;

		[[nodiscard]]
		std::unordered_set<const Material *>::const_iterator begin() const noexcept;

		[[nodiscard]]
		std::unordered_set<const Material *>::const_iterator end() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const MaterialPack *, std::type_index, const Material *, const Material *> &getMaterialChangeEvent() const noexcept;

	private:
		std::unordered_map<std::type_index, std::shared_ptr<const Material>> __materialMap;
		std::unordered_set<const Material *> __materials;

		mutable Lib::Event<const MaterialPack *, std::type_index, const Material *, const Material *> __materialChangeEvent;
	};

	constexpr const std::unordered_map<uint32_t, const Texture *> &Material::getTextures() const noexcept
	{
		return __textures;
	}

	constexpr Lib::EventView<const Material *> &Material::getUpdateEvent() const noexcept
	{
		return __updateEvent;
	}

	constexpr Lib::EventView<const Material *, uint32_t, const Texture *, const Texture *> &Material::getTextureChangeEvent() const noexcept
	{
		return __textureChangeEvent;
	}

	template <typename $Data>
	const std::byte *TypedMaterial<$Data>::getData() const noexcept
	{
		return reinterpret_cast<const std::byte *>(&__data);
	}

	template <typename $Data>
	size_t TypedMaterial<$Data>::getSize() const noexcept
	{
		return sizeof($Data);
	}

	template <typename $Data>
	constexpr $Data &TypedMaterial<$Data>::_getTypedData() noexcept
	{
		return __data;
	}

	template <typename $Data>
	constexpr const $Data &TypedMaterial<$Data>::_getTypedData() const noexcept
	{
		return __data;
	}

	template <std::derived_from<Material> $M>
	bool MaterialPack::hasMaterialOf() const noexcept
	{
		return hasMaterialOf(typeid($M));
	}

	template <std::derived_from<Material> $M>
	void MaterialPack::setMaterial(const std::shared_ptr<$M> &pMaterial) noexcept
	{
		setMaterial(typeid(*pMaterial), pMaterial);
	}

	template <std::derived_from<Material> $M>
	void MaterialPack::setMaterial(const std::shared_ptr<const $M> &pMaterial) noexcept
	{
		setMaterial(typeid(*pMaterial), pMaterial);
	}

	constexpr Lib::EventView<const MaterialPack *, std::type_index, const Material *, const Material *> &MaterialPack::getMaterialChangeEvent() const noexcept
	{
		return __materialChangeEvent;
	}
}

module: private;

namespace Engine
{
	uint32_t Material::getTextureSlotCount() const noexcept
	{
		return 0U;
	}

	void Material::_setTexture(const uint32_t slotIndex, const Texture *const pTexture) noexcept
	{
		auto &pPlaceholder{ __textures[slotIndex] };
		if (pPlaceholder == pTexture)
			return;

		const auto pPrevTexture{ pPlaceholder };
		pPlaceholder = pTexture;

		__textureChangeEvent.invoke(this, slotIndex, pPrevTexture, pTexture);
	}

	void Material::_invokeUpdateEvent() const noexcept
	{
		__updateEvent.invoke(this);
	}

	bool MaterialPack::hasMaterialOf(const std::type_index &type) const noexcept
	{
		const auto foundIt{ __materialMap.find(type) };
		if (foundIt == __materialMap.end())
			return false;

		return foundIt->second.get();
	}

	void MaterialPack::setMaterial(const std::type_index &type, const std::shared_ptr<const Material> &pMaterial) noexcept
	{
		auto &holder{ __materialMap[type] };
		if (holder == pMaterial)
			return;

		const Material *pPrevMaterial{ };

		if (holder)
		{
			pPrevMaterial = holder.get();
			__materials.erase(pPrevMaterial);
		}

		holder = pMaterial;
		if (holder)
			__materials.emplace(holder.get());

		__materialChangeEvent.invoke(this, type, pPrevMaterial, pMaterial.get());
	}

	std::unordered_set<const Material *>::const_iterator MaterialPack::begin() const noexcept
	{
		return __materials.begin();
	}

	std::unordered_set<const Material *>::const_iterator MaterialPack::end() const noexcept
	{
		return __materials.end();
	}
}
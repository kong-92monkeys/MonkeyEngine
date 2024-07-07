export module ntmonkeys.com.Engine.Material;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Event;
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
		virtual const std::byte *getData() const noexcept = 0;

		[[nodiscard]]
		virtual size_t getSize() const noexcept = 0;

		[[nodiscard]]
		constexpr Lib::EventView<const Material *> &getInvalidateEvent() const noexcept;

	protected:
		void _invokeInvalidateEvent() noexcept;

	private:
		mutable Lib::Event<const Material *> __invalidateEvent;
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
		virtual $Data &_getTypedData() noexcept = 0;
	};

	export class MaterialPack : public Lib::Unique
	{
	public:
		[[nodiscard]]
		constexpr const std::unordered_set<const Material *> &getMaterials() const noexcept;

		template <std::derived_from<Material> $M>
		void setMaterial(const std::shared_ptr<const $M> &pMaterial) noexcept;
		void setMaterial(const std::type_index &type, const std::shared_ptr<const Material> &pMaterial) noexcept;

	private:
		std::unordered_map<std::type_index, std::shared_ptr<const Material>> __materialMap;
		std::unordered_set<const Material *> __materials;

		Lib::Event<const MaterialPack *, std::type_index, const Material *, const Material *> __materialChangeEvent;
	};

	constexpr Lib::EventView<const Material *> &Material::getInvalidateEvent() const noexcept
	{
		return __invalidateEvent;
	}

	template <typename $Data>
	const std::byte *TypedMaterial<$Data>::getData() const noexcept override final
	{
		return reinterpret_cast<std::byte *>(&(const_cast<TypedMaterial<$Data> *>(this)->_getTypedData()));
	}

	template <typename $Data>
	size_t TypedMaterial<$Data>::getSize() const noexcept override final
	{
		return sizeof($Data);
	}

	constexpr const std::unordered_set<const Material *> &MaterialPack::getMaterials() const noexcept
	{
		return __materials;
	}

	template <std::derived_from<Material> $M>
	void MaterialPack::setMaterial(const std::shared_ptr<const $M> &pMaterial) noexcept
	{
		setMaterial(typeid($M), pMaterial);
	}
}

module: private;

namespace Engine
{
	void Material::_invokeInvalidateEvent() noexcept
	{
		__invalidateEvent.invoke(this);
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
}
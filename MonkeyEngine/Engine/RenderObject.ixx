export module ntmonkeys.com.Engine.RenderObject;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.Mesh;
import ntmonkeys.com.Engine.DrawParam;
import <vector>;
import <memory>;
import <unordered_set>;
import <unordered_map>;
import <typeindex>;

namespace Engine
{
	export class RenderObject : public Lib::Unique
	{
	public:
		RenderObject() noexcept;

		[[nodiscard]]
		const Renderer *getRenderer() const noexcept;
		void setRenderer(const std::shared_ptr<const Renderer> &pRenderer);

		[[nodiscard]]
		const Mesh *getMesh() const noexcept;
		void setMesh(const std::shared_ptr<const Mesh> &pMesh);

		[[nodiscard]]
		const DrawParam *getDrawParam() const noexcept;
		void setDrawParam(const std::shared_ptr<const DrawParam> &pDrawParam);

		[[nodiscard]]
		const MaterialPack *getMaterialPack(const uint32_t instanceIndex) const noexcept;
		void setMaterialPack(const uint32_t instanceIndex, const std::shared_ptr<const MaterialPack> &pMaterialPack);

		[[nodiscard]]
		uint32_t getInstanceCount() const noexcept;
		void setInstanceCount(const uint32_t count);

		[[nodiscard]]
		constexpr bool isVisible() const noexcept;
		void setVisible(const bool visible);

		[[nodiscard]]
		constexpr bool isDrawable() const noexcept;

		void draw(Graphics::CommandBuffer &commandBuffer) const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, const Renderer *, const Renderer *> &getRendererChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, const Mesh *, const Mesh *> &getMeshChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, const DrawParam *, const DrawParam *> &getDrawParamChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, uint32_t, const MaterialPack *, const MaterialPack *> &getMaterialPackChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, uint32_t, uint32_t> &getInstanceCountChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, bool, bool> &getDrawableChangeEvent() const noexcept;

	private:
		std::shared_ptr<const Renderer> __pRenderer;
		std::shared_ptr<const Mesh> __pMesh;
		std::shared_ptr<const DrawParam> __pDrawParam;
		std::vector<std::shared_ptr<const MaterialPack>> __materialPacks;

		bool __visible{ true };
		bool __drawable{ };

		std::unordered_map<const MaterialPack *, size_t> __materialPack2InstanceIndex;

		Lib::EventListenerPtr<const MaterialPack *, std::type_index, const Material *, const Material *> __pMaterialPackMaterialChangeListener;

		mutable Lib::Event<const RenderObject *, const Renderer *, const Renderer *> __rendererChangeEvent;
		mutable Lib::Event<const RenderObject *, const Mesh *, const Mesh *> __meshChangeEvent;
		mutable Lib::Event<const RenderObject *, const DrawParam *, const DrawParam *> __drawParamChangeEvent;
		mutable Lib::Event<const RenderObject *, uint32_t, const MaterialPack *, const MaterialPack *> __materialPackChangeEvent;
		mutable Lib::Event<const RenderObject *, uint32_t, uint32_t> __instanceCountChangeEvent;
		mutable Lib::Event<const RenderObject *, bool, bool> __drawableChangeEvent;

		[[nodiscard]]
		bool __resolveDrawable() const noexcept;
		void __validateDrawable();

		void __onMaterialPackMaterialChanged() noexcept;
	};

	constexpr bool RenderObject::isVisible() const noexcept
	{
		return __visible;
	}

	constexpr bool RenderObject::isDrawable() const noexcept
	{
		return __drawable;
	}

	constexpr Lib::EventView<const RenderObject *, const Renderer *, const Renderer *> &RenderObject::getRendererChangeEvent() const noexcept
	{
		return __rendererChangeEvent;
	}

	constexpr Lib::EventView<const RenderObject *, const Mesh *, const Mesh *> &RenderObject::getMeshChangeEvent() const noexcept
	{
		return __meshChangeEvent;
	}

	constexpr Lib::EventView<const RenderObject *, const DrawParam *, const DrawParam *> &RenderObject::getDrawParamChangeEvent() const noexcept
	{
		return __drawParamChangeEvent;
	}

	constexpr Lib::EventView<const RenderObject *, uint32_t, const MaterialPack *, const MaterialPack *> &RenderObject::getMaterialPackChangeEvent() const noexcept
	{
		return __materialPackChangeEvent;
	}

	constexpr Lib::EventView<const RenderObject *, uint32_t, uint32_t> &RenderObject::getInstanceCountChangeEvent() const noexcept
	{
		return __instanceCountChangeEvent;
	}

	constexpr Lib::EventView<const RenderObject *, bool, bool> &RenderObject::getDrawableChangeEvent() const noexcept
	{
		return __drawableChangeEvent;
	}
}

module: private;

namespace Engine
{
	RenderObject::RenderObject() noexcept
	{
		__pMaterialPackMaterialChangeListener = 
			Lib::EventListener<const MaterialPack *, std::type_index, const Material *, const Material *>::bind(
				&RenderObject::__onMaterialPackMaterialChanged, this);
	}

	const Renderer *RenderObject::getRenderer() const noexcept
	{
		return __pRenderer.get();
	}

	void RenderObject::setRenderer(const std::shared_ptr<const Renderer> &pRenderer)
	{
		if (__pRenderer == pRenderer)
			return;

		const auto pPrevRenderer{ __pRenderer.get() };
		__pRenderer = pRenderer;

		__rendererChangeEvent.invoke(this, pPrevRenderer, pRenderer.get());
		__validateDrawable();
	}

	const Mesh *RenderObject::getMesh() const noexcept
	{
		return __pMesh.get();
	}

	void RenderObject::setMesh(const std::shared_ptr<const Mesh> &pMesh)
	{
		if (__pMesh == pMesh)
			return;

		const auto pPrevMesh{ __pMesh.get() };
		__pMesh = pMesh;

		__meshChangeEvent.invoke(this, pPrevMesh, pMesh.get());
		__validateDrawable();
	}

	const DrawParam *RenderObject::getDrawParam() const noexcept
	{
		return __pDrawParam.get();
	}

	void RenderObject::setDrawParam(const std::shared_ptr<const DrawParam> &pDrawParam)
	{
		if (__pDrawParam == pDrawParam)
			return;

		const auto pPrevDrawParam{ __pDrawParam.get() };
		__pDrawParam = pDrawParam;

		__drawParamChangeEvent.invoke(this, pPrevDrawParam, pDrawParam.get());
		__validateDrawable();
	}

	const MaterialPack *RenderObject::getMaterialPack(const uint32_t instanceIndex) const noexcept
	{
		return __materialPacks[instanceIndex].get();
	}

	void RenderObject::setMaterialPack(const uint32_t instanceIndex, const std::shared_ptr<const MaterialPack> &pMaterialPack)
	{
		auto &holder{ __materialPacks[instanceIndex] };

		if (holder == pMaterialPack)
			return;

		const auto pPrevMaterialPack{ holder.get() };
		if (pPrevMaterialPack)
		{
			pPrevMaterialPack->getMaterialChangeEvent() -= __pMaterialPackMaterialChangeListener;
			__materialPack2InstanceIndex.erase(pPrevMaterialPack);
		}

		holder = pMaterialPack;
		if (pMaterialPack)
		{
			pMaterialPack->getMaterialChangeEvent() += __pMaterialPackMaterialChangeListener;
			__materialPack2InstanceIndex[pMaterialPack.get()] = instanceIndex;
		}

		__materialPackChangeEvent.invoke(this, instanceIndex, pPrevMaterialPack, pMaterialPack.get());
		__validateDrawable();
	}

	uint32_t RenderObject::getInstanceCount() const noexcept
	{
		return static_cast<uint32_t>(__materialPacks.size());
	}

	void RenderObject::setInstanceCount(const uint32_t count)
	{
		const uint32_t prevCount{ getInstanceCount() };
		if (prevCount == count)
			return;

		std::vector<std::pair<uint32_t, std::shared_ptr<const MaterialPack>>> expiredPacks;

		for (uint32_t instanceIndex{ prevCount - 1U }; instanceIndex >= count; --instanceIndex)
		{
			auto &pMaterialPack{ __materialPacks[instanceIndex] };

			pMaterialPack->getMaterialChangeEvent() -= __pMaterialPackMaterialChangeListener;
			__materialPack2InstanceIndex.erase(pMaterialPack.get());

			expiredPacks.emplace_back(instanceIndex, std::move(pMaterialPack));
		}

		__materialPacks.resize(count);

		for (const auto &[instanceIndex, expiredPack] : expiredPacks)
			__materialPackChangeEvent.invoke(this, instanceIndex, expiredPack.get(), nullptr);

		__instanceCountChangeEvent.invoke(this, prevCount, count);

		if (prevCount < count)
			__validateDrawable();
	}

	void RenderObject::setVisible(const bool visible)
	{
		if (__visible == visible)
			return;

		__visible = visible;
		__validateDrawable();
	}

	void RenderObject::draw(Graphics::CommandBuffer &commandBuffer) const noexcept
	{
		__pDrawParam->draw(commandBuffer, 1U, 0U);
	}

	bool RenderObject::__resolveDrawable() const noexcept
	{
		if (!__pRenderer || !__pMesh || !__pDrawParam || !__visible)
			return false;

		for (const auto &pMaterialPack : __materialPacks)
		{
			if (__pRenderer->isValidMaterialPack(pMaterialPack.get()))
				return false;
		}

		return true;
	}

	void RenderObject::__validateDrawable()
	{
		const bool curDrawable{ __resolveDrawable() };
		if (__drawable == curDrawable)
			return;

		__drawable = curDrawable;
		__drawableChangeEvent.invoke(this, !curDrawable, curDrawable);
	}

	void RenderObject::__onMaterialPackMaterialChanged() noexcept
	{
		__validateDrawable();
	}
}
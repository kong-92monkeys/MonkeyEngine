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
import <concepts>;

namespace Engine
{
	export class RenderObject : public Lib::Unique
	{
	public:
		[[nodiscard]]
		const Renderer *getRenderer() const noexcept;
		void setRenderer(const std::shared_ptr<const Renderer> &pRenderer) noexcept;

		[[nodiscard]]
		const Mesh *getMesh() const noexcept;
		void setMesh(const std::shared_ptr<const Mesh> &pMesh) noexcept;

		[[nodiscard]]
		const DrawParam *getDrawParam() const noexcept;
		void setDrawParam(const std::shared_ptr<const DrawParam> &pDrawParam) noexcept;

		[[nodiscard]]
		const MaterialPack *getMaterialPack(const uint32_t instanceIndex) const noexcept;
		void setMaterialPack(const uint32_t instanceIndex, const std::shared_ptr<const MaterialPack> &pMaterialPack) noexcept;

		[[nodiscard]]
		uint32_t getInstanceCount() const noexcept;
		void setInstanceCount(const uint32_t count) noexcept;

		void draw(Graphics::CommandBuffer &commandBuffer) const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, const Renderer *, const Renderer *> &getRendererChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, const Mesh *, const Mesh *> &getMeshChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, const DrawParam *, const DrawParam *> &getDrawParamChangeEvent() const noexcept;

		[[nodiscard]]
		constexpr Lib::EventView<const RenderObject *, uint32_t, const MaterialPack *, const MaterialPack *> &getMaterialPackChangeEvent() const noexcept;

	private:
		std::shared_ptr<const Renderer> __pRenderer;
		std::shared_ptr<const Mesh> __pMesh;
		std::shared_ptr<const DrawParam> __pDrawParam;
		std::vector<std::shared_ptr<const MaterialPack>> __materialPacks;

		mutable Lib::Event<const RenderObject *, const Renderer *, const Renderer *> __rendererChangeEvent;
		mutable Lib::Event<const RenderObject *, const Mesh *, const Mesh *> __meshChangeEvent;
		mutable Lib::Event<const RenderObject *, const DrawParam *, const DrawParam *> __drawParamChangeEvent;
		mutable Lib::Event<const RenderObject *, uint32_t, const MaterialPack *, const MaterialPack *> __materialPackChangeEvent;
		mutable Lib::Event<const RenderObject *, uint32_t, uint32_t> __instanceCountChangeEvent;
	};

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
}

module: private;

namespace Engine
{
	const Renderer *RenderObject::getRenderer() const noexcept
	{
		return __pRenderer.get();
	}

	void RenderObject::setRenderer(const std::shared_ptr<const Renderer> &pRenderer) noexcept
	{
		if (__pRenderer == pRenderer)
			return;

		const auto pPrevRenderer{ __pRenderer.get() };
		__pRenderer = pRenderer;

		__rendererChangeEvent.invoke(this, pPrevRenderer, pRenderer.get());
	}

	const Mesh *RenderObject::getMesh() const noexcept
	{
		return __pMesh.get();
	}

	void RenderObject::setMesh(const std::shared_ptr<const Mesh> &pMesh) noexcept
	{
		if (__pMesh == pMesh)
			return;

		const auto pPrevMesh{ __pMesh.get() };
		__pMesh = pMesh;

		__meshChangeEvent.invoke(this, pPrevMesh, pMesh.get());
	}

	const DrawParam *RenderObject::getDrawParam() const noexcept
	{
		return __pDrawParam.get();
	}

	void RenderObject::setDrawParam(const std::shared_ptr<const DrawParam> &pDrawParam) noexcept
	{
		if (__pDrawParam == pDrawParam)
			return;

		const auto pPrevDrawParam{ __pDrawParam.get() };
		__pDrawParam = pDrawParam;

		__drawParamChangeEvent.invoke(this, pPrevDrawParam, pDrawParam.get());
	}

	const MaterialPack *RenderObject::getMaterialPack(const uint32_t instanceIndex) const noexcept
	{
		return __materialPacks[instanceIndex].get();
	}

	void RenderObject::setMaterialPack(const uint32_t instanceIndex, const std::shared_ptr<const MaterialPack> &pMaterialPack) noexcept
	{
		auto &holder{ __materialPacks[instanceIndex] };

		if (holder == pMaterialPack)
			return;

		const auto pPrevMaterialPack{ holder.get() };
		holder = pMaterialPack;

		__materialPackChangeEvent.invoke(this, instanceIndex, pPrevMaterialPack, pMaterialPack.get());
	}

	uint32_t RenderObject::getInstanceCount() const noexcept
	{
		return static_cast<uint32_t>(__materialPacks.size());
	}

	void RenderObject::setInstanceCount(const uint32_t count) noexcept
	{
		const uint32_t prevCount{ getInstanceCount() };
		if (prevCount == count)
			return;

		std::vector<std::pair<uint32_t, std::shared_ptr<const MaterialPack>>> expiredPacks;

		for (uint32_t instanceIndex{ prevCount - 1U }; instanceIndex >= count; --instanceIndex)
			expiredPacks.emplace_back(instanceIndex, std::move(__materialPacks[instanceIndex]));

		__materialPacks.resize(count);

		for (const auto &[instanceIndex, expiredPack] : expiredPacks)
			__materialPackChangeEvent.invoke(this, instanceIndex, expiredPack.get(), nullptr);

		__instanceCountChangeEvent.invoke(this, prevCount, count);
	}

	void RenderObject::draw(Graphics::CommandBuffer &commandBuffer) const noexcept
	{
		__pDrawParam->draw(commandBuffer, 1U, 0U);
	}
}
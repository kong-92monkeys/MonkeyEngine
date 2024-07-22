module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.DrawParam;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.CommandBuffer;

namespace Engine
{
	export class DrawParam
	{
	public:
		virtual void draw(
			const Graphics::CommandBuffer &commandBuffer,
			const uint32_t instanceCount, const uint32_t firstInstance) const noexcept = 0;
	};

	export class DrawParamIndexed : public DrawParam
	{
	public:
		DrawParamIndexed(const uint32_t indexCount, const uint32_t firstIndex, const int32_t vertexOffset) noexcept;

		virtual void draw(
			const Graphics::CommandBuffer &commandBuffer,
			const uint32_t instanceCount, const uint32_t firstInstance) const noexcept override;

	private:
		const uint32_t __indexCount;
		const uint32_t __firstIndex;
		const int32_t __vertexOffset;
	};
}

module: private;

namespace Engine
{
	DrawParamIndexed::DrawParamIndexed(
		const uint32_t indexCount, const uint32_t firstIndex, const int32_t vertexOffset) noexcept :
		__indexCount{ indexCount }, __firstIndex{ firstIndex }, __vertexOffset{ vertexOffset }
	{}

	void DrawParamIndexed::draw(
		const Graphics::CommandBuffer &commandBuffer,
		const uint32_t instanceCount, const uint32_t firstInstance) const noexcept
	{
		commandBuffer.drawIndexed(__indexCount, instanceCount, __firstIndex, __vertexOffset, firstInstance);
	}
}
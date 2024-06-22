module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.TriangleRenderer;

import ntmonkeys.com.Frameworks.FrameworkRenderer;
import ntmonkeys.com.Frameworks.RenderPassFactory;

namespace Frameworks
{
	export class TriangleRenderer : public FrameworkRenderer
	{
	public:
		TriangleRenderer() noexcept;
		virtual ~TriangleRenderer() noexcept override = default;

	protected:
		[[nodiscard]]
		virtual const Engine::Renderer::ShaderInfoMap &_getShaderInfoMap() const noexcept override;

		[[nodiscard]]
		virtual const Graphics::RenderPass &_getRenderPass() const noexcept override;

	private:
		Engine::Renderer::ShaderInfoMap __shaderInfoMap;

		void __populateShaderInfoMap() noexcept;
	};
}

module: private;

namespace Frameworks
{
	TriangleRenderer::TriangleRenderer() noexcept
	{
		__populateShaderInfoMap();
	}

	const Engine::Renderer::ShaderInfoMap &TriangleRenderer::_getShaderInfoMap() const noexcept
	{
		return __shaderInfoMap;
	}

	const Graphics::RenderPass &TriangleRenderer::_getRenderPass() const noexcept
	{
		return _getRenderPassFactory().getInstance(RenderPassType::COLOR);
	}

	void TriangleRenderer::__populateShaderInfoMap() noexcept
	{
		__shaderInfoMap[VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT] = "Shaders/triangle.vert";
		__shaderInfoMap[VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT] = "Shaders/triangle.frag";
	}
}
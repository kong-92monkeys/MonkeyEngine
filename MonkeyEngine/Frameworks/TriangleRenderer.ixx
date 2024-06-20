module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.TriangleRenderer;

import ntmonkeys.com.Engine.Renderer;

namespace Frameworks
{
	export class TriangleRenderer : public Engine::Renderer
	{
	public:
		TriangleRenderer() = default;
		virtual ~TriangleRenderer() noexcept override = default;

	protected:
		virtual void _onInit() override;

		[[nodiscard]]
		virtual const ShaderInfoMap &_getShaderInfoMap() const noexcept override;

	private:
		ShaderInfoMap __shaderInfoMap;

		void __populateShaderInfoMap() noexcept;
	};
}

module: private;

namespace Frameworks
{
	void TriangleRenderer::_onInit()
	{
		__populateShaderInfoMap();
	}

	const Engine::Renderer::ShaderInfoMap &TriangleRenderer::_getShaderInfoMap() const noexcept
	{
		return __shaderInfoMap;
	}

	void TriangleRenderer::__populateShaderInfoMap() noexcept
	{
		__shaderInfoMap[VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT] = "Shaders/triangle.vert";
		__shaderInfoMap[VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT] = "Shaders/triangle.frag";
	}
}
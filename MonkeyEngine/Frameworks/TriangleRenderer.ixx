module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.TriangleRenderer;

import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Frameworks.Renderer;
import <memory>;
import <array>;

namespace Frameworks
{
	export class TriangleRenderer : public Renderer
	{
	public:
		virtual ~TriangleRenderer() noexcept override;

		[[nodiscard]]
		virtual RenderPassType getRenderPassType() const noexcept override;
		virtual void bind(Graphics::CommandBuffer &commandBuffer) noexcept override;

	protected:
		virtual void _onInit() override;

	private:
		std::unique_ptr<Graphics::DescriptorSetLayout> __pDescriptorSetLayout;
		std::unique_ptr<Graphics::PipelineLayout> __pPipelineLayout;
		std::unique_ptr<Graphics::Shader> __pVertexShader;
		std::unique_ptr<Graphics::Shader> __pFragmentShader;
		std::unique_ptr<Graphics::Pipeline> __pPipeline;

		void __createPipelineLayout();
		void __createShaders();
		void __createPipeline();
	};
}

module: private;

namespace Frameworks
{
	TriangleRenderer::~TriangleRenderer() noexcept
	{
		__pPipeline = nullptr;
		__pFragmentShader = nullptr;
		__pVertexShader = nullptr;
		__pPipelineLayout = nullptr;
	}

	RenderPassType TriangleRenderer::getRenderPassType() const noexcept
	{
		return RenderPassType::COLOR;
	}

	void TriangleRenderer::bind(Graphics::CommandBuffer &commandBuffer) noexcept
	{
		// TODO: bind pipeline
	}

	void TriangleRenderer::_onInit()
	{
		__createPipelineLayout();
		__createShaders();
		__createPipeline();
	}

	void TriangleRenderer::__createPipelineLayout()
	{
		__pPipelineLayout = _createPipelineLayout(0U, nullptr, 0U, nullptr);
	}

	void TriangleRenderer::__createShaders()
	{
		__pVertexShader = _createShader("Shaders/triangle.vert");
		__pFragmentShader = _createShader("Shaders/triangle.frag");
	}

	void TriangleRenderer::__createPipeline()
	{
		std::vector<VkPipelineShaderStageCreateInfo> stages;
		stages.resize(2ULL);

		auto &vertexShaderInfo{ stages[0] };
		vertexShaderInfo.sType	= VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderInfo.module = __pVertexShader->getHandle();
		vertexShaderInfo.pName = "main";

		auto &fragShaderInfo{ stages[1] };
		fragShaderInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderInfo.module = __pFragmentShader->getHandle();
		fragShaderInfo.pName = "main";

		const VkPipelineVertexInputStateCreateInfo vertexInputState
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO }
		};
		
		const VkPipelineInputAssemblyStateCreateInfo inputAssemblyState
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO },
			.topology				{ VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST },
			.primitiveRestartEnable	{ VK_FALSE }
		};

		const VkPipelineViewportStateCreateInfo viewportState
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO },
			.viewportCount		{ 1U },
			.scissorCount		{ 1U }
		};

		const VkPipelineRasterizationStateCreateInfo rasterizationState
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO },
			.depthClampEnable			{ VK_FALSE },
			.rasterizerDiscardEnable	{ VK_FALSE },
			.polygonMode				{ VkPolygonMode::VK_POLYGON_MODE_FILL },
			.cullMode					{ VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT },
			.frontFace					{ VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE },
			.depthBiasEnable			{ VK_FALSE },
			.lineWidth					{ 1.0f }
		};

		const VkPipelineMultisampleStateCreateInfo multisampleState
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO },
			.rasterizationSamples		{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.sampleShadingEnable		{ VK_FALSE },
			.minSampleShading			{ 1.0f },
			.pSampleMask				{ nullptr },
			.alphaToCoverageEnable		{ VK_FALSE },
			.alphaToOneEnable			{ VK_FALSE }
		};

		const VkPipelineColorBlendAttachmentState blendAttachment
		{
			.blendEnable			{ VK_FALSE },
			.colorWriteMask			{
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT |
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT |
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT |
				VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT
			}
		};

		const VkPipelineColorBlendStateCreateInfo colorBlendState
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO },
			.logicOpEnable		{ VK_FALSE },
			.attachmentCount	{ 1U },
			.pAttachments		{ &blendAttachment }
		};

		static constexpr std::array dynamicStateFlags
		{
			VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
			VkDynamicState::VK_DYNAMIC_STATE_SCISSOR
		};

		const VkPipelineDynamicStateCreateInfo dynamicState
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO },
			.dynamicStateCount	{ static_cast<uint32_t>(dynamicStateFlags.size()) },
			.pDynamicStates		{ dynamicStateFlags.data() }
		};

		const Graphics::LogicalDevice::GraphicsPipelineCreateInfo createInfo
		{
			.hPipelineLayout		{ __pPipelineLayout->getHandle() },
			.hRenderPass			{ _getRenderPass().getHandle() },
			.subpassIndex			{ 0U },
			.stageCount				{ static_cast<uint32_t>(stages.size()) },
			.pStages				{ stages.data() },
			.pVertexInputState		{ &vertexInputState },
			.pInputAssemblyState	{ &inputAssemblyState },
			.pTessellationState		{ nullptr },
			.pViewportState			{ &viewportState },
			.pRasterizationState	{ &rasterizationState },
			.pMultisampleState		{ &multisampleState },
			.pDepthStencilState		{ nullptr },
			.pColorBlendState		{ &colorBlendState },
			.pDynamicState			{ &dynamicState }
		};

		__pPipeline = _createPipeline(createInfo);
	}
}
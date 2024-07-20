module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.SimpleRenderer;

import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Engine.Constants;
import ntmonkeys.com.Engine.Renderer;
import ntmonkeys.com.Engine.Material;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Frameworks.Vertex;
import ntmonkeys.com.Frameworks.SimpleMaterial;
import <memory>;
import <array>;
import <vector>;

namespace Frameworks
{
	export class SimpleRenderer : public Engine::Renderer
	{
	public:
		virtual ~SimpleRenderer() noexcept override;

		[[nodiscard]]
		virtual bool isValidMaterialPack(const Engine::MaterialPack &materialPack) const noexcept override;

		[[nodiscard]]
		virtual std::optional<uint32_t> getDescriptorLocationOf(const std::type_index &materialType) const noexcept override;

		[[nodiscard]]
		virtual const Graphics::DescriptorSetLayout *getSubLayerDescSetLayout() const noexcept override;

		[[nodiscard]]
		virtual const Graphics::PipelineLayout &getPipelineLayout() const noexcept override;

		virtual void begin(Graphics::CommandBuffer &commandBuffer, const BeginInfo &beginInfo) const noexcept override;

	protected:
		virtual void _onInit() override;

	private:
		static constexpr uint32_t __SIMPLE_MATERIAL_DESC_LOCATION{ Engine::Constants::SUB_LAYER_MATERIAL_DESC_LOCATION0 };

		std::unique_ptr<Graphics::DescriptorSetLayout> __pSubLayerDescSetLayout;
		std::unique_ptr<Graphics::PipelineLayout> __pPipelineLayout;
		std::unique_ptr<Graphics::Shader> __pVertexShader;
		std::unique_ptr<Graphics::Shader> __pFragmentShader;
		std::unique_ptr<Graphics::Pipeline> __pPipeline;

		void __createSubLayerDescSetLayout();
		void __createPipelineLayout();
		void __createShaders();
		void __createPipeline();
	};
}

module: private;

namespace Frameworks
{
	SimpleRenderer::~SimpleRenderer() noexcept
	{
		__pPipeline = nullptr;
		__pFragmentShader = nullptr;
		__pVertexShader = nullptr;
		__pPipelineLayout = nullptr;
		__pSubLayerDescSetLayout = nullptr;
	}

	bool SimpleRenderer::isValidMaterialPack(const Engine::MaterialPack &materialPack) const noexcept
	{
		return materialPack.hasMaterialOf<SimpleMaterial>();
	}

	std::optional<uint32_t> SimpleRenderer::getDescriptorLocationOf(const std::type_index &materialType) const noexcept
	{
		if (materialType == typeid(SimpleMaterial))
			return __SIMPLE_MATERIAL_DESC_LOCATION;

		return std::nullopt;
	}

	const Graphics::DescriptorSetLayout *SimpleRenderer::getSubLayerDescSetLayout() const noexcept
	{
		return __pSubLayerDescSetLayout.get();
	}

	const Graphics::PipelineLayout &SimpleRenderer::getPipelineLayout() const noexcept
	{
		return *__pPipelineLayout;
	}

	void SimpleRenderer::begin(Graphics::CommandBuffer &commandBuffer, const BeginInfo &beginInfo) const noexcept
	{
		const auto &renderPass		{ _getRenderPass(Engine::RenderPassType::COLOR) };
		const auto &framebuffer		{ beginInfo.pFramebufferFactory->getInstance(Engine::RenderPassType::COLOR) };

		const VkRenderPassAttachmentBeginInfo attachmentInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO },
			.attachmentCount	{ 1U },
			.pAttachments		{ &(beginInfo.pSwapchainImageView->getHandle()) }
		};

		const VkRenderPassBeginInfo renderPassBeginInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
			.pNext			{ &attachmentInfo },
			.renderPass		{ renderPass.getHandle() },
			.framebuffer	{ framebuffer.getHandle() },
			.renderArea		{ *(beginInfo.pRenderArea) }
		};

		const VkSubpassBeginInfo subpassBeginInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO },
			.contents	{ VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE }
		};

		commandBuffer.beginRenderPass(renderPassBeginInfo, subpassBeginInfo);
		commandBuffer.bindPipeline(VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, __pPipeline->getHandle());
	}

	void SimpleRenderer::_onInit()
	{
		__createSubLayerDescSetLayout();
		__createPipelineLayout();
		__createShaders();
		__createPipeline();
	}

	void SimpleRenderer::__createSubLayerDescSetLayout()
	{
		std::vector<VkDescriptorSetLayoutBinding> bindings;

		auto &instanceInfoBinding				{ bindings.emplace_back() };
		instanceInfoBinding.binding				= Engine::Constants::SUB_LAYER_INSTANCE_INFO_LOCATION;
		instanceInfoBinding.descriptorType		= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		instanceInfoBinding.descriptorCount		= 1U;
		instanceInfoBinding.stageFlags			= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

		auto &materialBinding					{ bindings.emplace_back() };
		materialBinding.binding					= __SIMPLE_MATERIAL_DESC_LOCATION;
		materialBinding.descriptorType			= VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		materialBinding.descriptorCount			= 1U;
		materialBinding.stageFlags				= VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;

		__pSubLayerDescSetLayout = _createDescriptorSetLayout(0U, static_cast<uint32_t>(bindings.size()), bindings.data());
	}

	void SimpleRenderer::__createPipelineLayout()
	{
		std::unordered_map<uint32_t, VkDescriptorSetLayout> layoutMap;
		layoutMap[Engine::Constants::RENDER_TARGET_DESC_SET_LOCATION]	= _getRenderTargetDescSetLayout().getHandle();
		layoutMap[Engine::Constants::SUB_LAYER_DESC_SET_LOCATION]		= __pSubLayerDescSetLayout->getHandle();

		std::vector<VkDescriptorSetLayout> layouts;

		for (const auto &[setIndex, layout] : layoutMap)
		{
			if (setIndex >= layouts.size())
				layouts.resize(setIndex + 1U);

			layouts[setIndex] = layout;
		}

		__pPipelineLayout = _createPipelineLayout(static_cast<uint32_t>(layouts.size()), layouts.data(), 0U, nullptr);
	}

	void SimpleRenderer::__createShaders()
	{
		__pVertexShader = _createShader("Shaders/Simple.vert");
		__pFragmentShader = _createShader("Shaders/Simple.frag");
	}

	void SimpleRenderer::__createPipeline()
	{
		std::vector<VkPipelineShaderStageCreateInfo> stages;

		auto &vertexShaderInfo{ stages.emplace_back() };
		vertexShaderInfo.sType	= VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertexShaderInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
		vertexShaderInfo.module = __pVertexShader->getHandle();
		vertexShaderInfo.pName = "main";

		auto &fragShaderInfo{ stages.emplace_back() };
		fragShaderInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderInfo.stage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderInfo.module = __pFragmentShader->getHandle();
		fragShaderInfo.pName = "main";

		std::vector<VkVertexInputBindingDescription> vertexBindingDescs;

		auto &posBindingDesc		{ vertexBindingDescs.emplace_back() };
		posBindingDesc.binding		= VertexAttrib::POS_LOCATION;
		posBindingDesc.stride		= sizeof(Vertex_P);
		posBindingDesc.inputRate	= VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;

		auto &colorBindingDesc		{ vertexBindingDescs.emplace_back() };
		colorBindingDesc.binding	= VertexAttrib::COLOR_LOCATION;
		colorBindingDesc.stride		= sizeof(Vertex_C);
		colorBindingDesc.inputRate	= VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<VkVertexInputAttributeDescription> vertexAttribDescs;

		auto &posAttribDesc			{ vertexAttribDescs.emplace_back() };
		posAttribDesc.location		= VertexAttrib::POS_LOCATION;
		posAttribDesc.binding		= VertexAttrib::POS_LOCATION;
		posAttribDesc.format		= VkFormat::VK_FORMAT_R32G32B32_SFLOAT;
		posAttribDesc.offset		= 0U;

		auto &colorAttribDesc		{ vertexAttribDescs.emplace_back() };
		colorAttribDesc.location	= VertexAttrib::COLOR_LOCATION;
		colorAttribDesc.binding		= VertexAttrib::COLOR_LOCATION;
		colorAttribDesc.format		= VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;
		colorAttribDesc.offset		= 0U;

		const VkPipelineVertexInputStateCreateInfo vertexInputState
		{
			.sType								{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO },
			.vertexBindingDescriptionCount		{ static_cast<uint32_t>(vertexBindingDescs.size()) },
			.pVertexBindingDescriptions			{ vertexBindingDescs.data() },
			.vertexAttributeDescriptionCount	{ static_cast<uint32_t>(vertexAttribDescs.size()) },
			.pVertexAttributeDescriptions		{ vertexAttribDescs.data() }
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
			.hRenderPass			{ _getRenderPass(Engine::RenderPassType::COLOR).getHandle() },
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
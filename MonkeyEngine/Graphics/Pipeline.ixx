module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Pipeline;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class Pipeline : public Lib::Unique
	{
	public:
		struct GraphicsCreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkPipelineCache hPipelineCache{ };
			VkPipelineLayout hPipelineLayout{ };
			VkRenderPass hRenderPass{ };
			uint32_t subpassIndex{ };

			uint32_t stageCount{ };
			const VkPipelineShaderStageCreateInfo *pStages{ };
			const VkPipelineVertexInputStateCreateInfo *pVertexInputState{ };
			const VkPipelineInputAssemblyStateCreateInfo *pInputAssemblyState{ };
			const VkPipelineTessellationStateCreateInfo *pTessellationState{ };
			const VkPipelineViewportStateCreateInfo *pViewportState{ };
			const VkPipelineRasterizationStateCreateInfo *pRasterizationState{ };
			const VkPipelineMultisampleStateCreateInfo *pMultisampleState{ };
			const VkPipelineDepthStencilStateCreateInfo *pDepthStencilState{ };
			const VkPipelineColorBlendStateCreateInfo *pColorBlendState{ };
			const VkPipelineDynamicStateCreateInfo *pDynamicState{ };
		};

		explicit Pipeline(const GraphicsCreateInfo &createInfo) noexcept;
		virtual ~Pipeline() noexcept override;

		[[nodiscard]]
		constexpr const VkPipeline &getHandle() const noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkPipeline __handle{ };

		void __create(const GraphicsCreateInfo &createInfo);
	};

	constexpr const VkPipeline &Pipeline::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Pipeline::Pipeline(const GraphicsCreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	Pipeline::~Pipeline() noexcept
	{
		__deviceProc.vkDestroyPipeline(__hDevice, __handle, nullptr);
	}

	void Pipeline::__create(const GraphicsCreateInfo &createInfo)
	{
		const VkGraphicsPipelineCreateInfo vkCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO },
			.stageCount					{ createInfo.stageCount },
			.pStages					{ createInfo.pStages },
			.pVertexInputState			{ createInfo.pVertexInputState },
			.pInputAssemblyState		{ createInfo.pInputAssemblyState },
			.pTessellationState			{ createInfo.pTessellationState },
			.pViewportState				{ createInfo.pViewportState },
			.pRasterizationState		{ createInfo.pRasterizationState },
			.pMultisampleState			{ createInfo.pMultisampleState },
			.pDepthStencilState			{ createInfo.pDepthStencilState },
			.pColorBlendState			{ createInfo.pColorBlendState },
			.pDynamicState				{ createInfo.pDynamicState },
			.layout						{ createInfo.hPipelineLayout },
			.renderPass					{ createInfo.hRenderPass },
			.subpass					{ createInfo.subpassIndex },
			.basePipelineHandle			{ VK_NULL_HANDLE },
			.basePipelineIndex			{ -1 }
		};

		__deviceProc.vkCreateGraphicsPipelines(__hDevice, createInfo.hPipelineCache, 1U, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Pipeline." };
	}
}
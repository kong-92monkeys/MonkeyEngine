module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Pipeline;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
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
		};

		explicit Pipeline(const GraphicsCreateInfo &createInfo) noexcept;
		virtual ~Pipeline() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkPipeline __handle{ };

		void __createGraphics(const GraphicsCreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	Pipeline::Pipeline(const GraphicsCreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__createGraphics(createInfo);
	}

	Pipeline::~Pipeline() noexcept
	{
		__deviceProc.vkDestroyPipeline(__hDevice, __handle, nullptr);
	}

	void Pipeline::__createGraphics(const GraphicsCreateInfo &createInfo)
	{
		const VkGraphicsPipelineCreateInfo vkCreateInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO },
			.pNext					{},
			.flags					{},
			.stageCount				{},
			.pStages				{},
			.pVertexInputState		{},
			.pInputAssemblyState	{},
			.pTessellationState		{},
			.pViewportState			{},
			.pRasterizationState	{},
			.pMultisampleState		{},
			.pDepthStencilState		{},
			.pColorBlendState		{},
			.pDynamicState			{},
			.layout					{},
			.renderPass				{},
			.subpass				{},
			.basePipelineHandle		{},
			.basePipelineIndex		{}
		};

		__deviceProc.vkCreateGraphicsPipelines(__hDevice, createInfo.hPipelineCache, 1U, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a graphics pipeline." };
	}
}
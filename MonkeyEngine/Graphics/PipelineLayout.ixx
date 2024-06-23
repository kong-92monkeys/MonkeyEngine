module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.PipelineLayout;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class PipelineLayout : public Lib::Unique
	{
	public:
		friend class LogicalDevice;

		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			uint32_t setLayoutCount{ };
			const VkDescriptorSetLayout *pSetLayouts{ };
			uint32_t pushConstantRangeCount{ };
			const VkPushConstantRange *pPushConstantRanges{ };
		};

		explicit PipelineLayout(const CreateInfo &createInfo) noexcept;
		virtual ~PipelineLayout() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkPipelineLayout __handle{ };

		void __create(const CreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	PipelineLayout::PipelineLayout(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	PipelineLayout::~PipelineLayout() noexcept
	{
		__deviceProc.vkDestroyPipelineLayout(__hDevice, __handle, nullptr);
	}

	void PipelineLayout::__create(const CreateInfo &createInfo)
	{
		const VkPipelineLayoutCreateInfo vkCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO },
			.setLayoutCount				{ createInfo.setLayoutCount },
			.pSetLayouts				{ createInfo.pSetLayouts },
			.pushConstantRangeCount		{ createInfo.pushConstantRangeCount },
			.pPushConstantRanges		{ createInfo.pPushConstantRanges }
		};

		__deviceProc.vkCreatePipelineLayout(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a PipelineLayout." };
	}
}
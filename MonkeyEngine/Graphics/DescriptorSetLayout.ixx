module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.DescriptorSetLayout;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class DescriptorSetLayout : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			uint32_t bindingCount{ };
			const VkDescriptorSetLayoutBinding *pBindings{ };
		};

		explicit DescriptorSetLayout(const CreateInfo &createInfo) noexcept;
		virtual ~DescriptorSetLayout() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkDescriptorSetLayout __handle{ };

		void __create(const CreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	DescriptorSetLayout::DescriptorSetLayout(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	DescriptorSetLayout::~DescriptorSetLayout() noexcept
	{
		__deviceProc.vkDestroyDescriptorSetLayout(__hDevice, __handle, nullptr);
	}

	void DescriptorSetLayout::__create(const CreateInfo &createInfo)
	{
		const VkDescriptorSetLayoutCreateInfo vkCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.bindingCount	{ createInfo.bindingCount },
			.pBindings		{ createInfo.pBindings }
		};

		__deviceProc.vkCreateDescriptorSetLayout(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a DescriptorSetLayout." };
	}
}
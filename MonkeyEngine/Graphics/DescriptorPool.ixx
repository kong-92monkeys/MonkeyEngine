module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.DescriptorPool;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.DescriptorSet;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class DescriptorPool : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkDescriptorPoolCreateFlags flags{ };
			uint32_t maxSets{ };
			uint32_t poolSizeCount{ };
			const VkDescriptorPoolSize *pPoolSizes{ };
		};

		explicit DescriptorPool(const CreateInfo &createInfo) noexcept;
		virtual ~DescriptorPool() noexcept override;

		[[nodiscard]]
		DescriptorSet *allocateDescriptorSet(const VkDescriptorSetLayout hLayout);
		VkResult reset() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		const VkDescriptorPoolCreateFlags __flags{ };
		VkDescriptorPool __handle{ };

		void __create(const CreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	DescriptorPool::DescriptorPool(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice },
		__flags			{ createInfo.flags }
	{
		__create(createInfo);
	}

	DescriptorPool::~DescriptorPool() noexcept
	{
		__deviceProc.vkDestroyDescriptorPool(__hDevice, __handle, nullptr);
	}

	DescriptorSet *DescriptorPool::allocateDescriptorSet(const VkDescriptorSetLayout hLayout)
	{
		const DescriptorSet::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __hDevice },
			.hDescriptorPool	{ __handle },
			.needFree			{ static_cast<bool>(__flags & VkDescriptorPoolCreateFlagBits::VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT) },
			.hLayout			{ hLayout }
		};

		return new DescriptorSet{ createInfo };
	}

	VkResult DescriptorPool::reset() noexcept
	{
		return __deviceProc.vkResetDescriptorPool(__hDevice, __handle, 0U);
	}

	void DescriptorPool::__create(const CreateInfo &createInfo)
	{
		const VkDescriptorPoolCreateInfo vkCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO },
			.flags			{ createInfo.flags },
			.maxSets		{ createInfo.maxSets },
			.poolSizeCount	{ createInfo.poolSizeCount },
			.pPoolSizes		{ createInfo.pPoolSizes }
		};

		__deviceProc.vkCreateDescriptorPool(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a DescriptorPool." };
	}
}
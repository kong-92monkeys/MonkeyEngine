module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.DescriptorSet;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class DescriptorSet : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkDescriptorPool hDescriptorPool{ };
			bool needFree{ };
			VkDescriptorSetLayout hLayout{ };
		};

		explicit DescriptorSet(const CreateInfo &createInfo) noexcept;
		virtual ~DescriptorSet() noexcept override;

		void update(
			const uint32_t descriptorWriteCount,
			const VkWriteDescriptorSet *const pDescriptorWrites,
			const uint32_t descriptorCopyCount,
			const VkCopyDescriptorSet *const pDescriptorCopies);

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const VkDescriptorPool __hDescriptorPool;

		const bool __needFree;
		VkDescriptorSet __handle{ };

		void __create(const CreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	DescriptorSet::DescriptorSet(const CreateInfo &createInfo) noexcept :
		__deviceProc		{ *(createInfo.pDeviceProc) },
		__hDevice			{ createInfo.hDevice },
		__hDescriptorPool	{ createInfo.hDescriptorPool },
		__needFree			{ createInfo.needFree }
	{
		__create(createInfo);
	}

	DescriptorSet::~DescriptorSet() noexcept
	{
		if (!__needFree)
			return;

		__deviceProc.vkFreeDescriptorSets(__hDevice, __hDescriptorPool, 1U, &__handle);
	}

	void DescriptorSet::update(
		const uint32_t descriptorWriteCount,
		const VkWriteDescriptorSet *const pDescriptorWrites,
		const uint32_t descriptorCopyCount,
		const VkCopyDescriptorSet *const pDescriptorCopies)
	{
		__deviceProc.vkUpdateDescriptorSets(
			__hDevice, descriptorWriteCount, pDescriptorWrites, descriptorCopyCount, pDescriptorCopies);
	}

	void DescriptorSet::__create(const CreateInfo &createInfo)
	{
		const VkDescriptorSetAllocateInfo vkCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.descriptorPool		{ createInfo.hDescriptorPool },
			.descriptorSetCount	{ 1U },
			.pSetLayouts		{ &(createInfo.hLayout) }
		};

		__deviceProc.vkAllocateDescriptorSets(__hDevice, &vkCreateInfo, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a DescriptorSet." };
	}
}
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
		class CreationException : public std::exception
		{
		public:
			CreationException(const VkResult result) noexcept;

		private:
			const VkResult __result;
		};

		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkDescriptorPool hDescriptorPool{ };
			bool needFree{ };
			VkDescriptorSetLayout hLayout{ };
			const uint32_t *pVariableDescriptorCount{ };
		};

		explicit DescriptorSet(const CreateInfo &createInfo);
		virtual ~DescriptorSet() noexcept override;

		[[nodiscard]]
		constexpr const VkDescriptorSet &getHandle() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const VkDescriptorPool __hDescriptorPool;

		const bool __needFree;
		VkDescriptorSet __handle{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr const VkDescriptorSet &DescriptorSet::getHandle() noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	DescriptorSet::DescriptorSet(const CreateInfo &createInfo) :
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

	void DescriptorSet::__create(const CreateInfo &createInfo)
	{
		const auto pVariableDescCount{ createInfo.pVariableDescriptorCount };

		const VkDescriptorSetVariableDescriptorCountAllocateInfo descCountAllocInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_VARIABLE_DESCRIPTOR_COUNT_ALLOCATE_INFO },
			.descriptorSetCount	{ 1U },
			.pDescriptorCounts	{ pVariableDescCount }
		};

		const VkDescriptorSetAllocateInfo vkCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO },
			.pNext				{ pVariableDescCount ? &descCountAllocInfo : nullptr },
			.descriptorPool		{ createInfo.hDescriptorPool },
			.descriptorSetCount	{ 1U },
			.pSetLayouts		{ &(createInfo.hLayout) }
		};

		const VkResult result{ __deviceProc.vkAllocateDescriptorSets(__hDevice, &vkCreateInfo, &__handle) };
		if (result != VkResult::VK_SUCCESS)
			throw CreationException{ result };
	}

	DescriptorSet::CreationException::CreationException(const VkResult result) noexcept :
		__result{ result }
	{}
}
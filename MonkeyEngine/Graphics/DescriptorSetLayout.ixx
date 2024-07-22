module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.DescriptorSetLayout;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;
import <unordered_map>;

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
			VkDescriptorSetLayoutCreateFlags flags{ };
			uint32_t bindingCount{ };
			const VkDescriptorBindingFlags *pBindingFlags{ };
			const VkDescriptorSetLayoutBinding *pBindings{ };
		};

		explicit DescriptorSetLayout(const CreateInfo &createInfo) noexcept;
		virtual ~DescriptorSetLayout() noexcept override;

		[[nodiscard]]
		constexpr const std::unordered_map<VkDescriptorType, uint32_t> &getDescriptorCountInfo() const noexcept;

		[[nodiscard]]
		constexpr const VkDescriptorSetLayout &getHandle() const noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkDescriptorSetLayout __handle{ };
		std::unordered_map<VkDescriptorType, uint32_t> __descriptorCountInfo;

		void __create(const CreateInfo &createInfo);
	};

	constexpr const std::unordered_map<VkDescriptorType, uint32_t> &DescriptorSetLayout::getDescriptorCountInfo() const noexcept
	{
		return __descriptorCountInfo;
	}

	constexpr const VkDescriptorSetLayout &DescriptorSetLayout::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	DescriptorSetLayout::DescriptorSetLayout(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);

		for (uint32_t bindingIter{ }; bindingIter < createInfo.bindingCount; ++bindingIter)
		{
			const auto &binding{ createInfo.pBindings[bindingIter] };
			__descriptorCountInfo[binding.descriptorType] += binding.descriptorCount;
		}
	}

	DescriptorSetLayout::~DescriptorSetLayout() noexcept
	{
		__deviceProc.vkDestroyDescriptorSetLayout(__hDevice, __handle, nullptr);
	}

	void DescriptorSetLayout::__create(const CreateInfo &createInfo)
	{
		const VkDescriptorSetLayoutBindingFlagsCreateInfo flagInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO },
			.bindingCount	{ createInfo.bindingCount },
			.pBindingFlags	{ createInfo.pBindingFlags }
		};

		const VkDescriptorSetLayoutCreateInfo vkCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO },
			.pNext			{ &flagInfo },
			.flags			{ createInfo.flags },
			.bindingCount	{ createInfo.bindingCount },
			.pBindings		{ createInfo.pBindings }
		};

		__deviceProc.vkCreateDescriptorSetLayout(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a DescriptorSetLayout." };
	}
}
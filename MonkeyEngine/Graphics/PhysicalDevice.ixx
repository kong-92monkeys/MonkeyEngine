module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.PhysicalDevice;

import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Surface;
import <vector>;
import <unordered_map>;
import <string_view>;
import <memory>;
import <stdexcept>;
import <array>;
import <optional>;

namespace Graphics
{
	export struct QueueFamilyInfo
	{
	public:
		const VkQueueFamilyProperties *pProps{ };
		const VkQueueFamilyGlobalPriorityPropertiesKHR *pGlobalPriorityProps{ };
	};

	export class PhysicalDevice
	{
	public:
		struct MakeInfo
		{
		public:
			const VK::InstanceProc *pInstanceProc{ };
			VkInstance hVulkanInstance{ };
			VkPhysicalDevice hPhysicalDevice{ };
		};

		explicit PhysicalDevice(const MakeInfo &makeInfo) noexcept;

		[[nodiscard]]
		std::unique_ptr<LogicalDevice> createLogicalDevice() const;

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;

		VkPhysicalDeviceProperties2 __props{ };
		VkPhysicalDeviceVulkan11Properties __11Props{ };
		VkPhysicalDeviceVulkan12Properties __12Props{ };
		VkPhysicalDeviceVulkan13Properties __13Props{ };
		VkPhysicalDeviceRobustness2PropertiesEXT __robustness2Props{ };
		VkPhysicalDeviceDescriptorBufferPropertiesEXT __descriptorBufferProps{ };

		VkPhysicalDeviceFeatures2 __features{ };
		VkPhysicalDeviceVulkan11Features __11Features{ };
		VkPhysicalDeviceVulkan12Features __12Features{ };
		VkPhysicalDeviceVulkan13Features __13Features{ };
		VkPhysicalDeviceRobustness2FeaturesEXT __robustness2Features{ };
		VkPhysicalDeviceDescriptorBufferFeaturesEXT __descriptorBufferFeatures{ };

		std::vector<VkExtensionProperties> __extensions;
		std::unordered_map<std::string_view, const VkExtensionProperties *> __extensionMap;
		std::unordered_map<VkFormat, VkFormatProperties3> __formatPropMap;

		std::vector<VkQueueFamilyProperties2> __queueFamilyProps;
		std::vector<VkQueueFamilyGlobalPriorityPropertiesKHR> __queueFamilyGlobalPriorityProps;
		std::vector<QueueFamilyInfo> __queueFamilyInfos;

		std::optional<uint32_t> __queueFamilyIndex;

		void __resolveProps() noexcept;
		void __resolveFeatures() noexcept;
		void __resolveExtensions() noexcept;
		void __resolveFormatProps() noexcept;
		void __resolveQueueFamilyInfos() noexcept;
		void __resolveQueueFamilyIndex() noexcept;
	};
}

module: private;

namespace Graphics
{
	PhysicalDevice::PhysicalDevice(const MakeInfo &makeInfo) noexcept :
		__instanceProc		{ *(makeInfo.pInstanceProc) },
		__hInstance			{ makeInfo.hVulkanInstance },
		__hPhysicalDevice	{ makeInfo.hPhysicalDevice }
	{
		__resolveProps();
		__resolveFeatures();
		__resolveExtensions();
		__resolveFormatProps();
		__resolveQueueFamilyInfos();
		__resolveQueueFamilyIndex();
	}

	std::unique_ptr<LogicalDevice> PhysicalDevice::createLogicalDevice() const
	{
		const LogicalDevice::CreateInfo createInfo
		{
			.pInstanceProc				{ &__instanceProc },
			.hInstance					{ __hInstance },
			.hPhysicalDevice			{ __hPhysicalDevice },
			.queueFamilyIndex			{ __queueFamilyIndex.value() },

			.p10Props					{ &(__props.properties) },
			.p11Props					{ &__11Props },
			.p12Props					{ &__12Props },
			.p13Props					{ &__13Props },
			.pRobustness2Props			{ &__robustness2Props },
			.pDescriptorBufferProps		{ &__descriptorBufferProps },

			.p10Features				{ &(__features.features) },
			.p11Features				{ &__11Features },
			.p12Features				{ &__12Features },
			.p13Features				{ &__13Features },
			.pRobustness2Features		{ &__robustness2Features },
			.pDescriptorBufferFeatures	{ &__descriptorBufferFeatures },

			.pExtensionMap				{ &__extensionMap }
		};

		return std::make_unique<LogicalDevice>(createInfo);
	}

	void PhysicalDevice::__resolveProps() noexcept
	{
		__props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		__props.pNext = &__11Props;

		__11Props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
		__11Props.pNext = &__12Props;

		__12Props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
		__12Props.pNext = &__13Props;

		__13Props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
		__13Props.pNext = &__robustness2Props;

		__robustness2Props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;
		__robustness2Props.pNext = &__descriptorBufferProps;

		__descriptorBufferProps.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_PROPERTIES_EXT;
		__descriptorBufferProps.pNext = nullptr;

		__instanceProc.vkGetPhysicalDeviceProperties2(__hPhysicalDevice, &__props);
	}

	void PhysicalDevice::__resolveFeatures() noexcept
	{
		__features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		__features.pNext = &__11Features;

		__11Features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		__11Features.pNext = &__12Features;

		__12Features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		__12Features.pNext = &__13Features;

		__13Features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		__13Features.pNext = &__robustness2Features;

		__robustness2Features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		__robustness2Features.pNext = &__descriptorBufferFeatures;

		__descriptorBufferFeatures.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
		__descriptorBufferFeatures.pNext = nullptr;

		__instanceProc.vkGetPhysicalDeviceFeatures2(__hPhysicalDevice, &__features);
	}

	void PhysicalDevice::__resolveExtensions() noexcept
	{
		uint32_t extensionCount{ };
		__instanceProc.vkEnumerateDeviceExtensionProperties(__hPhysicalDevice, nullptr, &extensionCount, nullptr);

		__extensions.resize(extensionCount);
		__instanceProc.vkEnumerateDeviceExtensionProperties(__hPhysicalDevice, nullptr, &extensionCount, __extensions.data());

		for (const auto &extension : __extensions)
			__extensionMap[extension.extensionName] = &extension;
	}

	void PhysicalDevice::__resolveFormatProps() noexcept
	{
		static constexpr std::array formats
		{
			VkFormat::VK_FORMAT_D24_UNORM_S8_UINT,
			VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT
		};

		for (const auto format : formats)
		{
			VkFormatProperties3 prop3
			{
				.sType	{ VkStructureType::VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_3 },
			};

			VkFormatProperties2 props
			{
				.sType	{ VkStructureType::VK_STRUCTURE_TYPE_FORMAT_PROPERTIES_2 },
				.pNext	{ &prop3 }
			};

			__instanceProc.vkGetPhysicalDeviceFormatProperties2(__hPhysicalDevice, format, &props);
			__formatPropMap[format] = prop3;
		}
	}

	void PhysicalDevice::__resolveQueueFamilyInfos() noexcept
	{
		uint32_t familyCount{ };
		__instanceProc.vkGetPhysicalDeviceQueueFamilyProperties2(__hPhysicalDevice, &familyCount, nullptr);

		__queueFamilyProps.resize(familyCount);
		__queueFamilyGlobalPriorityProps.resize(familyCount);
		__queueFamilyInfos.resize(familyCount);

		for (uint32_t iter{ }; iter < familyCount; ++iter)
		{
			auto &props					{ __queueFamilyProps[iter] };
			auto &globalPriorityProps	{ __queueFamilyGlobalPriorityProps[iter] };
			auto &infos					{ __queueFamilyInfos[iter] };

			props.sType = VkStructureType::VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			props.pNext = &globalPriorityProps;

			globalPriorityProps.sType = VkStructureType::VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_KHR;
			globalPriorityProps.pNext = nullptr;

			infos.pProps = &(props.queueFamilyProperties);
			infos.pGlobalPriorityProps = &globalPriorityProps;
		}

		__instanceProc.vkGetPhysicalDeviceQueueFamilyProperties2(__hPhysicalDevice, &familyCount, __queueFamilyProps.data());
	}

	void PhysicalDevice::__resolveQueueFamilyIndex() noexcept
	{
		const uint32_t familyCount{ static_cast<uint32_t>(__queueFamilyInfos.size()) };
		for (uint32_t iter{ }; iter < familyCount; ++iter)
		{
			const auto &familyInfo{ __queueFamilyInfos[iter] };

			const bool hasGraphicsFlag{ static_cast<bool>(familyInfo.pProps->queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) };
			if (!hasGraphicsFlag)
				continue;

			__queueFamilyIndex = iter;
			break;
		}
	}
}
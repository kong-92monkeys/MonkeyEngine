module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.DeviceInfo;

import <vector>;
import <unordered_map>;
import <string_view>;
import ntmonkeys.com.VK.VulkanProc;

namespace Graphics
{
	export class DeviceInfo
	{
	public:
		DeviceInfo(const VK::InstanceProc &proc, const VkPhysicalDevice hPhysicalDevice) noexcept;

	private:
		const VK::InstanceProc &__proc;
		const VkPhysicalDevice __hPhysicalDevice;

		VkPhysicalDeviceProperties2 __props{ };
		VkPhysicalDeviceVulkan11Properties __11props{ };
		VkPhysicalDeviceVulkan12Properties __12props{ };
		VkPhysicalDeviceVulkan13Properties __13props{ };
		VkPhysicalDeviceRobustness2PropertiesEXT __robustness2Props{ };

		VkPhysicalDeviceFeatures2 __features{ };
		VkPhysicalDeviceVulkan11Features __11features{ };
		VkPhysicalDeviceVulkan12Features __12features{ };
		VkPhysicalDeviceVulkan13Features __13features{ };
		VkPhysicalDeviceRobustness2FeaturesEXT __robustness2Features{ };

		std::vector<VkExtensionProperties> __extensions;
		std::unordered_map<std::string_view, const VkExtensionProperties *> __extensionMap;

		std::vector<VkQueueFamilyProperties2> __queueFamilyProps;
		std::vector<VkQueueFamilyGlobalPriorityPropertiesKHR> __queueFamilyGlobalPriorityProps;

		void __resolveProps() noexcept;
		void __resolveFeatures() noexcept;
		void __resolveExtensions() noexcept;
		void __resolveQueueFamilies() noexcept;
	};
}

module: private;

namespace Graphics
{
	DeviceInfo::DeviceInfo(const VK::InstanceProc &proc, const VkPhysicalDevice hPhysicalDevice) noexcept :
		__proc{ proc }, __hPhysicalDevice{ hPhysicalDevice }
	{
		__resolveProps();
		__resolveFeatures();
		__resolveExtensions();
		__resolveQueueFamilies();
	}

	void DeviceInfo::__resolveProps() noexcept
	{
		__props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		__props.pNext = &__11props;

		__11props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
		__11props.pNext = &__12props;

		__12props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
		__12props.pNext = &__13props;

		__13props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
		__13props.pNext = &__robustness2Props;

		__robustness2Props.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_PROPERTIES_EXT;
		__robustness2Props.pNext = nullptr;

		__proc.vkGetPhysicalDeviceProperties2(__hPhysicalDevice, &__props);
	}

	void DeviceInfo::__resolveFeatures() noexcept
	{
		__features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		__features.pNext = &__11features;

		__11features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		__11features.pNext = &__12features;

		__12features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		__12features.pNext = &__13features;

		__13features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		__13features.pNext = &__robustness2Features;

		__robustness2Features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		__robustness2Features.pNext = nullptr;

		__proc.vkGetPhysicalDeviceFeatures2(__hPhysicalDevice, &__features);
	}

	void DeviceInfo::__resolveExtensions() noexcept
	{
		uint32_t extensionCount{ };
		__proc.vkEnumerateDeviceExtensionProperties(__hPhysicalDevice, nullptr, &extensionCount, nullptr);

		__extensions.resize(extensionCount);
		__proc.vkEnumerateDeviceExtensionProperties(__hPhysicalDevice, nullptr, &extensionCount, __extensions.data());

		for (const auto &extension : __extensions)
			__extensionMap[extension.extensionName] = &extension;
	}

	void DeviceInfo::__resolveQueueFamilies() noexcept
	{
		uint32_t familyCount{ };
		__proc.vkGetPhysicalDeviceQueueFamilyProperties2(__hPhysicalDevice, &familyCount, nullptr);

		__queueFamilyProps.resize(familyCount);
		__queueFamilyGlobalPriorityProps.resize(familyCount);

		for (uint32_t iter{ }; iter < familyCount; ++iter)
		{
			auto &props{ __queueFamilyProps[iter] };
			auto &globalPriorityProps{ __queueFamilyGlobalPriorityProps[iter] };

			props.sType = VkStructureType::VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			props.pNext = &globalPriorityProps;

			globalPriorityProps.sType = VkStructureType::VK_STRUCTURE_TYPE_QUEUE_FAMILY_GLOBAL_PRIORITY_PROPERTIES_KHR;
			globalPriorityProps.pNext = nullptr;
		}

		__proc.vkGetPhysicalDeviceQueueFamilyProperties2(__hPhysicalDevice, &familyCount, __queueFamilyProps.data());
	}
}
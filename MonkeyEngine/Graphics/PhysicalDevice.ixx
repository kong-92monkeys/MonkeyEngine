module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.PhysicalDevice;

import <vector>;
import <unordered_map>;
import <string_view>;
import <memory>;
import <stdexcept>;
import <array>;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Surface;

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
			VkInstance hInstance{ };
			VkPhysicalDevice hPhysicalDevice{ };
		};

		explicit PhysicalDevice(const MakeInfo &makeInfo) noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceProperties &get10Props() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceVulkan11Properties &get11Props() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceVulkan12Properties &get12Props() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceVulkan13Properties &get13Props() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceRobustness2PropertiesEXT &getRobustness2Props() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceFeatures &get10Features() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceVulkan11Features &get11Features() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceVulkan12Features &get12Features() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceVulkan13Features &get13Features() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceRobustness2FeaturesEXT &getRobustness2Features() const noexcept;

		[[nodiscard]]
		constexpr const std::unordered_map<std::string_view, const VkExtensionProperties *> &getExtensionMap() const noexcept;

		[[nodiscard]]
		constexpr const std::unordered_map<VkFormat, VkFormatProperties3> &getFormatPropertiesMap() const noexcept;

		[[nodiscard]]
		constexpr const std::vector<QueueFamilyInfo> &getQueueFamilyInfos() const noexcept;

		[[nodiscard]]
		bool isWin32PresentSupported(const uint32_t queueFamilyIndex) const noexcept;

		[[nodiscard]]
		std::unique_ptr<LogicalDevice> createLogicalDevice(
			const uint32_t queueFamilyIndex,
			const VkPhysicalDeviceFeatures2 &features,
			const std::vector<const char *> &extensions) const;

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
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
		std::unordered_map<VkFormat, VkFormatProperties3> __formatPropMap;

		std::vector<VkQueueFamilyProperties2> __queueFamilyProps;
		std::vector<VkQueueFamilyGlobalPriorityPropertiesKHR> __queueFamilyGlobalPriorityProps;
		std::vector<QueueFamilyInfo> __queueFamilyInfos;

		void __resolveProps() noexcept;
		void __resolveFeatures() noexcept;
		void __resolveExtensions() noexcept;
		void __resolveFormatProps() noexcept;
		void __resolveQueueFamilyInfos() noexcept;
	};

	constexpr const VkPhysicalDeviceProperties &PhysicalDevice::get10Props() const noexcept
	{
		return __props.properties;
	}

	constexpr const VkPhysicalDeviceVulkan11Properties &PhysicalDevice::get11Props() const noexcept
	{
		return __11props;
	}

	constexpr const VkPhysicalDeviceVulkan12Properties &PhysicalDevice::get12Props() const noexcept
	{
		return __12props;
	}

	constexpr const VkPhysicalDeviceVulkan13Properties &PhysicalDevice::get13Props() const noexcept
	{
		return __13props;
	}

	constexpr const VkPhysicalDeviceRobustness2PropertiesEXT &PhysicalDevice::getRobustness2Props() const noexcept
	{
		return __robustness2Props;
	}

	constexpr const VkPhysicalDeviceFeatures &PhysicalDevice::get10Features() const noexcept
	{
		return __features.features;
	}

	constexpr const VkPhysicalDeviceVulkan11Features &PhysicalDevice::get11Features() const noexcept
	{
		return __11features;
	}

	constexpr const VkPhysicalDeviceVulkan12Features &PhysicalDevice::get12Features() const noexcept
	{
		return __12features;
	}

	constexpr const VkPhysicalDeviceVulkan13Features &PhysicalDevice::get13Features() const noexcept
	{
		return __13features;
	}

	constexpr const VkPhysicalDeviceRobustness2FeaturesEXT &PhysicalDevice::getRobustness2Features() const noexcept
	{
		return __robustness2Features;
	}

	constexpr const std::unordered_map<std::string_view, const VkExtensionProperties *> &PhysicalDevice::getExtensionMap() const noexcept
	{
		return __extensionMap;
	}

	constexpr const std::unordered_map<VkFormat, VkFormatProperties3> &PhysicalDevice::getFormatPropertiesMap() const noexcept
	{
		return __formatPropMap;
	}

	constexpr const std::vector<QueueFamilyInfo> &PhysicalDevice::getQueueFamilyInfos() const noexcept 
	{
		return __queueFamilyInfos;
	}
}

module: private;

namespace Graphics
{
	PhysicalDevice::PhysicalDevice(const MakeInfo &makeInfo) noexcept :
		__instanceProc		{ *(makeInfo.pInstanceProc) },
		__hInstance			{ makeInfo.hInstance },
		__hPhysicalDevice	{ makeInfo.hPhysicalDevice }
	{
		__resolveProps();
		__resolveFeatures();
		__resolveExtensions();
		__resolveFormatProps();
		__resolveQueueFamilyInfos();
	}

	bool PhysicalDevice::isWin32PresentSupported(const uint32_t queueFamilyIndex) const noexcept
	{
		return __instanceProc.vkGetPhysicalDeviceWin32PresentationSupportKHR(__hPhysicalDevice, queueFamilyIndex);
	}

	std::unique_ptr<LogicalDevice> PhysicalDevice::createLogicalDevice(
		const uint32_t queueFamilyIndex,
		const VkPhysicalDeviceFeatures2 &features,
		const std::vector<const char *> &extensions) const
	{
		const LogicalDevice::CreateInfo createInfo
		{
			.pInstanceProc		{ &__instanceProc },
			.hInstance			{ __hInstance },
			.hPhysicalDevice	{ __hPhysicalDevice },
			.queueFamilyIndex	{ queueFamilyIndex },
			.pFeatures			{ &features },
			.pExtensions		{ &extensions }
		};

		return std::make_unique<LogicalDevice>(createInfo);
	}

	void PhysicalDevice::__resolveProps() noexcept
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

		__instanceProc.vkGetPhysicalDeviceProperties2(__hPhysicalDevice, &__props);
	}

	void PhysicalDevice::__resolveFeatures() noexcept
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
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.PhysicalDevice;

import ntmonkeys.com.Lib.Unique;
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

	export class PhysicalDevice : public Lib::Unique
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
		constexpr const VkPhysicalDeviceDescriptorBufferPropertiesEXT &getDescriptorBufferProps() const noexcept;

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
		constexpr const VkPhysicalDeviceDescriptorBufferFeaturesEXT &getDescriptorBufferFeatures() const noexcept;

		[[nodiscard]]
		constexpr const std::unordered_map<std::string_view, const VkExtensionProperties *> &getExtensions() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDeviceMemoryProperties &getMemoryProps() const noexcept;

		[[nodiscard]]
		constexpr const std::unordered_map<VkFormat, VkFormatProperties3> &getFormatProps() const noexcept;

		[[nodiscard]]
		constexpr const std::vector<QueueFamilyInfo> &getQueueFamilyInfos() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getQueueFamilyIndex() const noexcept;

		[[nodiscard]]
		constexpr const VkPhysicalDevice &getHandle() const noexcept;

		[[nodiscard]]
		LogicalDevice *createLogicalDevice() const;

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __handle;

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

		VkPhysicalDeviceMemoryProperties2 __memoryProps;
		std::unordered_map<VkFormat, VkFormatProperties3> __formatPropMap;

		std::vector<VkQueueFamilyProperties2> __queueFamilyProps;
		std::vector<VkQueueFamilyGlobalPriorityPropertiesKHR> __queueFamilyGlobalPriorityProps;
		std::vector<QueueFamilyInfo> __queueFamilyInfos;

		uint32_t __queueFamilyIndex{ };

		void __resolveProps() noexcept;
		void __resolveFeatures() noexcept;
		void __resolveExtensions() noexcept;
		void __resolveMemoryProps() noexcept;
		void __resolveFormatProps() noexcept;
		void __resolveQueueFamilyInfos() noexcept;
		void __resolveQueueFamilyIndex();
	};

	constexpr const VkPhysicalDeviceProperties &PhysicalDevice::get10Props() const noexcept
	{
		return __props.properties;
	}

	constexpr const VkPhysicalDeviceVulkan11Properties &PhysicalDevice::get11Props() const noexcept
	{
		return __11Props;
	}

	constexpr const VkPhysicalDeviceVulkan12Properties &PhysicalDevice::get12Props() const noexcept
	{
		return __12Props;
	}

	constexpr const VkPhysicalDeviceVulkan13Properties &PhysicalDevice::get13Props() const noexcept
	{
		return __13Props;
	}

	constexpr const VkPhysicalDeviceRobustness2PropertiesEXT &PhysicalDevice::getRobustness2Props() const noexcept
	{
		return __robustness2Props;
	}

	constexpr const VkPhysicalDeviceDescriptorBufferPropertiesEXT &PhysicalDevice::getDescriptorBufferProps() const noexcept
	{
		return __descriptorBufferProps;
	}

	constexpr const VkPhysicalDeviceFeatures &PhysicalDevice::get10Features() const noexcept
	{
		return __features.features;
	}

	constexpr const VkPhysicalDeviceVulkan11Features &PhysicalDevice::get11Features() const noexcept
	{
		return __11Features;
	}

	constexpr const VkPhysicalDeviceVulkan12Features &PhysicalDevice::get12Features() const noexcept
	{
		return __12Features;
	}

	constexpr const VkPhysicalDeviceVulkan13Features &PhysicalDevice::get13Features() const noexcept
	{
		return __13Features;
	}

	constexpr const VkPhysicalDeviceRobustness2FeaturesEXT &PhysicalDevice::getRobustness2Features() const noexcept
	{
		return __robustness2Features;
	}

	constexpr const VkPhysicalDeviceDescriptorBufferFeaturesEXT &PhysicalDevice::getDescriptorBufferFeatures() const noexcept
	{
		return __descriptorBufferFeatures;
	}

	constexpr const std::unordered_map<std::string_view, const VkExtensionProperties *> &PhysicalDevice::getExtensions() const noexcept
	{
		return __extensionMap;
	}

	constexpr const VkPhysicalDeviceMemoryProperties &PhysicalDevice::getMemoryProps() const noexcept
	{
		return __memoryProps.memoryProperties;
	}

	constexpr const std::unordered_map<VkFormat, VkFormatProperties3> &PhysicalDevice::getFormatProps() const noexcept
	{
		return __formatPropMap;
	}

	constexpr const std::vector<QueueFamilyInfo> &PhysicalDevice::getQueueFamilyInfos() const noexcept
	{
		return __queueFamilyInfos;
	}

	constexpr uint32_t PhysicalDevice::getQueueFamilyIndex() const noexcept
	{
		return __queueFamilyIndex;
	}

	constexpr const VkPhysicalDevice &PhysicalDevice::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	PhysicalDevice::PhysicalDevice(const MakeInfo &makeInfo) noexcept :
		__instanceProc		{ *(makeInfo.pInstanceProc) },
		__hInstance			{ makeInfo.hVulkanInstance },
		__handle			{ makeInfo.hPhysicalDevice }
	{
		__resolveProps();
		__resolveFeatures();
		__resolveExtensions();
		__resolveMemoryProps();
		__resolveFormatProps();
		__resolveQueueFamilyInfos();
		__resolveQueueFamilyIndex();
	}

	LogicalDevice *PhysicalDevice::createLogicalDevice() const
	{
		const LogicalDevice::CreateInfo createInfo
		{
			.pInstanceProc				{ &__instanceProc },
			.hInstance					{ __hInstance },
			.hPhysicalDevice			{ __handle },
			.queueFamilyIndex			{ __queueFamilyIndex },

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

		return new LogicalDevice{ createInfo };
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

		__instanceProc.vkGetPhysicalDeviceProperties2(__handle, &__props);
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

		__instanceProc.vkGetPhysicalDeviceFeatures2(__handle, &__features);
	}

	void PhysicalDevice::__resolveExtensions() noexcept
	{
		uint32_t extensionCount{ };
		__instanceProc.vkEnumerateDeviceExtensionProperties(__handle, nullptr, &extensionCount, nullptr);

		__extensions.resize(extensionCount);
		__instanceProc.vkEnumerateDeviceExtensionProperties(__handle, nullptr, &extensionCount, __extensions.data());

		for (const auto &extension : __extensions)
			__extensionMap[extension.extensionName] = &extension;
	}

	void PhysicalDevice::__resolveMemoryProps() noexcept
	{
		__memoryProps.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
		__instanceProc.vkGetPhysicalDeviceMemoryProperties2(__handle, &__memoryProps);
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

			__instanceProc.vkGetPhysicalDeviceFormatProperties2(__handle, format, &props);
			__formatPropMap[format] = prop3;
		}
	}

	void PhysicalDevice::__resolveQueueFamilyInfos() noexcept
	{
		uint32_t familyCount{ };
		__instanceProc.vkGetPhysicalDeviceQueueFamilyProperties2(__handle, &familyCount, nullptr);

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

		__instanceProc.vkGetPhysicalDeviceQueueFamilyProperties2(__handle, &familyCount, __queueFamilyProps.data());
	}

	void PhysicalDevice::__resolveQueueFamilyIndex()
	{
		std::optional<uint32_t> queueFamilyIndex;

		const uint32_t familyCount{ static_cast<uint32_t>(__queueFamilyInfos.size()) };
		for (uint32_t iter{ }; iter < familyCount; ++iter)
		{
			const auto &familyInfo{ __queueFamilyInfos[iter] };

			const bool hasGraphicsFlag{ static_cast<bool>(familyInfo.pProps->queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT) };
			if (!hasGraphicsFlag)
				continue;

			queueFamilyIndex = iter;
			break;
		}

		if (queueFamilyIndex.has_value())
		{
			__queueFamilyIndex = queueFamilyIndex.value();
			return;
		}

		throw std::runtime_error{ "Cannot resolve suitable queue family index." };
	}
}
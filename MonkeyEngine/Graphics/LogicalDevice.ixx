module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.LogicalDevice;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Graphics.Queue;
import ntmonkeys.com.Graphics.Surface;
import <vector>;
import <memory>;
import <stdexcept>;
import <unordered_map>;
import <format>;

namespace Graphics
{
	export class LogicalDevice : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::InstanceProc *pInstanceProc{ };
			VkInstance hInstance{ };
			VkPhysicalDevice hPhysicalDevice{ };
			uint32_t queueFamilyIndex{ };
			
			const VkPhysicalDeviceProperties *p10Props{ };
			const VkPhysicalDeviceVulkan11Properties *p11Props{ };
			const VkPhysicalDeviceVulkan12Properties *p12Props{ };
			const VkPhysicalDeviceVulkan13Properties *p13Props{ };
			const VkPhysicalDeviceRobustness2PropertiesEXT *pRobustness2Props{ };
			const VkPhysicalDeviceDescriptorBufferPropertiesEXT *pDescriptorBufferProps{ };

			const VkPhysicalDeviceFeatures *p10Features{ };
			const VkPhysicalDeviceVulkan11Features *p11Features{ };
			const VkPhysicalDeviceVulkan12Features *p12Features{ };
			const VkPhysicalDeviceVulkan13Features *p13Features{ };
			const VkPhysicalDeviceRobustness2FeaturesEXT *pRobustness2Features{ };
			const VkPhysicalDeviceDescriptorBufferFeaturesEXT *pDescriptorBufferFeatures{ };
			const VkPhysicalDeviceVariablePointersFeatures *pVariablePointersFeatures{ };

			const std::unordered_map<std::string_view, const VkExtensionProperties *> *pExtensionMap;
		};

		explicit LogicalDevice(const CreateInfo &createInfo) noexcept;
		virtual ~LogicalDevice() noexcept override;
		
		[[nodiscard]]
		constexpr Queue &getQueue() noexcept;

		[[nodiscard]]
		std::unique_ptr<Surface> createSurface(const HINSTANCE hAppInstance, const HWND hwnd);

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;
		const uint32_t __queueFamilyIndex;

		VkDevice __handle{ };
		VK::DeviceProc __deviceProc;

		std::unique_ptr<Queue> __pQueue;
		VkPipelineCache __hPipelineCache{ };

		void __createDevice(const CreateInfo &createInfo);
		void __loadDeviceProc() noexcept;
		void __retrieveQueue();
		void __createPipelineCache();
	};

	constexpr Queue &LogicalDevice::getQueue() noexcept
	{
		return *__pQueue;
	}
}

module: private;

#pragma warning(disable: 5103)
#define LOAD_DEVICE_PROC(funcName) (__deviceProc.##funcName = reinterpret_cast<PFN_##funcName>(__instanceProc.vkGetDeviceProcAddr(__handle, #funcName)))

namespace Graphics
{
	LogicalDevice::LogicalDevice(const CreateInfo &createInfo) noexcept :
		__instanceProc		{ *(createInfo.pInstanceProc) },
		__hInstance			{ createInfo.hInstance },
		__hPhysicalDevice	{ createInfo.hPhysicalDevice },
		__queueFamilyIndex	{ createInfo.queueFamilyIndex }
	{
		__createDevice(createInfo);
		__loadDeviceProc();
		__retrieveQueue();
		__createPipelineCache();
	}

	LogicalDevice::~LogicalDevice() noexcept
	{
		__deviceProc.vkDestroyPipelineCache(__handle, __hPipelineCache, nullptr);
		__deviceProc.vkDestroyDevice(__handle, nullptr);
	}

	std::unique_ptr<Surface> LogicalDevice::createSurface(const HINSTANCE hAppInstance, const HWND hwnd)
	{
		const Surface::CreateInfo createInfo
		{
			.pInstanceProc		{ &__instanceProc },
			.hInstance			{ __hInstance },
			.hPhysicalDevice	{ __hPhysicalDevice },
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.hAppInstance		{ hAppInstance },
			.hwnd				{ hwnd }
		};

		return std::make_unique<Surface>(createInfo);
	}

	void LogicalDevice::__createDevice(const CreateInfo &createInfo)
	{
		const auto deviceVersion{ Graphics::ConversionUtil::fromVulkanVersion(createInfo.p10Props->apiVersion) };
		if ((deviceVersion.major <= 1U) && (deviceVersion.minor < 3U))
			throw std::runtime_error{ "The device API version is too low." };

		const bool featureSupported
		{
			createInfo.p10Features->samplerAnisotropy &&
			createInfo.p11Features->variablePointers &&
			createInfo.p11Features->variablePointersStorageBuffer &&
			createInfo.p12Features->imagelessFramebuffer &&
			createInfo.p12Features->timelineSemaphore &&
			createInfo.p13Features->pipelineCreationCacheControl &&
			createInfo.p13Features->synchronization2 &&
			createInfo.pRobustness2Features->nullDescriptor &&
			createInfo.pDescriptorBufferFeatures->descriptorBuffer
		};

		if (!featureSupported)
			throw std::runtime_error{ "The device doesn't support the features." };

		if (!(__instanceProc.vkGetPhysicalDeviceWin32PresentationSupportKHR(__hPhysicalDevice, __queueFamilyIndex)))
			throw std::runtime_error{ "The device doesn't support win32 presentation." };
		
		VkPhysicalDeviceFeatures2 features{ };
		VkPhysicalDeviceVulkan11Features features11{ };
		VkPhysicalDeviceVulkan12Features features12{ };
		VkPhysicalDeviceVulkan13Features features13{ };
		VkPhysicalDeviceRobustness2FeaturesEXT featuresRobustness{ };
		VkPhysicalDeviceDescriptorBufferFeaturesEXT featuresDescriptorBuffer{ };

		features.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		features.features.samplerAnisotropy = VK_TRUE;
		features.pNext = &features11;

		features11.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		features11.variablePointers = VK_TRUE;
		features11.variablePointersStorageBuffer = VK_TRUE;
		features11.pNext = &features12;

		features12.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		features12.imagelessFramebuffer = VK_TRUE;
		features12.timelineSemaphore = VK_TRUE;
		features12.pNext = &features13;

		features13.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		features13.pipelineCreationCacheControl = VK_TRUE;
		features13.synchronization2 = VK_TRUE;
		features13.pNext = &featuresRobustness;

		featuresRobustness.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ROBUSTNESS_2_FEATURES_EXT;
		featuresRobustness.nullDescriptor = VK_TRUE;
		featuresRobustness.pNext = &featuresDescriptorBuffer;

		featuresDescriptorBuffer.sType = VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_BUFFER_FEATURES_EXT;
		featuresDescriptorBuffer.descriptorBuffer = VK_TRUE;
		featuresDescriptorBuffer.pNext = nullptr;

		std::vector<const char *> extensions;
		extensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_MAINTENANCE_5_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_ROBUSTNESS_2_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_DESCRIPTOR_BUFFER_EXTENSION_NAME);
		extensions.emplace_back(VK_EXT_FULL_SCREEN_EXCLUSIVE_EXTENSION_NAME);
		//extensions.emplace_back(VK_EXT_IMAGE_COMPRESSION_CONTROL_EXTENSION_NAME);

		for (const auto extension : extensions)
		{
			if (createInfo.pExtensionMap->contains(extension))
				continue;

			throw std::runtime_error{ std::format("Device extension not supported: {}", extension) };
		}

		static constexpr float queuePriority{ 1.0f };

		const VkDeviceQueueCreateInfo queueCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.queueCount			{ 1U },
			.pQueuePriorities	{ &queuePriority }
		};

		const VkDeviceCreateInfo vkCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
			.pNext						{ &features },
			.queueCreateInfoCount		{ 1U },
			.pQueueCreateInfos			{ &queueCreateInfo },
			.enabledExtensionCount		{ static_cast<uint32_t>(extensions.size()) },
			.ppEnabledExtensionNames	{ extensions.data() }
		};

		__instanceProc.vkCreateDevice(__hPhysicalDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a VkDevice." };
	}

	void LogicalDevice::__loadDeviceProc() noexcept
	{
		// Device
		LOAD_DEVICE_PROC(vkDeviceWaitIdle);
		LOAD_DEVICE_PROC(vkDestroyDevice);

		// Queue
		LOAD_DEVICE_PROC(vkGetDeviceQueue2);
		LOAD_DEVICE_PROC(vkQueueWaitIdle);
		LOAD_DEVICE_PROC(vkQueueSubmit2);
		LOAD_DEVICE_PROC(vkQueuePresentKHR);

		// Descriptor set layout
		LOAD_DEVICE_PROC(vkCreateDescriptorSetLayout);
		LOAD_DEVICE_PROC(vkDestroyDescriptorSetLayout);

		// Pipeline layout
		LOAD_DEVICE_PROC(vkCreatePipelineLayout);
		LOAD_DEVICE_PROC(vkDestroyPipelineLayout);

		// Pipeline cache
		LOAD_DEVICE_PROC(vkCreatePipelineCache);
		LOAD_DEVICE_PROC(vkDestroyPipelineCache);

		// Pipeline
		LOAD_DEVICE_PROC(vkCreateGraphicsPipelines);
		LOAD_DEVICE_PROC(vkDestroyPipeline);

		// Swapchain
		LOAD_DEVICE_PROC(vkCreateSwapchainKHR);
		LOAD_DEVICE_PROC(vkDestroySwapchainKHR);
		LOAD_DEVICE_PROC(vkGetSwapchainImagesKHR);

		LOAD_DEVICE_PROC(vkCreateImageView);
		LOAD_DEVICE_PROC(vkDestroyImageView);
	}

	void LogicalDevice::__retrieveQueue()
	{
		const Queue::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.familyIndex	{ __queueFamilyIndex },
			.queueIndex		{ 0U }
		};

		__pQueue = std::make_unique<Queue>(createInfo);
	}

	void LogicalDevice::__createPipelineCache()
	{
		const VkPipelineCacheCreateInfo createInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO },
			.flags{ VkPipelineCacheCreateFlagBits::VK_PIPELINE_CACHE_CREATE_EXTERNALLY_SYNCHRONIZED_BIT }
		};

		__deviceProc.vkCreatePipelineCache(__handle, &createInfo, nullptr, &__hPipelineCache);
		if (!__hPipelineCache)
			throw std::runtime_error{ "Cannot create a pipeline cache." };
	}
}
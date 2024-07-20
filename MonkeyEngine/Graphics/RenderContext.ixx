module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.RenderContext;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Lib.Logger;
import ntmonkeys.com.VK.VulkanLoader;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.Surface;
import ntmonkeys.com.Graphics.ConversionUtil;
import <stdexcept>;
import <memory>;
import <vector>;
import <array>;
import <string>;
import <format>;
import <unordered_map>;

namespace Graphics
{
	export class RenderContext : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			std::string vulkanLoaderLibName;
			bool debugMode{ };
			PFN_vkDebugUtilsMessengerCallbackEXT debugCallback{ };

			std::string appName;
			Lib::Version appVersion;

			std::string engineName;
			Lib::Version engineVersion;
		};

		explicit RenderContext(const CreateInfo &createInfo) noexcept;
		virtual ~RenderContext() noexcept override;

		[[nodiscard]]
		constexpr const std::vector<std::unique_ptr<PhysicalDevice>> &getPhysicalDevices() const noexcept;

	private:
		std::unique_ptr<VK::VulkanLoader> __pVulkanLoader;

		Lib::Version __instanceVersion;
		std::vector<VkLayerProperties> __instanceLayers;
		std::vector<VkExtensionProperties> __instanceExtensions;

		std::unordered_map<std::string_view, const VkLayerProperties *> __instanceLayerMap;
		std::unordered_map<std::string_view, const VkExtensionProperties *> __instanceExtensionMap;

		VkDebugUtilsMessengerCreateInfoEXT __debugMessengerCreateInfo{ };

		VkInstance __hVulkanInstance{ };
		VK::InstanceProc __instanceProc;

		VkDebugUtilsMessengerEXT __hDebugMessenger{ };

		std::vector<std::unique_ptr<PhysicalDevice>> __physicalDevices;

		void __createVulkanLoader(const std::string &libName);
		void __resolveInstanceVersion();
		void __resolveInstanceLayers() noexcept;
		void __resolveInstanceExtensions() noexcept;

		void __populateDebugMessengerCreateInfo(const PFN_vkDebugUtilsMessengerCallbackEXT debugCallback) noexcept;

		void __createVulkanInstance(
			const std::string &appName, const Lib::Version &appVersion,
			const std::string &engineName, const Lib::Version &engineVersion);

		void __loadInstanceProc() noexcept;

		void __createDebugMessenger();
		void __enumeratePhysicalDevices();
	};

	constexpr const std::vector<std::unique_ptr<PhysicalDevice>> &RenderContext::getPhysicalDevices() const noexcept
	{
		return __physicalDevices;
	}
}

module: private;

#pragma warning(disable: 5103)
#define LOAD_INSTANCE_PROC(funcName) (__instanceProc.##funcName = reinterpret_cast<PFN_##funcName>(globalProc.vkGetInstanceProcAddr(__hVulkanInstance, #funcName)))

namespace Graphics
{
	RenderContext::RenderContext(const CreateInfo &createInfo) noexcept
	{
		__createVulkanLoader(createInfo.vulkanLoaderLibName);
		__resolveInstanceVersion();
		__resolveInstanceLayers();
		__resolveInstanceExtensions();

		if (createInfo.debugMode)
			__populateDebugMessengerCreateInfo(createInfo.debugCallback);

		__createVulkanInstance(
			createInfo.appName, createInfo.appVersion,
			createInfo.engineName, createInfo.engineVersion);

		__loadInstanceProc();

		if (createInfo.debugMode)
			__createDebugMessenger();

		__enumeratePhysicalDevices();
	}

	RenderContext::~RenderContext() noexcept
	{
		if (__hDebugMessenger)
			__instanceProc.vkDestroyDebugUtilsMessengerEXT(__hVulkanInstance, __hDebugMessenger, nullptr);

		__instanceProc.vkDestroyInstance(__hVulkanInstance, nullptr);
	}

	void RenderContext::__createVulkanLoader(const std::string &libName)
	{
		__pVulkanLoader = std::make_unique<VK::VulkanLoader>(libName);
		if (__pVulkanLoader->isVulkanSupported())
			return;

		throw std::runtime_error{ "Current system does not support Vulkan." };
	}

	void RenderContext::__resolveInstanceVersion()
	{
		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };

		uint32_t encodedVer{ };

		if (!(globalProc.vkEnumerateInstanceVersion))
			encodedVer = VK_API_VERSION_1_0;
		else
			globalProc.vkEnumerateInstanceVersion(&encodedVer);

		__instanceVersion = Graphics::ConversionUtil::fromVulkanVersion(encodedVer);
		if (__instanceVersion < Graphics::ConversionUtil::fromVulkanVersion(VK_API_VERSION_1_3))
			throw std::runtime_error{ "The supported vulkan instance version is too low." };
	}

	void RenderContext::__resolveInstanceLayers() noexcept
	{
		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };

		uint32_t layerCount{ };
		globalProc.vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		__instanceLayers.resize(layerCount);
		globalProc.vkEnumerateInstanceLayerProperties(&layerCount, __instanceLayers.data());

		for (const auto &layer : __instanceLayers)
			__instanceLayerMap[layer.layerName] = &layer;
	}

	void RenderContext::__resolveInstanceExtensions() noexcept
	{
		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };

		uint32_t extensionCount{ };
		globalProc.vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		__instanceExtensions.resize(extensionCount);
		globalProc.vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, __instanceExtensions.data());

		for (const auto &extension : __instanceExtensions)
			__instanceExtensionMap[extension.extensionName] = &extension;
	}

	void RenderContext::__populateDebugMessengerCreateInfo(const PFN_vkDebugUtilsMessengerCallbackEXT debugCallback) noexcept
	{
		__debugMessengerCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		__debugMessengerCreateInfo.messageSeverity = (
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		);

		__debugMessengerCreateInfo.messageType = (
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
		);

		__debugMessengerCreateInfo.pfnUserCallback = debugCallback;
	}

	void RenderContext::__createVulkanInstance(
		const std::string &appName, const Lib::Version &appVersion,
		const std::string &engineName, const Lib::Version &engineVersion)
	{
		const void *pNextChain{ };
		std::vector<const char *> layers;
		std::vector<const char *> extensions;

		extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME);

#ifndef NDEBUG
		layers.emplace_back("VK_LAYER_KHRONOS_validation");
		extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

		static constexpr std::array enabledFeatures
		{
			// 퍼포먼스 떨어지는 코드 경고
			VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT,

			// 메모리 해저드 경고
			VkValidationFeatureEnableEXT::VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT
		};

		const VkValidationFeaturesEXT validationFeatures
		{
			.sType							{ VkStructureType::VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT },
			.pNext							{ &__debugMessengerCreateInfo },
			.enabledValidationFeatureCount	{ static_cast<uint32_t>(enabledFeatures.size()) },
			.pEnabledValidationFeatures		{ enabledFeatures.data() }
		};

		pNextChain = &validationFeatures;
#endif

		for (const auto layer : layers)
		{
			if (__instanceLayerMap.contains(layer))
				continue;

			throw std::runtime_error{ std::format("Instance layer not supported: {}", layer) };
		}

		for (const auto extension : extensions)
		{
			if (__instanceExtensionMap.contains(extension))
				continue;

			throw std::runtime_error{ std::format("Instance extension not supported: {}", extension) };
		}

		const VkApplicationInfo appInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO },
			.pApplicationName	{ appName.c_str() },
			.applicationVersion	{ ConversionUtil::toVulkanVersion(appVersion) },
			.pEngineName		{ engineName.c_str() },
			.engineVersion		{ ConversionUtil::toVulkanVersion(engineVersion) },
			.apiVersion			{ ConversionUtil::toVulkanVersion(__instanceVersion) }
		};

		const VkInstanceCreateInfo createInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
			.pNext						{ pNextChain },
			.pApplicationInfo			{ &appInfo },
			.enabledLayerCount			{ static_cast<uint32_t>(layers.size()) },
			.ppEnabledLayerNames		{ layers.data() },
			.enabledExtensionCount		{ static_cast<uint32_t>(extensions.size()) },
			.ppEnabledExtensionNames	{ extensions.data() }
		};

		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };
		globalProc.vkCreateInstance(&createInfo, nullptr, &__hVulkanInstance);

		if (!__hVulkanInstance)
			throw std::runtime_error{ "Cannot create a Vulkan instance." };
	}

	void RenderContext::__loadInstanceProc() noexcept
	{
		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };

		// VulkanInstance
		LOAD_INSTANCE_PROC(vkDestroyInstance);
		LOAD_INSTANCE_PROC(vkCreateDebugUtilsMessengerEXT);
		LOAD_INSTANCE_PROC(vkDestroyDebugUtilsMessengerEXT);
		LOAD_INSTANCE_PROC(vkEnumeratePhysicalDevices);

		// Physical device
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceProperties2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceFeatures2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceFormatProperties2);
		LOAD_INSTANCE_PROC(vkEnumerateDeviceExtensionProperties);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceMemoryProperties2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceQueueFamilyProperties2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfaceSupportKHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceWin32PresentationSupportKHR);

		// Physical device - Surface
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfaceFormats2KHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfacePresentModesKHR);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceSurfacePresentModes2EXT);

		// Surface
		LOAD_INSTANCE_PROC(vkCreateWin32SurfaceKHR);
		LOAD_INSTANCE_PROC(vkDestroySurfaceKHR);

		// Device
		LOAD_INSTANCE_PROC(vkCreateDevice);
		LOAD_INSTANCE_PROC(vkGetDeviceProcAddr);
	}

	void RenderContext::__createDebugMessenger()
	{
        __instanceProc.vkCreateDebugUtilsMessengerEXT(__hVulkanInstance, &__debugMessengerCreateInfo, nullptr, &__hDebugMessenger);
        if (!__hDebugMessenger)
            throw std::runtime_error{ "Cannot create a debug messenger." };
	}

	void RenderContext::__enumeratePhysicalDevices()
	{
		std::vector<VkPhysicalDevice> handles;

		uint32_t deviceCount{ };
		__instanceProc.vkEnumeratePhysicalDevices(__hVulkanInstance, &deviceCount, nullptr);

		if (!deviceCount)
			throw std::runtime_error{ "No physical devices are detected." };

		handles.resize(deviceCount);
		__instanceProc.vkEnumeratePhysicalDevices(__hVulkanInstance, &deviceCount, handles.data());

		for (const auto handle : handles)
		{
			const PhysicalDevice::MakeInfo createInfo
			{
				.pInstanceProc		{ &__instanceProc },
				.hVulkanInstance	{ __hVulkanInstance },
				.hPhysicalDevice	{ handle }
			};

			__physicalDevices.emplace_back(std::make_unique<PhysicalDevice>(createInfo));
		}
	}
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Core;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Lib.Logger;
import ntmonkeys.com.VK.VulkanLoader;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Engine.ConversionUtil;
import ntmonkeys.com.Graphics.RenderContext;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.DebugMessenger;
import ntmonkeys.com.Engine.RenderingEngine;
import <stdexcept>;
import <unordered_map>;
import <memory>;
import <array>;

namespace Engine
{
	export class Core : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			std::string vulkanLoaderLibName;

			std::string appName;
			Lib::Version appVersion;

			std::string engineName;
			Lib::Version engineVersion;

			Lib::Version instanceVersion{ 1U, 0U, 0U, 0U };
		};

		Core(const CreateInfo &createInfo);
		virtual ~Core() noexcept;

		[[nodiscard]]
		std::unique_ptr<RenderingEngine> createEngine();

	private:
		std::unique_ptr<VK::VulkanLoader> __pVulkanLoader;

		Lib::Version __instanceVer;

		std::vector<VkLayerProperties> __instanceLayers;
		std::vector<VkExtensionProperties> __instanceExtensions;

		std::unordered_map<std::string_view, const VkLayerProperties *> __instanceLayerMap;
		std::unordered_map<std::string_view, const VkExtensionProperties *> __instanceExtensionMap;

		VkDebugUtilsMessengerCreateInfoEXT __debugMessengerCreateInfo{ };

		std::unique_ptr<Graphics::RenderContext> __pRenderContext;
		std::unique_ptr<Graphics::DebugMessenger> __pDebugMessenger;

		void __createVulkanLoader(const std::string &libName);
		void __resolveInstanceVersion();
		void __resolveInstanceLayers() noexcept;
		void __resolveInstanceExtensions() noexcept;
		constexpr void __populateDebugMessengerCreateInfo() noexcept;

		void __createRenderContext(
			const std::string &appName, const Lib::Version &appVersion,
			const std::string &engineName, const Lib::Version &engineVersion);

		void __createDebugMessenger();

		static VkBool32 __vkDebugUtilsMessengerCallbackEXT(
			const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT *const pCallbackData,
			void *const pUserData) noexcept;
	};
}

module: private;

namespace Engine
{
	Core::Core(const CreateInfo &createInfo) :
		__instanceVer{ createInfo.instanceVersion }
	{
		__createVulkanLoader(createInfo.vulkanLoaderLibName);
		__resolveInstanceVersion();
		__resolveInstanceLayers();
		__resolveInstanceExtensions();

#ifndef NDEBUG
		__populateDebugMessengerCreateInfo();
#endif

		__createRenderContext(
			createInfo.appName, createInfo.appVersion,
			createInfo.engineName, createInfo.engineVersion);

#ifndef NDEBUG
		__createDebugMessenger();
#endif
	}

	Core::~Core() noexcept
	{
		__pDebugMessenger = nullptr;
		__pRenderContext = nullptr;
	}

	std::unique_ptr<RenderingEngine> Core::createEngine()
	{
		const auto &devices{ __pRenderContext->getPhysicalDevices() };
		return std::make_unique<RenderingEngine>(devices.front());
	}

	void Core::__createVulkanLoader(const std::string &libName)
	{
		__pVulkanLoader = std::make_unique<VK::VulkanLoader>(libName);
		if (__pVulkanLoader->isVulkanSupported())
			return;

		throw std::runtime_error{ "Current device does not support Vulkan." };
	}

	void Core::__resolveInstanceVersion()
	{
		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };

		uint32_t encodedVer{ };

		if (!(globalProc.vkEnumerateInstanceVersion))
			encodedVer = VK_API_VERSION_1_0;
		else
			globalProc.vkEnumerateInstanceVersion(&encodedVer);

		const auto decodedVer{ ConversionUtil::fromVulkanVersion(encodedVer) };
		__instanceVer = ((__instanceVer < decodedVer) ? __instanceVer : decodedVer);

		if (__instanceVer < ConversionUtil::fromVulkanVersion(VK_API_VERSION_1_3))
			throw std::runtime_error{ "The supported vulkan instance version is too low." };
	}

	void Core::__resolveInstanceLayers() noexcept
	{
		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };

		uint32_t layerCount{ };
		globalProc.vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		__instanceLayers.resize(layerCount);
		globalProc.vkEnumerateInstanceLayerProperties(&layerCount, __instanceLayers.data());

		for (const auto &layer : __instanceLayers)
			__instanceLayerMap[layer.layerName] = &layer;
	}

	void Core::__resolveInstanceExtensions() noexcept
	{
		const auto &globalProc{ __pVulkanLoader->getGlobalProc() };

		uint32_t extensionCount{ };
		globalProc.vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		__instanceExtensions.resize(extensionCount);
		globalProc.vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, __instanceExtensions.data());

		for (const auto &extension : __instanceExtensions)
			__instanceExtensionMap[extension.extensionName] = &extension;
	}

	constexpr void Core::__populateDebugMessengerCreateInfo() noexcept
	{
		__debugMessengerCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		__debugMessengerCreateInfo.messageSeverity =
		(
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
		);

		__debugMessengerCreateInfo.messageType =
		(
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT
		);

		__debugMessengerCreateInfo.pfnUserCallback = __vkDebugUtilsMessengerCallbackEXT;
	}

	void Core::__createRenderContext(
		const std::string &appName, const Lib::Version &appVersion,
		const std::string &engineName, const Lib::Version &engineVersion)
	{
		const VkApplicationInfo appInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO },
			.pApplicationName	{ appName.c_str() },
			.applicationVersion	{ ConversionUtil::toVulkanVersion(appVersion) },
			.pEngineName		{ engineName.c_str() },
			.engineVersion		{ ConversionUtil::toVulkanVersion(engineVersion) },
			.apiVersion			{ ConversionUtil::toVulkanVersion(__instanceVer) }
		};

		VkInstanceCreateInfo createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
			.pApplicationInfo	{ &appInfo }
		};

		std::vector<const char *> layers;
		std::vector<const char *> extensions;

		if (!(__instanceExtensionMap.contains(VK_KHR_SURFACE_EXTENSION_NAME)))
			throw std::runtime_error{ "WSI is not supported." };

		if (!(__instanceExtensionMap.contains(VK_KHR_WIN32_SURFACE_EXTENSION_NAME)))
			throw std::runtime_error{ "WSI is not supported." };

		extensions.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);
		extensions.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#ifndef NDEBUG
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

		static constexpr auto vvlLayerName	{ "VK_LAYER_KHRONOS_validation" };
		const bool vvlSupported				{ __instanceLayerMap.contains(vvlLayerName) };
		const bool debugMessengerSupported	{ __instanceExtensionMap.contains(VK_EXT_DEBUG_UTILS_EXTENSION_NAME) };

		if (vvlSupported && debugMessengerSupported)
		{
			layers.emplace_back(vvlLayerName);
			extensions.emplace_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
			createInfo.pNext = &validationFeatures;
		}
#endif

		createInfo.enabledLayerCount		= static_cast<uint32_t>(layers.size());
		createInfo.ppEnabledLayerNames		= layers.data();

		createInfo.enabledExtensionCount	= static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames	= extensions.data();

		__pRenderContext = std::make_unique<Graphics::RenderContext>(__pVulkanLoader->getGlobalProc(), createInfo);
	}

	void Core::__createDebugMessenger()
	{
		__pDebugMessenger = __pRenderContext->createDebugMessenger(__debugMessengerCreateInfo);
	}

	VkBool32 Core::__vkDebugUtilsMessengerCallbackEXT(
		const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		const VkDebugUtilsMessageTypeFlagsEXT messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT *const pCallbackData,
		void *const pUserData) noexcept
	{
		Lib::Logger::Severity logSeverity{ };

		switch (messageSeverity)
		{
			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				logSeverity = Lib::Logger::Severity::FATAL;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				logSeverity = Lib::Logger::Severity::WARNING;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				logSeverity = Lib::Logger::Severity::INFO;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				logSeverity = Lib::Logger::Severity::VERBOSE;
				break;
		}

		Lib::Logger::log(logSeverity, pCallbackData->pMessage);
		return VK_FALSE;
	}
}
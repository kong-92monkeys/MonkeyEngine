module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Core;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Lib.Logger;
import ntmonkeys.com.VK.VulkanLoader;
import ntmonkeys.com.Graphics.ConversionUtil;
import <stdexcept>;
import <unordered_map>;
import <memory>;

namespace Graphics
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

	private:
		std::unique_ptr<VK::VulkanLoader> __pVulkanLoader;

		Lib::Version __instanceVer;

		std::vector<VkLayerProperties> __instanceLayers;
		std::vector<VkExtensionProperties> __instanceExtensions;

		std::unordered_map<std::string_view, const VkLayerProperties *> __instanceLayerMap;
		std::unordered_map<std::string_view, const VkExtensionProperties *> __instanceExtensionMap;

		VkDebugUtilsMessengerCreateInfoEXT __debugUtilsMessengerCreateInfo{ };

		VkInstance __hVulkanInstance{ };

		void __createVulkanLoader(const std::string &libName);
		void __resolveInstanceVersion();
		void __resolveInstanceLayers() noexcept;
		void __resolveInstanceExtensions() noexcept;
		constexpr void __populateDebugUtilsMessengerCreateInfo() noexcept;
		void __createVulkanInstance();

		static VkBool32 __vkDebugUtilsMessengerCallbackEXT(
			const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT *const pCallbackData,
			void *const pUserData) noexcept;
	};

	Core::Core(const CreateInfo &createInfo) :
		__instanceVer{ createInfo.instanceVersion }
	{
		__createVulkanLoader(createInfo.vulkanLoaderLibName);
		__resolveInstanceVersion();
		__resolveInstanceLayers();
		__resolveInstanceExtensions();

#ifndef NDEBUG
		__populateDebugUtilsMessengerCreateInfo();
#endif

		__createVulkanInstance();
	}

	Core::~Core() noexcept
	{

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

	constexpr void Core::__populateDebugUtilsMessengerCreateInfo() noexcept
	{
		__debugUtilsMessengerCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		__debugUtilsMessengerCreateInfo.messageSeverity =
			(
				VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
				VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
				VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT |
				VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				);

		__debugUtilsMessengerCreateInfo.messageType =
			(
				VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
				VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
				VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
				VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT |
				VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT
				);

		__debugUtilsMessengerCreateInfo.pfnUserCallback = __vkDebugUtilsMessengerCallbackEXT;
	}

	void Core::__createVulkanInstance()
	{
		// TODO: Validation layer ºôµåÇØ¾ß ÇÔ
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
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Core;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Lib.Logger;
import ntmonkeys.com.VK.VulkanLoader;
import ntmonkeys.com.Graphics.ConversionUtil;
import <stdexcept>;

namespace Graphics
{
	export class Core : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			std::string appName;
			Lib::Version appVersion;

			std::string engineName;
			Lib::Version engineVersion;

			Lib::Version instanceVersion{ 1U, 0U, 0U, 0U };
		};

		Core(const CreateInfo &createInfo);
		virtual ~Core() noexcept;

	private:
		VkDebugUtilsMessengerCreateInfoEXT __debugUtilsMessengerCreateInfo{ };
		Lib::Version __instanceVer;

		void __checkVulkanSupport();
		constexpr void __populateDebugUtilsMessengerCreateInfo() noexcept;
		void __resolveInstanceVersion();

		static VkBool32 __vkDebugUtilsMessengerCallbackEXT(
			const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT *const pCallbackData,
			void *const pUserData) noexcept;
	};

	Core::Core(const CreateInfo &createInfo) :
		__instanceVer{ createInfo.instanceVersion }
	{
		__checkVulkanSupport();

#ifndef NDEBUG
		__populateDebugUtilsMessengerCreateInfo();
#endif

		__resolveInstanceVersion();
	}

	Core::~Core() noexcept
	{

	}

	void Core::__checkVulkanSupport()
	{
		const auto &loader{ VK::VulkanLoader::getInstance() };
		if (loader.isVulkanSupported())
			return;

		throw std::runtime_error{ "Current device does not support Vulkan." };
	}

	constexpr void Core::__populateDebugUtilsMessengerCreateInfo() noexcept
	{
		__debugUtilsMessengerCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		__debugUtilsMessengerCreateInfo.messageSeverity =
		(
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT		|
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT		|
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT		|
			VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
		);

		__debugUtilsMessengerCreateInfo.messageType =
		(
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT					|
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT				|
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT				|
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_DEVICE_ADDRESS_BINDING_BIT_EXT	|
			VkDebugUtilsMessageTypeFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT
		);

		__debugUtilsMessengerCreateInfo.pfnUserCallback = __vkDebugUtilsMessengerCallbackEXT;
	}

	void Core::__resolveInstanceVersion()
	{
		const auto &globalProc{ VK::VulkanLoader::getInstance().getGlobalProc() };

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
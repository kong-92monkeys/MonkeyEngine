module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.RenderContext;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.DebugMessenger;
import ntmonkeys.com.Graphics.Surface;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Lib.Version;
import <stdexcept>;
import <memory>;
import <vector>;
import <array>;

namespace Graphics
{
	export class RenderContext : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::GlobalProc *pGlobalProc{ };

			std::string appName;
			Lib::Version appVersion;
			std::string engineName;
			Lib::Version engineVersion;
			Lib::Version instanceVersion;

			bool useVvl{ };
			PFN_vkDebugUtilsMessengerCallbackEXT vvlCallback{ };
			void *pVvlData{ };

			std::vector<const char *> *pLayers{ };
			std::vector<const char *> *pExtensions{ };
		};

		explicit RenderContext(const CreateInfo &createInfo) noexcept;
		virtual ~RenderContext() noexcept override;

		[[nodiscard]]
		constexpr const std::vector<PhysicalDevice> &getPhysicalDevices() const noexcept;

		[[nodiscard]]
		std::unique_ptr<DebugMessenger> createDebugMessenger(
			const PFN_vkDebugUtilsMessengerCallbackEXT callback,
			void *const pUserData);

	private:
		const VK::GlobalProc &__globalProc;
		
		VkInstance __handle{ };
		VK::InstanceProc __instanceProc;

		std::vector<PhysicalDevice> __physicalDevices;

		void __create(const CreateInfo &createInfo);
		void __loadInstanceProc() noexcept;
		void __enumeratePhysicalDevices() noexcept;
	};

	constexpr const std::vector<PhysicalDevice> &RenderContext::getPhysicalDevices() const noexcept
	{
		return __physicalDevices;
	}
}

module: private;

#pragma warning(disable: 5103)
#define LOAD_INSTANCE_PROC(funcName) (__instanceProc.##funcName = reinterpret_cast<PFN_##funcName>(__globalProc.vkGetInstanceProcAddr(__handle, #funcName)))

namespace Graphics
{
	RenderContext::RenderContext(const CreateInfo &createInfo) noexcept :
		__globalProc{ *(createInfo.pGlobalProc) }
	{
		__create(createInfo);
		__loadInstanceProc();
		__enumeratePhysicalDevices();
	}

	RenderContext::~RenderContext() noexcept
	{
		__instanceProc.vkDestroyInstance(__handle, nullptr);
	}

	std::unique_ptr<DebugMessenger> RenderContext::createDebugMessenger(
		const PFN_vkDebugUtilsMessengerCallbackEXT callback,
		void *const pUserData)
	{
		const DebugMessenger::CreateInfo createInfo
		{
			.pInstanceProc		{ &__instanceProc },
			.hInstance			{ __handle },
			.messageSeverity	{ DebugMessenger::defaultMessageSeverity },
			.messageType		{ DebugMessenger::defaultMessageType },
			.pfnUserCallback	{ callback },
			.pUserData			{ pUserData }
		};

		return std::make_unique<DebugMessenger>(createInfo);
	}

	void RenderContext::__create(const CreateInfo &createInfo)
	{
		const VkApplicationInfo appInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_APPLICATION_INFO },
			.pApplicationName	{ createInfo.appName.c_str() },
			.applicationVersion	{ ConversionUtil::toVulkanVersion(createInfo.appVersion) },
			.pEngineName		{ createInfo.engineName.c_str() },
			.engineVersion		{ ConversionUtil::toVulkanVersion(createInfo.engineVersion) },
			.apiVersion			{ ConversionUtil::toVulkanVersion(createInfo.instanceVersion) }
		};

		const VkDebugUtilsMessengerCreateInfoEXT debugMessengerCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT },
			.messageSeverity	{ DebugMessenger::defaultMessageSeverity },
			.messageType		{ DebugMessenger::defaultMessageType },
			.pfnUserCallback	{ createInfo.vvlCallback }
		};

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
			.pNext							{ &debugMessengerCreateInfo },
			.enabledValidationFeatureCount	{ static_cast<uint32_t>(enabledFeatures.size()) },
			.pEnabledValidationFeatures		{ enabledFeatures.data() }
		};

		const auto &layers		{ *(createInfo.pLayers) };
		const auto &extensions	{ *(createInfo.pExtensions) };

		const VkInstanceCreateInfo vkCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO },
			.pNext						{ createInfo.useVvl ? &validationFeatures : nullptr },
			.pApplicationInfo			{ &appInfo },
			.enabledLayerCount			{ static_cast<uint32_t>(layers.size()) },
			.ppEnabledLayerNames		{ layers.data() },
			.enabledExtensionCount		{ static_cast<uint32_t>(extensions.size()) },
			.ppEnabledExtensionNames	{ extensions.data() }
		};

		__globalProc.vkCreateInstance(&vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create RenderContext." };
	}

	void RenderContext::__loadInstanceProc() noexcept
	{
		// Instance
		LOAD_INSTANCE_PROC(vkDestroyInstance);
		LOAD_INSTANCE_PROC(vkCreateDebugUtilsMessengerEXT);
		LOAD_INSTANCE_PROC(vkDestroyDebugUtilsMessengerEXT);
		LOAD_INSTANCE_PROC(vkEnumeratePhysicalDevices);

		// Physical device
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceProperties2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceFeatures2);
		LOAD_INSTANCE_PROC(vkGetPhysicalDeviceFormatProperties2);
		LOAD_INSTANCE_PROC(vkEnumerateDeviceExtensionProperties);
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

	void RenderContext::__enumeratePhysicalDevices() noexcept
	{
		std::vector<VkPhysicalDevice> deviceHandles;

		uint32_t deviceCount{ };
		__instanceProc.vkEnumeratePhysicalDevices(__handle, &deviceCount, nullptr);

		deviceHandles.resize(deviceCount);
		__instanceProc.vkEnumeratePhysicalDevices(__handle, &deviceCount, deviceHandles.data());

		for (const auto deviceHandle : deviceHandles)
		{
			const PhysicalDevice::MakeInfo createInfo
			{
				.pInstanceProc		{ &__instanceProc },
				.hInstance			{ __handle },
				.hPhysicalDevice	{ deviceHandle }
			};

			__physicalDevices.emplace_back(createInfo);

		}
	}
}
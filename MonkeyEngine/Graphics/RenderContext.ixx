module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.RenderContext;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.DebugMessenger;
import ntmonkeys.com.Graphics.Surface;
import <stdexcept>;
import <memory>;
import <vector>;

namespace Graphics
{
	export class RenderContext : public Lib::Unique
	{
	public:
		RenderContext(
			const VK::GlobalProc &globalProc,
			const VkInstanceCreateInfo &createInfo) noexcept;

		virtual ~RenderContext() noexcept override;

		[[nodiscard]]
		constexpr const std::vector<PhysicalDevice> &getPhysicalDevices() const noexcept;

		[[nodiscard]]
		std::unique_ptr<DebugMessenger> createDebugMessenger(
			const VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageType,
			const PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback,
			void *const pUserData);

	private:
		const VK::GlobalProc &__globalProc;
		
		VkInstance __handle{ };
		VK::InstanceProc __instanceProc;

		std::vector<PhysicalDevice> __physicalDevices;

		void __create(const VkInstanceCreateInfo &createInfo);
		void __loadProc() noexcept;
		void __resolvePhysicalDevices() noexcept;
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
	RenderContext::RenderContext(
		const VK::GlobalProc &globalProc,
		const VkInstanceCreateInfo &createInfo) noexcept :
		__globalProc{ globalProc }
	{
		__create(createInfo);
		__loadProc();
		__resolvePhysicalDevices();
	}

	RenderContext::~RenderContext() noexcept
	{
		__instanceProc.vkDestroyInstance(__handle, nullptr);
	}

	std::unique_ptr<DebugMessenger> RenderContext::createDebugMessenger(
		const VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
		const VkDebugUtilsMessageTypeFlagsEXT messageType,
		const PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback,
		void *const pUserData)
	{
		const DebugMessenger::CreateInfo createInfo
		{
			.pInstanceProc		{ &__instanceProc },
			.hInstance			{ __handle },
			.messageSeverity	{ messageSeverity },
			.messageType		{ messageType },
			.pfnUserCallback	{ pfnUserCallback },
			.pUserData			{ pUserData }
		};

		return std::make_unique<DebugMessenger>(createInfo);
	}

	void RenderContext::__create(const VkInstanceCreateInfo &createInfo)
	{
		__globalProc.vkCreateInstance(&createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create RenderContext." };
	}

	void RenderContext::__loadProc() noexcept
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

	void RenderContext::__resolvePhysicalDevices() noexcept
	{
		std::vector<VkPhysicalDevice> deviceHandles;

		uint32_t deviceCount{ };
		__instanceProc.vkEnumeratePhysicalDevices(__handle, &deviceCount, nullptr);

		deviceHandles.resize(deviceCount);
		__instanceProc.vkEnumeratePhysicalDevices(__handle, &deviceCount, deviceHandles.data());

		for (const auto handle : deviceHandles)
			__physicalDevices.emplace_back(__instanceProc, __handle, handle);
	}
}
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
		std::unique_ptr<DebugMessenger> createDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT &createInfo);

	private:
		const VK::GlobalProc &__globalProc;
		
		VkInstance __handle{ };
		VK::InstanceProc __proc;

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
		__proc.vkDestroyInstance(__handle, nullptr);
	}

	std::unique_ptr<DebugMessenger> RenderContext::createDebugMessenger(const VkDebugUtilsMessengerCreateInfoEXT &createInfo)
	{
		return std::make_unique<DebugMessenger>(__proc, __handle, createInfo);
	}

	void RenderContext::__create(const VkInstanceCreateInfo &createInfo)
	{
		__globalProc.vkCreateInstance(&createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create RenderContext." };

	}

	void RenderContext::__loadProc() noexcept
	{
		__proc.vkDestroyInstance =
			reinterpret_cast<PFN_vkDestroyInstance>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkDestroyInstance"));

		__proc.vkCreateDebugUtilsMessengerEXT =
			reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkCreateDebugUtilsMessengerEXT"));

		__proc.vkDestroyDebugUtilsMessengerEXT =
			reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkDestroyDebugUtilsMessengerEXT"));

		__proc.vkEnumeratePhysicalDevices =
			reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkEnumeratePhysicalDevices"));

		__proc.vkGetPhysicalDeviceProperties2 =
			reinterpret_cast<PFN_vkGetPhysicalDeviceProperties2>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkGetPhysicalDeviceProperties2"));

		__proc.vkGetPhysicalDeviceFeatures2 =
			reinterpret_cast<PFN_vkGetPhysicalDeviceFeatures2>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkGetPhysicalDeviceFeatures2"));

		__proc.vkEnumerateDeviceExtensionProperties =
			reinterpret_cast<PFN_vkEnumerateDeviceExtensionProperties>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkEnumerateDeviceExtensionProperties"));

		__proc.vkGetPhysicalDeviceQueueFamilyProperties2 =
			reinterpret_cast<PFN_vkGetPhysicalDeviceQueueFamilyProperties2>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkGetPhysicalDeviceQueueFamilyProperties2"));

		__proc.vkGetPhysicalDeviceSurfaceSupportKHR =
			reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceSupportKHR>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkGetPhysicalDeviceSurfaceSupportKHR"));
		
		__proc.vkGetPhysicalDeviceWin32PresentationSupportKHR =
			reinterpret_cast<PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkGetPhysicalDeviceWin32PresentationSupportKHR"));

		__proc.vkCreateWin32SurfaceKHR =
			reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkCreateWin32SurfaceKHR"));

		__proc.vkDestroySurfaceKHR =
			reinterpret_cast<PFN_vkDestroySurfaceKHR>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkDestroySurfaceKHR"));

		__proc.vkCreateDevice =
			reinterpret_cast<PFN_vkCreateDevice>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkCreateDevice"));

		__proc.vkGetDeviceProcAddr =
			reinterpret_cast<PFN_vkGetDeviceProcAddr>(
				__globalProc.vkGetInstanceProcAddr(__handle, "vkGetDeviceProcAddr"));
	}

	void RenderContext::__resolvePhysicalDevices() noexcept
	{
		std::vector<VkPhysicalDevice> deviceHandles;

		uint32_t deviceCount{ };
		__proc.vkEnumeratePhysicalDevices(__handle, &deviceCount, nullptr);

		deviceHandles.resize(deviceCount);
		__proc.vkEnumeratePhysicalDevices(__handle, &deviceCount, deviceHandles.data());

		for (const auto handle : deviceHandles)
			__physicalDevices.emplace_back(__proc, __handle, handle);
	}
}
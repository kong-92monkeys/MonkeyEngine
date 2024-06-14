module;

#include "Vulkan.h"

export module ntmonkeys.com.VK.VulkanProc;

namespace VK
{
	export struct GlobalProc
	{
	public:
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{ };
		PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion{ };
		PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties{ };
		PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties{ };
		PFN_vkCreateInstance vkCreateInstance{ };
	};

	export struct InstanceProc
	{
	public:
		// Instance
		PFN_vkDestroyInstance vkDestroyInstance{ };
		PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT{ };
		PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT{ };
		PFN_vkEnumeratePhysicalDevices vkEnumeratePhysicalDevices{ };

		// Physical device
		PFN_vkGetPhysicalDeviceProperties2 vkGetPhysicalDeviceProperties2{ };
		PFN_vkGetPhysicalDeviceFeatures2 vkGetPhysicalDeviceFeatures2{ };
		PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties{ };
		PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2{ };
		PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR{ };

		// Surface
		PFN_vkCreateWin32SurfaceKHR vkCreateWin32SurfaceKHR{ };
		PFN_vkDestroySurfaceKHR vkDestroySurfaceKHR{ };

		// Device
		PFN_vkCreateDevice vkCreateDevice{ };
		PFN_vkGetDeviceProcAddr vkGetDeviceProcAddr{ };
	};

	export struct DeviceProc
	{
	public:
		// Device
		PFN_vkDeviceWaitIdle vkDeviceWaitIdle{ };
		PFN_vkDestroyDevice vkDestroyDevice{ };

		// Queue
		PFN_vkGetDeviceQueue2 vkGetDeviceQueue2{ };
		PFN_vkQueueWaitIdle vkQueueWaitIdle{ };
		PFN_vkQueueSubmit2 vkQueueSubmit2{ };
		PFN_vkQueuePresentKHR vkQueuePresentKHR{ };

		// Pipeline cache
		PFN_vkCreatePipelineCache vkCreatePipelineCache{ };
		PFN_vkDestroyPipelineCache vkDestroyPipelineCache{ };
	};
}

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
	};
}

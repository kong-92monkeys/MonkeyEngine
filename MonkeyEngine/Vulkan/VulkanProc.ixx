module;

#include "Vulkan.h"

export module ntmonkeys.com.VK.VulkanProc;

namespace VK
{
	export class GlobalProc
	{
	public:
		PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr{ };
		PFN_vkEnumerateInstanceVersion vkEnumerateInstanceVersion{ };
		PFN_vkEnumerateInstanceExtensionProperties vkEnumerateInstanceExtensionProperties{ };
		PFN_vkEnumerateInstanceLayerProperties vkEnumerateInstanceLayerProperties{ };
		PFN_vkCreateInstance vkCreateInstance{ };
	};
}

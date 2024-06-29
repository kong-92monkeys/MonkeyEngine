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
		PFN_vkGetPhysicalDeviceFormatProperties2 vkGetPhysicalDeviceFormatProperties2{ };
		PFN_vkEnumerateDeviceExtensionProperties vkEnumerateDeviceExtensionProperties{ };
		PFN_vkGetPhysicalDeviceQueueFamilyProperties2 vkGetPhysicalDeviceQueueFamilyProperties2{ };
		PFN_vkGetPhysicalDeviceSurfaceSupportKHR vkGetPhysicalDeviceSurfaceSupportKHR{ };
		PFN_vkGetPhysicalDeviceWin32PresentationSupportKHR vkGetPhysicalDeviceWin32PresentationSupportKHR{ };

		// Physical device - Surface
		PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR vkGetPhysicalDeviceSurfaceCapabilities2KHR{ };
		PFN_vkGetPhysicalDeviceSurfaceFormats2KHR vkGetPhysicalDeviceSurfaceFormats2KHR{ };
		PFN_vkGetPhysicalDeviceSurfacePresentModesKHR vkGetPhysicalDeviceSurfacePresentModesKHR{ };
		PFN_vkGetPhysicalDeviceSurfacePresentModes2EXT vkGetPhysicalDeviceSurfacePresentModes2EXT{ };
		
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

		// Descriptor set layout
		PFN_vkCreateDescriptorSetLayout vkCreateDescriptorSetLayout{ };
		PFN_vkDestroyDescriptorSetLayout vkDestroyDescriptorSetLayout{ };

		// Shader module
		PFN_vkCreateShaderModule vkCreateShaderModule{ };
		PFN_vkDestroyShaderModule vkDestroyShaderModule{ };

		// Render pass
		PFN_vkCreateRenderPass2 vkCreateRenderPass2{ };
		PFN_vkDestroyRenderPass vkDestroyRenderPass{ };

		// Pipeline layout
		PFN_vkCreatePipelineLayout vkCreatePipelineLayout{ };
		PFN_vkDestroyPipelineLayout vkDestroyPipelineLayout{ };

		// Pipeline cache
		PFN_vkCreatePipelineCache vkCreatePipelineCache{ };
		PFN_vkDestroyPipelineCache vkDestroyPipelineCache{ };

		// Pipeline
		PFN_vkCreateGraphicsPipelines vkCreateGraphicsPipelines{ };
		PFN_vkDestroyPipeline vkDestroyPipeline{ };

		// Swapchain
		PFN_vkCreateSwapchainKHR vkCreateSwapchainKHR{ };
		PFN_vkDestroySwapchainKHR vkDestroySwapchainKHR{ };
		PFN_vkGetSwapchainImagesKHR vkGetSwapchainImagesKHR{ };
		PFN_vkAcquireNextImage2KHR vkAcquireNextImage2KHR{ };

		// ImageView
		PFN_vkCreateImageView vkCreateImageView{ };
		PFN_vkDestroyImageView vkDestroyImageView{ };

		// Framebuffer
		PFN_vkCreateFramebuffer vkCreateFramebuffer{ };
		PFN_vkDestroyFramebuffer vkDestroyFramebuffer{ };

		// Semaphore
		PFN_vkCreateSemaphore vkCreateSemaphore{ };
		PFN_vkDestroySemaphore vkDestroySemaphore{ };

		// Fence
		PFN_vkCreateFence vkCreateFence{ };
		PFN_vkDestroyFence vkDestroyFence{ };
		PFN_vkWaitForFences vkWaitForFences{ };
		PFN_vkResetFences vkResetFences{ };

		// Command pool
		PFN_vkCreateCommandPool vkCreateCommandPool{ };
		PFN_vkDestroyCommandPool vkDestroyCommandPool{ };
		PFN_vkAllocateCommandBuffers vkAllocateCommandBuffers{ };
		PFN_vkResetCommandPool vkResetCommandPool{ };

		// Command buffer
		PFN_vkBeginCommandBuffer vkBeginCommandBuffer{ };
		PFN_vkEndCommandBuffer vkEndCommandBuffer{ };
		PFN_vkCmdBeginRenderPass2 vkCmdBeginRenderPass2{ };
		PFN_vkCmdEndRenderPass2 vkCmdEndRenderPass2{ };
		PFN_vkCmdBindPipeline vkCmdBindPipeline{ };
		PFN_vkCmdSetViewport vkCmdSetViewport{ };
		PFN_vkCmdSetScissor vkCmdSetScissor{ };
		PFN_vkCmdDraw vkCmdDraw{ };
		PFN_vkCmdDrawIndexed vkCmdDrawIndexed{ };
	};
}
module;

#include "Vulkan.h"

export module ntmonkeys.com.VK.VulkanProc;

#pragma warning(disable: 5103)
#define DECLARE_PROC(funcName) PFN_##funcName funcName{ }

namespace VK
{
	export struct GlobalProc
	{
	public:
		DECLARE_PROC(vkGetInstanceProcAddr);
		DECLARE_PROC(vkEnumerateInstanceVersion);
		DECLARE_PROC(vkEnumerateInstanceExtensionProperties);
		DECLARE_PROC(vkEnumerateInstanceLayerProperties);
		DECLARE_PROC(vkCreateInstance);
	};

	export struct InstanceProc
	{
	public:
		// Instance
		DECLARE_PROC(vkDestroyInstance);
		DECLARE_PROC(vkCreateDebugUtilsMessengerEXT);
		DECLARE_PROC(vkDestroyDebugUtilsMessengerEXT);
		DECLARE_PROC(vkEnumeratePhysicalDevices);

		// Physical device
		DECLARE_PROC(vkGetPhysicalDeviceProperties2);
		DECLARE_PROC(vkGetPhysicalDeviceFeatures2);
		DECLARE_PROC(vkGetPhysicalDeviceFormatProperties2);
		DECLARE_PROC(vkEnumerateDeviceExtensionProperties);
		DECLARE_PROC(vkGetPhysicalDeviceMemoryProperties2);
		DECLARE_PROC(vkGetPhysicalDeviceQueueFamilyProperties2);
		DECLARE_PROC(vkGetPhysicalDeviceSurfaceSupportKHR);
		DECLARE_PROC(vkGetPhysicalDeviceWin32PresentationSupportKHR);

		// Physical device - Surface
		DECLARE_PROC(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
		DECLARE_PROC(vkGetPhysicalDeviceSurfaceFormats2KHR);
		DECLARE_PROC(vkGetPhysicalDeviceSurfacePresentModesKHR);
		DECLARE_PROC(vkGetPhysicalDeviceSurfacePresentModes2EXT);

		// Surface
		DECLARE_PROC(vkCreateWin32SurfaceKHR);
		DECLARE_PROC(vkDestroySurfaceKHR);

		// Device
		DECLARE_PROC(vkCreateDevice);
		DECLARE_PROC(vkGetDeviceProcAddr);
	};

	export struct DeviceProc
	{
	public:
		// Device
		DECLARE_PROC(vkDeviceWaitIdle);
		DECLARE_PROC(vkDestroyDevice);

		// Queue
		DECLARE_PROC(vkGetDeviceQueue2);
		DECLARE_PROC(vkQueueWaitIdle);
		DECLARE_PROC(vkQueueSubmit2);
		DECLARE_PROC(vkQueuePresentKHR);

		// Descriptor set layout
		DECLARE_PROC(vkCreateDescriptorSetLayout);
		DECLARE_PROC(vkDestroyDescriptorSetLayout);

		// Descriptor pool
		DECLARE_PROC(vkCreateDescriptorPool);
		DECLARE_PROC(vkDestroyDescriptorPool);
		DECLARE_PROC(vkResetDescriptorPool);

		// Descriptor set
		DECLARE_PROC(vkAllocateDescriptorSets);
		DECLARE_PROC(vkFreeDescriptorSets);
		DECLARE_PROC(vkUpdateDescriptorSets);

		// Shader module
		DECLARE_PROC(vkCreateShaderModule);
		DECLARE_PROC(vkDestroyShaderModule);

		// Render pass
		DECLARE_PROC(vkCreateRenderPass2);
		DECLARE_PROC(vkDestroyRenderPass);

		// Pipeline layout
		DECLARE_PROC(vkCreatePipelineLayout);
		DECLARE_PROC(vkDestroyPipelineLayout);

		// Pipeline cache
		DECLARE_PROC(vkCreatePipelineCache);
		DECLARE_PROC(vkDestroyPipelineCache);

		// Pipeline
		DECLARE_PROC(vkCreateGraphicsPipelines);
		DECLARE_PROC(vkDestroyPipeline);

		// Swapchain
		DECLARE_PROC(vkCreateSwapchainKHR);
		DECLARE_PROC(vkDestroySwapchainKHR);
		DECLARE_PROC(vkGetSwapchainImagesKHR);
		DECLARE_PROC(vkAcquireNextImage2KHR);

		// Image
		DECLARE_PROC(vkCreateImage);
		DECLARE_PROC(vkDestroyImage);
		DECLARE_PROC(vkGetImageMemoryRequirements2);
		DECLARE_PROC(vkBindImageMemory2);

		// ImageView
		DECLARE_PROC(vkCreateImageView);
		DECLARE_PROC(vkDestroyImageView);

		// Sampler
		DECLARE_PROC(vkCreateSampler);
		DECLARE_PROC(vkDestroySampler);

		// Framebuffer
		DECLARE_PROC(vkCreateFramebuffer);
		DECLARE_PROC(vkDestroyFramebuffer);

		// Semaphore
		DECLARE_PROC(vkCreateSemaphore);
		DECLARE_PROC(vkDestroySemaphore);

		// Fence
		DECLARE_PROC(vkCreateFence);
		DECLARE_PROC(vkDestroyFence);
		DECLARE_PROC(vkWaitForFences);
		DECLARE_PROC(vkResetFences);

		// Command pool
		DECLARE_PROC(vkCreateCommandPool);
		DECLARE_PROC(vkDestroyCommandPool);
		DECLARE_PROC(vkAllocateCommandBuffers);
		DECLARE_PROC(vkResetCommandPool);

		// Command buffer
		DECLARE_PROC(vkBeginCommandBuffer);
		DECLARE_PROC(vkEndCommandBuffer);
		DECLARE_PROC(vkCmdBeginRenderPass2);
		DECLARE_PROC(vkCmdEndRenderPass2);
		DECLARE_PROC(vkCmdBindPipeline);
		DECLARE_PROC(vkCmdSetViewport);
		DECLARE_PROC(vkCmdSetScissor);
		DECLARE_PROC(vkCmdDraw);
		DECLARE_PROC(vkCmdDrawIndexed);
		DECLARE_PROC(vkCmdCopyBuffer2);
		DECLARE_PROC(vkCmdCopyBufferToImage2);
		DECLARE_PROC(vkCmdPipelineBarrier2);
		DECLARE_PROC(vkCmdBindVertexBuffers);
		DECLARE_PROC(vkCmdBindIndexBuffer);
		DECLARE_PROC(vkCmdBindDescriptorSets);

		// Memory
		DECLARE_PROC(vkAllocateMemory);
		DECLARE_PROC(vkFreeMemory);
		DECLARE_PROC(vkMapMemory);
		DECLARE_PROC(vkUnmapMemory);

		// Buffer
		DECLARE_PROC(vkCreateBuffer);
		DECLARE_PROC(vkDestroyBuffer);
		DECLARE_PROC(vkGetBufferMemoryRequirements2);
		DECLARE_PROC(vkBindBufferMemory2);
	};
}
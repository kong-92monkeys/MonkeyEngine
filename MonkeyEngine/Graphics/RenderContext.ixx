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

#pragma warning(disable: 5103)
#define LOAD_PROC(funcName) (__proc.##funcName = reinterpret_cast<PFN_##funcName>(__globalProc.vkGetInstanceProcAddr(__handle, #funcName)))

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
		// Instance
		LOAD_PROC(vkDestroyInstance);
		LOAD_PROC(vkCreateDebugUtilsMessengerEXT);
		LOAD_PROC(vkDestroyDebugUtilsMessengerEXT);
		LOAD_PROC(vkEnumeratePhysicalDevices);

		// Physical device
		LOAD_PROC(vkGetPhysicalDeviceProperties2);
		LOAD_PROC(vkGetPhysicalDeviceFeatures2);
		LOAD_PROC(vkGetPhysicalDeviceFormatProperties2);
		LOAD_PROC(vkEnumerateDeviceExtensionProperties);
		LOAD_PROC(vkGetPhysicalDeviceQueueFamilyProperties2);
		LOAD_PROC(vkGetPhysicalDeviceSurfaceSupportKHR);
		LOAD_PROC(vkGetPhysicalDeviceWin32PresentationSupportKHR);
		LOAD_PROC(vkGetPhysicalDeviceSurfaceCapabilities2KHR);
		LOAD_PROC(vkGetPhysicalDeviceSurfaceFormats2KHR);
		LOAD_PROC(vkGetPhysicalDeviceSurfacePresentModes2EXT);

		// Surface
		LOAD_PROC(vkCreateWin32SurfaceKHR);
		LOAD_PROC(vkDestroySurfaceKHR);

		// Device
		LOAD_PROC(vkCreateDevice);
		LOAD_PROC(vkGetDeviceProcAddr);
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
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Surface;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class Surface : public Lib::Unique
	{
	public:
		Surface(
			const VK::InstanceProc &proc,
			const VkInstance hInstance,
			const VkPhysicalDevice hPhysicalDevice,
			const uint32_t familyQueueIndex,
			const VkWin32SurfaceCreateInfoKHR &createInfo) noexcept;

		virtual ~Surface() noexcept override;

	private:
		const VK::InstanceProc &__proc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;
		const uint32_t __familyQueueIndex;
		const HWND __hWnd;

		VkSurfaceKHR __handle{ };

		VkPresentModeKHR __presentMode{ };

		void __create(const VkWin32SurfaceCreateInfoKHR &createInfo);
		void __checkDeviceSupport();
		void __resolvePresentMode() noexcept;
		void __resolveProperties() noexcept;
	};
}

module: private;

namespace Graphics
{
	Surface::Surface(
		const VK::InstanceProc &proc,
		const VkInstance hInstance,
		const VkPhysicalDevice hPhysicalDevice,
		const uint32_t familyQueueIndex,
		const VkWin32SurfaceCreateInfoKHR &createInfo) noexcept :
		__proc				{ proc },
		__hInstance			{ hInstance },
		__hPhysicalDevice	{ hPhysicalDevice },
		__familyQueueIndex	{ familyQueueIndex },
		__hWnd				{ createInfo.hwnd }
	{
		__create(createInfo);
		__checkDeviceSupport();
		__resolvePresentMode();
		__resolveProperties();
	}

	Surface::~Surface() noexcept
	{
		__proc.vkDestroySurfaceKHR(__hInstance, __handle, nullptr);
	}

	void Surface::__create(const VkWin32SurfaceCreateInfoKHR &createInfo)
	{
		__proc.vkCreateWin32SurfaceKHR(__hInstance, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Surface." };
	}

	void Surface::__checkDeviceSupport()
	{
		VkBool32 supported{ };
		__proc.vkGetPhysicalDeviceSurfaceSupportKHR(__hPhysicalDevice, __familyQueueIndex, __handle, &supported);

		if (!supported)
			throw std::runtime_error{ "The device doesn't support current surface." };
	}

	void Surface::__resolvePresentMode() noexcept
	{
		uint32_t presentModeCount{ };
		__proc.vkGetPhysicalDeviceSurfacePresentModesKHR(__hPhysicalDevice, __handle, &presentModeCount, nullptr);

		std::vector<VkPresentModeKHR> supportedModes;
		supportedModes.resize(presentModeCount);
		__proc.vkGetPhysicalDeviceSurfacePresentModesKHR(__hPhysicalDevice, __handle, &presentModeCount, supportedModes.data());

		__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
		for (const auto mode : supportedModes)
		{
			if (mode != VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
				continue;

			__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
	}

	void Surface::__resolveProperties() noexcept
	{
		VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenWin32Info
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT },
			.hmonitor	{ MonitorFromWindow(__hWnd, MONITOR_DEFAULTTONEAREST) }
		};

		VkSurfaceFullScreenExclusiveInfoEXT fullscreenInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT },
			.pNext					{ &fullscreenWin32Info },
			.fullScreenExclusive	{ VkFullScreenExclusiveEXT::VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT }
		};

		const VkSurfacePresentModeEXT presentModeInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_EXT },
			.pNext			{ &fullscreenInfo },
			.presentMode	{ __presentMode }
		};

		const VkPhysicalDeviceSurfaceInfo2KHR surfaceInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR },
			.pNext		{ &presentModeInfo },
			.surface	{ __handle }
		};

		VkSurfaceCapabilitiesFullScreenExclusiveEXT fullScreenCapabilities
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT },
		};

		VkSurfaceCapabilities2KHR capabilities
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR },
			.pNext	{ &fullScreenCapabilities }
		};

		__proc.vkGetPhysicalDeviceSurfaceCapabilities2KHR(__hPhysicalDevice, &surfaceInfo, &capabilities);
	}
}
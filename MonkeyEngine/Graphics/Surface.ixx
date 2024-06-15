module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Surface;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.Swapchain;
import <vector>;
import <stdexcept>;
import <memory>;
import <algorithm>;

namespace Graphics
{
	export class Surface : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::InstanceProc *pInstanceProc{ };
			VkInstance hInstance{ };
			VkPhysicalDevice hPhysicalDevice{ };

			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			uint32_t queueFamilyIndex{ };

			HINSTANCE hAppInstance{ };
			HWND hwnd{ };
		};

		Surface(const CreateInfo &createInfo) noexcept;
		virtual ~Surface() noexcept override;

		[[nodiscard]]
		constexpr uint32_t getWidth() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getHeight() const noexcept;

		void sync();

		[[nodiscard]]
		std::unique_ptr<Swapchain> createSwapchain(
			const VkImageUsageFlags imageUsage,
			const bool clipped,
			std::unique_ptr<Swapchain> pOldSwapchain);

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;

		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const uint32_t __queueFamilyIndex;

		const HWND __hwnd;

		VkSurfaceKHR __handle{ };

		VkPresentModeKHR __presentMode{ };

		VkSurfacePresentModeEXT __presentModeInfo{ };
		VkSurfaceFullScreenExclusiveInfoEXT __fullscreenInfo{ };
		VkSurfaceFullScreenExclusiveWin32InfoEXT __fullscreenWin32Info{ };
		VkPhysicalDeviceSurfaceInfo2KHR __surfaceInfo{ };

		VkSurfaceCapabilitiesFullScreenExclusiveEXT __fullScreenCapabilities{ };
		VkSurfaceCapabilities2KHR __capabilities{ };

		VkFormat __format{ };
		VkColorSpaceKHR __colorSpace{ };
		VkCompositeAlphaFlagBitsKHR __compositeAlpha{ };

		void __create(const HINSTANCE hAppInstance);

		void __checkDeviceSupport();
		void __resolvePresentMode() noexcept;
		void __populateQueryInfos();
		void __resolveCapabilities();
		void __resolveFormat();
		void __resolveCompositeAlpha() noexcept;
	};

	constexpr uint32_t Surface::getWidth() const noexcept
	{
		const auto &currentExtent{ __capabilities.surfaceCapabilities.currentExtent };
		return currentExtent.width;
	}

	constexpr uint32_t Surface::getHeight() const noexcept
	{
		const auto &currentExtent{ __capabilities.surfaceCapabilities.currentExtent };
		return currentExtent.height;
	}
}

module: private;

namespace Graphics
{
	Surface::Surface(const CreateInfo &createInfo) noexcept :
		__instanceProc		{ *(createInfo.pInstanceProc) },
		__hInstance			{ createInfo.hInstance },
		__hPhysicalDevice	{ createInfo.hPhysicalDevice },

		__deviceProc		{ *(createInfo.pDeviceProc) },
		__hDevice			{ createInfo.hDevice },
		__queueFamilyIndex	{ createInfo.queueFamilyIndex },
		__hwnd				{ createInfo.hwnd }
	{
		__create(createInfo.hAppInstance);
		sync();
	}

	Surface::~Surface() noexcept
	{
		__instanceProc.vkDestroySurfaceKHR(__hInstance, __handle, nullptr);
	}

	void Surface::sync()
	{
		__checkDeviceSupport();
		__resolvePresentMode();
		__populateQueryInfos();
		__resolveCapabilities();
		__resolveFormat();
		__resolveCompositeAlpha();
	}

	std::unique_ptr<Swapchain> Surface::createSwapchain(
		const VkImageUsageFlags imageUsage,
		const bool clipped,
		std::unique_ptr<Swapchain> pOldSwapchain)
	{
		const auto &capabilities{ __capabilities.surfaceCapabilities };

		const Swapchain::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __hDevice },
			.hSurface			{ __handle },
			.minImageCount		{ std::clamp(3U, capabilities.minImageCount, capabilities.maxImageCount) },
			.imageFormat		{ __format },
			.imageColorSpace	{ __colorSpace },
			.imageExtent		{ capabilities.currentExtent },
			.imageUsage			{ imageUsage },
			.preTransform		{ capabilities.currentTransform },
			.compositeAlpha		{ __compositeAlpha },
			.presentMode		{ __presentMode },
			.clipped			{ clipped ? VK_TRUE : VK_FALSE },
			.pOldSwapchain		{ std::move(pOldSwapchain) },
			.fullScreenMode		{ __fullscreenInfo.fullScreenExclusive },
			.fullScreenMonitor	{ __fullscreenWin32Info.hmonitor }
		};

		return std::make_unique<Swapchain>(createInfo);
	}

	void Surface::__create(const HINSTANCE hAppInstance)
	{
		const VkWin32SurfaceCreateInfoKHR createInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
			.hinstance		{ hAppInstance },
			.hwnd			{ __hwnd }
		};

		__instanceProc.vkCreateWin32SurfaceKHR(__hInstance, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Surface." };
	}

	void Surface::__checkDeviceSupport()
	{
		VkBool32 supported{ };
		__instanceProc.vkGetPhysicalDeviceSurfaceSupportKHR(__hPhysicalDevice, __queueFamilyIndex, __handle, &supported);

		if (!supported)
			throw std::runtime_error{ "The device doesn't support current surface." };
	}

	void Surface::__resolvePresentMode() noexcept
	{
		uint32_t presentModeCount{ };
		__instanceProc.vkGetPhysicalDeviceSurfacePresentModesKHR(__hPhysicalDevice, __handle, &presentModeCount, nullptr);

		std::vector<VkPresentModeKHR> supportedModes;
		supportedModes.resize(presentModeCount);
		__instanceProc.vkGetPhysicalDeviceSurfacePresentModesKHR(__hPhysicalDevice, __handle, &presentModeCount, supportedModes.data());

		__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_FIFO_KHR;
		for (const auto mode : supportedModes)
		{
			if (mode != VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR)
				continue;

			__presentMode = VkPresentModeKHR::VK_PRESENT_MODE_MAILBOX_KHR;
			break;
		}
	}

	void Surface::__populateQueryInfos()
	{
		const auto hMonitor{ MonitorFromWindow(__hwnd, MONITOR_DEFAULTTONEAREST) };
		if (!hMonitor)
			throw std::runtime_error{ "Cannot find the monitor handle." };

		__presentModeInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_SURFACE_PRESENT_MODE_EXT;
		__presentModeInfo.pNext					= &__fullscreenInfo;
		__presentModeInfo.presentMode			= __presentMode;

		__fullscreenInfo.sType					= VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT;
		__fullscreenInfo.pNext					= &__fullscreenWin32Info;
		__fullscreenInfo.fullScreenExclusive	= VkFullScreenExclusiveEXT::VK_FULL_SCREEN_EXCLUSIVE_APPLICATION_CONTROLLED_EXT;

		__fullscreenWin32Info.sType				= VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT;
		__fullscreenWin32Info.pNext				= nullptr;
		__fullscreenWin32Info.hmonitor			= hMonitor;
		
		__surfaceInfo.sType						= VkStructureType::VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
		__surfaceInfo.pNext						= &__presentModeInfo;
		__surfaceInfo.surface					= __handle;
	}

	void Surface::__resolveCapabilities()
	{
		__fullScreenCapabilities.sType = VkStructureType::VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_FULL_SCREEN_EXCLUSIVE_EXT;

		__capabilities.sType = VkStructureType::VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR ;
		__capabilities.pNext = &__fullScreenCapabilities;

		__instanceProc.vkGetPhysicalDeviceSurfaceCapabilities2KHR(__hPhysicalDevice, &__surfaceInfo, &__capabilities);
		
		if (!(__fullScreenCapabilities.fullScreenExclusiveSupported))
			throw std::runtime_error{ "Fullscreen mode is not supported on the device." };
	}

	void Surface::__resolveFormat()
	{
		uint32_t formatCount{ };
		__instanceProc.vkGetPhysicalDeviceSurfaceFormats2KHR(__hPhysicalDevice, &__surfaceInfo, &formatCount, nullptr);

		std::vector<VkSurfaceFormat2KHR> formats;
		formats.resize(formatCount);

		for (auto &format : formats)
			format.sType = VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FORMAT_2_KHR;

		__instanceProc.vkGetPhysicalDeviceSurfaceFormats2KHR(__hPhysicalDevice, &__surfaceInfo, &formatCount, formats.data());

		__format = VkFormat::VK_FORMAT_UNDEFINED;
		for (const auto &format : formats)
		{
			const auto &surfaceFormat{ format.surfaceFormat };
			if (
				(surfaceFormat.format == VkFormat::VK_FORMAT_R8G8B8A8_SRGB) &&
				(surfaceFormat.colorSpace == VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR))
			{
				__format = VkFormat::VK_FORMAT_R8G8B8A8_SRGB;
				__colorSpace = VkColorSpaceKHR::VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
				break;
			}
		}

		if (__format == VkFormat::VK_FORMAT_UNDEFINED)
			throw std::runtime_error{ "Cannot resolve suitable surface format." };
	}

	void Surface::__resolveCompositeAlpha() noexcept
	{
		const auto &capabilities{ __capabilities.surfaceCapabilities };

		if (capabilities.supportedCompositeAlpha & VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR)
		{
			__compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
			return;
		}

		if (capabilities.supportedCompositeAlpha & VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR)
		{
			__compositeAlpha = VkCompositeAlphaFlagBitsKHR::VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
			return;
		}
	}
}
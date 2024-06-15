module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Swapchain;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;
import <memory>;

namespace Graphics
{
	export class Swapchain : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkSurfaceKHR hSurface{ };
			uint32_t minImageCount{ };
			VkFormat imageFormat{ };
			VkColorSpaceKHR imageColorSpace{ };
			VkExtent2D imageExtent{ };
			VkImageUsageFlags imageUsage{ };
			VkSurfaceTransformFlagBitsKHR preTransform{ };
			VkCompositeAlphaFlagBitsKHR compositeAlpha{ };
			VkPresentModeKHR presentMode{ };
			VkBool32 clipped{ };
			std::unique_ptr<Swapchain> pOldSwapchain;
			VkFullScreenExclusiveEXT fullScreenMode{ };
			HMONITOR fullScreenMonitor{ };
		};

		Swapchain(const CreateInfo &createInfo) noexcept;

		virtual ~Swapchain() noexcept override;

		[[nodiscard]]
		constexpr VkSwapchainKHR makeOldSwapchain() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkSwapchainKHR __handle{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr VkSwapchainKHR Swapchain::makeOldSwapchain() noexcept
	{
		const VkSwapchainKHR retVal{ __handle };
		__handle = VK_NULL_HANDLE;
		return retVal;
	}
}

module: private;

namespace Graphics
{
	Swapchain::Swapchain(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	Swapchain::~Swapchain() noexcept
	{
		if (!__handle)
			return;

		__deviceProc.vkDestroySwapchainKHR(__hDevice, __handle, nullptr);
	}

	void Swapchain::__create(const CreateInfo &createInfo)
	{
		VkSurfaceFullScreenExclusiveWin32InfoEXT fullscreenWin32Info
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_WIN32_INFO_EXT },
			.hmonitor	{ createInfo.fullScreenMonitor }
		};

		const VkSurfaceFullScreenExclusiveInfoEXT fullscreenInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_SURFACE_FULL_SCREEN_EXCLUSIVE_INFO_EXT },
			.pNext					{ &fullscreenWin32Info },
			.fullScreenExclusive	{ createInfo.fullScreenMode }
		};

		const VkSwapchainCreateInfoKHR vkCreateInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR },
			.pNext					{ &fullscreenInfo },
			.surface				{ createInfo.hSurface },
			.minImageCount			{ createInfo.minImageCount },
			.imageFormat			{ createInfo.imageFormat },
			.imageColorSpace		{ createInfo.imageColorSpace },
			.imageExtent			{ createInfo.imageExtent },
			.imageArrayLayers		{ 1U },
			.imageUsage				{ createInfo.imageUsage },
			.imageSharingMode		{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE },
			.preTransform			{ createInfo.preTransform },
			.compositeAlpha			{ createInfo.compositeAlpha },
			.presentMode			{ createInfo.presentMode },
			.clipped				{ createInfo.clipped },
			.oldSwapchain			{ createInfo.pOldSwapchain ? createInfo.pOldSwapchain->makeOldSwapchain() : VK_NULL_HANDLE }
		};

		__deviceProc.vkCreateSwapchainKHR(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Swapchain." };
	}
}
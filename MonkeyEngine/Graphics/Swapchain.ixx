module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Swapchain;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.Image;
import ntmonkeys.com.Graphics.ImageView;
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
			Swapchain *pOldSwapchain{ };
			VkFullScreenExclusiveEXT fullScreenMode{ };
			HMONITOR fullScreenMonitor{ };
		};

		explicit Swapchain(const CreateInfo &createInfo) noexcept;
		virtual ~Swapchain() noexcept override;

		[[nodiscard]]
		VkSwapchainKHR makeOldSwapchain() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const VkFormat __imageFormat;

		VkSwapchainKHR __handle{ };
		std::vector<std::unique_ptr<Image>> __images;
		std::vector<std::unique_ptr<ImageView>> __imageViews;

		void __create(const CreateInfo &createInfo);
		void __enumerateImages() noexcept;
		void __createImageViews();
	};
}

module: private;

namespace Graphics
{
	Swapchain::Swapchain(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice },
		__imageFormat	{ createInfo.imageFormat }
	{
		__create(createInfo);
		__enumerateImages();
		__createImageViews();
	}

	Swapchain::~Swapchain() noexcept
	{
		if (!__handle)
			return;

		__imageViews.clear();
		__images.clear();
		__deviceProc.vkDestroySwapchainKHR(__hDevice, __handle, nullptr);
	}

	VkSwapchainKHR Swapchain::makeOldSwapchain() noexcept
	{
		const VkSwapchainKHR retVal{ __handle };

		__imageViews.clear();
		__images.clear();
		__handle = VK_NULL_HANDLE;

		return retVal;
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
			.imageFormat			{ __imageFormat },
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

	void Swapchain::__enumerateImages() noexcept
	{
		uint32_t imageCount{ };
		__deviceProc.vkGetSwapchainImagesKHR(__hDevice, __handle, &imageCount, nullptr);

		std::vector<VkImage> imageHandles;
		imageHandles.resize(imageCount);

		__deviceProc.vkGetSwapchainImagesKHR(__hDevice, __handle, &imageCount, imageHandles.data());

		for (const auto handle : imageHandles)
		{
			const Image::PlaceholderInfo placeholderInfo
			{
				.pDeviceProc	{ &__deviceProc },
				.hDevice		{ __hDevice },
				.externalHandle	{ handle },
				.format			{ __imageFormat }
			};

			__images.emplace_back(std::make_unique<Image>(placeholderInfo));
		}
	}

	void Swapchain::__createImageViews()
	{
		for (const auto &pImage : __images)
		{
			static constexpr VkComponentMapping components
			{
				.r{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
				.g{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
				.b{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY },
				.a{ VkComponentSwizzle::VK_COMPONENT_SWIZZLE_IDENTITY }
			};

			static constexpr VkImageSubresourceRange subresourceRange
			{
				.aspectMask		{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
				.baseMipLevel	{ 0U },
				.levelCount		{ 1U },
				.baseArrayLayer	{ 0U },
				.layerCount		{ 1U }
			};

			auto pImageView
			{
				pImage->createImageView(
					VkImageViewType::VK_IMAGE_VIEW_TYPE_2D,
					components, subresourceRange,
					VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
			};

			__imageViews.emplace_back(std::move(pImageView));
		}
	}
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.ImageView;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;

namespace Graphics
{
	export class ImageView : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkImage hImage{ };
			VkImageViewType viewType{ };
			VkFormat format{ };
			VkComponentMapping components{ };
			VkImageSubresourceRange subresourceRange{ };
			VkImageUsageFlags usage{ };
		};

		explicit ImageView(const CreateInfo &createInfo);
		virtual ~ImageView() noexcept override;

		[[nodiscard]]
		constexpr const VkImageView &getHandle() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkImageView __handle{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr const VkImageView &ImageView::getHandle() noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	ImageView::ImageView(const CreateInfo &createInfo) :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	ImageView::~ImageView() noexcept
	{
		__deviceProc.vkDestroyImageView(__hDevice, __handle, nullptr);
	}

	void ImageView::__create(const CreateInfo &createInfo)
	{
		const VkImageViewUsageCreateInfo usageInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO },
			.usage	{ createInfo.usage }
		};

		const VkImageViewCreateInfo vkCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO },
			.pNext				{ &usageInfo },
			.image				{ createInfo.hImage },
			.viewType			{ createInfo.viewType },
			.format				{ createInfo.format },
			.components			{ createInfo.components },
			.subresourceRange	{ createInfo.subresourceRange }
		};

		__deviceProc.vkCreateImageView(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a ImageView." };
	}
}
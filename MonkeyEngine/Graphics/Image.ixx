module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Image;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.ImageView;
import <stdexcept>;
import <memory>;

namespace Graphics
{
	export class Image : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			// TODO
		};

		struct PlaceholderInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkImage externalHandle{ };
			VkFormat format{ };
		};

		explicit Image(const PlaceholderInfo &placeholderInfo) noexcept;
		virtual ~Image() noexcept override;

		[[nodiscard]]
		std::unique_ptr<ImageView> createImageView(
			const VkImageViewType viewType,
			const VkComponentMapping &components,
			const VkImageSubresourceRange &subresourceRange,
			const VkImageUsageFlags usage);

		[[nodiscard]]
		constexpr const VkImage &getHandle() const noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const VkFormat __format;

		VkImage __handle{ };
		bool __ownHandle{ };
	};

	constexpr const VkImage &Image::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Image::Image(const PlaceholderInfo &placeholderInfo) noexcept :
		__deviceProc	{ *(placeholderInfo.pDeviceProc) },
		__hDevice		{ placeholderInfo.hDevice },
		__handle		{ placeholderInfo.externalHandle },
		__ownHandle		{ false },
		__format		{ placeholderInfo.format }
	{}

	Image::~Image() noexcept
	{
		if (!__ownHandle)
			return;
	}

	std::unique_ptr<ImageView> Image::createImageView(
		const VkImageViewType viewType,
		const VkComponentMapping &components,
		const VkImageSubresourceRange &subresourceRange,
		const VkImageUsageFlags usage)
	{
		const ImageView::CreateInfo createInfo
		{
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __hDevice },
			.hImage				{ __handle },
			.viewType			{ viewType },
			.format				{ __format },
			.components			{ components },
			.subresourceRange	{ subresourceRange },
			.usage				{ usage }
		};

		return std::make_unique<ImageView>(createInfo);
	}
}
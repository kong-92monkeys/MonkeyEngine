module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Image;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.ImageView;
import <stdexcept>;

namespace Graphics
{
	export class Image : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkImageCreateFlags flags{ };
			VkImageType imageType{ };
			VkFormat format{ };
			VkExtent3D extent{ };
			uint32_t mipLevels{ };
			uint32_t arrayLayers{ };
			VkSampleCountFlagBits samples{ };
			VkImageTiling tiling{ };
			VkImageUsageFlags usage{ };
		};

		struct PlaceholderInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkImage externalHandle{ };
			VkFormat format{ };
		};

		explicit Image(const CreateInfo &createInfo) noexcept;
		explicit Image(const PlaceholderInfo &placeholderInfo) noexcept;

		virtual ~Image() noexcept override;

		[[nodiscard]]
		constexpr bool needDedicatedAllocation() const noexcept;

		[[nodiscard]]
		constexpr const VkMemoryRequirements &getMemoryRequirements() const noexcept;

		VkResult bindMemory(const VkDeviceMemory hMemory, const VkDeviceSize offset) noexcept;

		[[nodiscard]]
		ImageView *createImageView(
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

		VkMemoryDedicatedRequirements __memDedicatedReq{ };
		VkMemoryRequirements2 __memReq2{ };

		VkImage __handle{ };
		bool __ownHandle{ };

		void __create(const CreateInfo &createInfo);
		void __resolveMemoryRequirements() noexcept;
	};

	constexpr bool Image::needDedicatedAllocation() const noexcept
	{
		return (__memDedicatedReq.prefersDedicatedAllocation || __memDedicatedReq.requiresDedicatedAllocation);
	}

	constexpr const VkMemoryRequirements &Image::getMemoryRequirements() const noexcept
	{
		return __memReq2.memoryRequirements;
	}

	constexpr const VkImage &Image::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Image::Image(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice },
		__ownHandle		{ true },
		__format		{ createInfo.format }
	{
		__create(createInfo);
		__resolveMemoryRequirements();
	}

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

		__deviceProc.vkDestroyImage(__hDevice, __handle, nullptr);
	}

	VkResult Image::bindMemory(const VkDeviceMemory hMemory, const VkDeviceSize offset) noexcept
	{
		const VkBindImageMemoryInfo bindInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BIND_IMAGE_MEMORY_INFO },
			.image			{ __handle },
			.memory			{ hMemory },
			.memoryOffset	{ offset }
		};

		return __deviceProc.vkBindImageMemory2(__hDevice, 1U, &bindInfo);
	}

	ImageView *Image::createImageView(
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

		return new ImageView{ createInfo };
	}

	void Image::__create(const CreateInfo &createInfo)
	{
		const VkImageCreateInfo vkCreateInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO },
			.flags					{ createInfo.flags },
			.imageType				{ createInfo.imageType },
			.format					{ createInfo.format },
			.extent					{ createInfo.extent },
			.mipLevels				{ createInfo.mipLevels },
			.arrayLayers			{ createInfo.arrayLayers },
			.samples				{ createInfo.samples },
			.tiling					{ createInfo.tiling },
			.usage					{ createInfo.usage },
			.sharingMode			{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE },
			.initialLayout			{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED }
		};

		__deviceProc.vkCreateImage(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Image." };
	}

	void Image::__resolveMemoryRequirements() noexcept
	{
		__memDedicatedReq.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;

		__memReq2.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		__memReq2.pNext = &__memDedicatedReq;

		const VkImageMemoryRequirementsInfo2 resolveInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_REQUIREMENTS_INFO_2 },
			.image	{ __handle }
		};
		
		__deviceProc.vkGetImageMemoryRequirements2(__hDevice, &resolveInfo, &__memReq2);
	}
}
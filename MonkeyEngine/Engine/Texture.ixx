module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Texture;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Graphics.Image;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.MemoryAllocator;
import <memory>;

namespace Engine
{
	export class Texture : public Lib::Unique
	{
	public:
		Texture(
			const EngineContext &context, const VkImageType imageType,
			const VkFormat format, const VkExtent3D &extent, const bool useMipmaps);

		virtual ~Texture() noexcept override;

	private:
		const EngineContext &__context;

		std::shared_ptr<ImageChunk> __pImage;

		void __createImage(
			const VkImageType imageType, const VkFormat format,
			const VkExtent3D &extent, const bool useMipmaps);
	};
}

module: private;

namespace Engine
{
	Texture::Texture(
		const EngineContext &context, const VkImageType imageType,
		const VkFormat format, const VkExtent3D &extent, const bool useMipmaps) :
		__context{ context }
	{
		__createImage(imageType, format, extent, useMipmaps);
	}

	Texture::~Texture() noexcept
	{
		const auto pLazyDeleter{ __context.pLazyDeleter };
		pLazyDeleter->reserve(std::move(__pImage));
	}

	void Texture::__createImage(
		const VkImageType imageType, const VkFormat format,
		const VkExtent3D &extent, const bool useMipmaps)
	{
		// TODO: handle useMipmaps

		const Graphics::LogicalDevice::ImageCreateInfo createInfo
		{
			.flags			{ 0U },
			.imageType		{ imageType },
			.format			{ format },
			.extent			{ extent },
			.mipLevels		{ 1U },
			.arrayLayers	{ 1U },
			.samples		{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.tiling			{ VkImageTiling::VK_IMAGE_TILING_OPTIMAL },
			.usage			{
				VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT |
				VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT
			}
		};

		__pImage = std::shared_ptr<ImageChunk>
		{
			__context.pMemoryAllocator->allocateImage(
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, createInfo)
		};
	}
}
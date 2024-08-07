﻿module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Texture;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Graphics.Image;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.MemoryAllocator;
import <memory>;

namespace Engine
{
	export class Texture : public Lib::Unique
	{
	public:
		struct RegionInfo
		{
		public:
			uint32_t bufferRowLength{ };
			uint32_t bufferImageHeight{ };
			VkImageSubresourceLayers imageSubresource{ };
			VkOffset3D imageOffset{ };
			VkExtent3D imageExtent{ };
		};

		Texture(
			const EngineContext &context, const VkImageType imageType, const VkImageViewType viewType,
			const VkFormat format, const VkExtent3D &extent, const bool useMipmaps);

		virtual ~Texture() noexcept override;

		void update(
			const void *const pData, const size_t size, const RegionInfo &regionInfo,
			const VkPipelineStageFlags2 srcStageMask,
			const VkPipelineStageFlags2 dstStageMask, const VkAccessFlags2 dstAccessMask) noexcept;

		[[nodiscard]]
		const Graphics::ImageView &getImageView() const noexcept;

	private:
		const EngineContext &__context;

		std::shared_ptr<ImageChunk> __pImage;
		std::shared_ptr<Graphics::ImageView> __pImageView;

		void __createImage(
			const VkImageType imageType, const VkFormat format,
			const VkExtent3D &extent, const bool useMipmaps);

		void __createImageView(const VkImageViewType viewType);
	};
}

module: private;

namespace Engine
{
	Texture::Texture(
		const EngineContext &context, const VkImageType imageType, const VkImageViewType viewType,
		const VkFormat format, const VkExtent3D &extent, const bool useMipmaps) :
		__context{ context }
	{
		__createImage(imageType, format, extent, useMipmaps);
		__createImageView(viewType);
	}

	Texture::~Texture() noexcept
	{
		const auto pLazyDeleter{ __context.pLazyDeleter };
		pLazyDeleter->reserve(std::move(__pImage));
		pLazyDeleter->reserve(std::move(__pImageView));
	}

	void Texture::update(
		const void *const pData, const size_t size, const RegionInfo &regionInfo,
		const VkPipelineStageFlags2 srcStageMask,
		const VkPipelineStageFlags2 dstStageMask, const VkAccessFlags2 dstAccessMask) noexcept
	{
		const auto pLazyDeleter			{ __context.pLazyDeleter };
		const auto pMemoryAllocator		{ __context.pMemoryAllocator };
		const auto pCommandExecutor		{ __context.pCommandExecutor };

		const auto pStagingBuffer
		{
			pMemoryAllocator->allocateBuffer(
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
		};

		std::memcpy(pStagingBuffer->getMappedMemory(), pData, size);

		pCommandExecutor->reserve([=, pDst{ __pImage.get() }, pSrc{ pStagingBuffer }](auto &commandBuffer)
		{
			// 주의: 파라미터로 받은 accessMask는 read access만 들어온다고 가정, before barrier에서 입력하지 않음
			const VkImageMemoryBarrier2 beforeMemoryBarrier
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 },
				.srcStageMask			{ srcStageMask },
				.srcAccessMask			{ 0U },
				.dstStageMask			{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.dstAccessMask			{ VK_ACCESS_2_TRANSFER_WRITE_BIT },
				.oldLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED },
				.newLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
				.image					{ pDst->getImage().getHandle() },
				.subresourceRange		{
					.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
					.baseMipLevel		{ 0U },
					.levelCount			{ 1U },
					.baseArrayLayer		{ 0U },
					.layerCount			{ 1U }
				}
			};

			const VkDependencyInfo beforeBarrier
			{
				.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.imageMemoryBarrierCount	{ 1U },
				.pImageMemoryBarriers		{ &beforeMemoryBarrier }
			};

			commandBuffer.pipelineBarrier(beforeBarrier);

			const VkBufferImageCopy2 region
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_IMAGE_COPY_2 },
				.bufferOffset		{ pSrc->getOffset() },

				// Texel 개수 (메모리 사이즈 아님)
				.bufferRowLength	{ regionInfo.bufferRowLength },
				.bufferImageHeight	{ regionInfo.bufferImageHeight },
				.imageSubresource	{ regionInfo.imageSubresource },
				.imageOffset		{ regionInfo.imageOffset },
				.imageExtent		{ regionInfo.imageExtent }
			};

			const VkCopyBufferToImageInfo2 copyInfo
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COPY_BUFFER_TO_IMAGE_INFO_2 },
				.srcBuffer		{ pSrc->getBuffer().getHandle() },
				.dstImage		{ pDst->getImage().getHandle() },
				.dstImageLayout	{ VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
				.regionCount	{ 1U },
				.pRegions		{ &region }
			};

			commandBuffer.copyBufferToImage(copyInfo);

			const VkImageMemoryBarrier2 afterMemoryBarrier
			{
				.sType					{ VkStructureType::VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2 },
				.srcStageMask			{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.srcAccessMask			{ VK_ACCESS_2_TRANSFER_WRITE_BIT },
				.dstStageMask			{ dstStageMask },
				.dstAccessMask			{ dstAccessMask },
				.oldLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
				.newLayout				{ VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL },
				.image					{ pDst->getImage().getHandle() },
				.subresourceRange		{
					.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
					.baseMipLevel		{ 0U },
					.levelCount			{ 1U },
					.baseArrayLayer		{ 0U },
					.layerCount			{ 1U }
				}
			};

			const VkDependencyInfo afterBarrier
			{
				.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.imageMemoryBarrierCount	{ 1U },
				.pImageMemoryBarriers		{ &afterMemoryBarrier }
			};

			commandBuffer.pipelineBarrier(afterBarrier);
		});

		pLazyDeleter->reserve(std::shared_ptr<BufferChunk>{ pStagingBuffer });
	}

	const Graphics::ImageView &Texture::getImageView() const noexcept
	{
		return *__pImageView;
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

	void Texture::__createImageView(const VkImageViewType viewType)
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
			.aspectMask			{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
			.baseMipLevel		{ 0U },
			.levelCount			{ 1U },
			.baseArrayLayer		{ 0U },
			.layerCount			{ 1U }
		};

		__pImageView = std::shared_ptr<Graphics::ImageView>
		{
			__pImage->getImage().createImageView(
				viewType, components, subresourceRange,
				VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT)
		};
	}
}
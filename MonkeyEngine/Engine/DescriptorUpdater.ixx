module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.DescriptorUpdater;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Sampler;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Engine.Texture;
import <list>;
import <vector>;
import <memory>;

namespace Engine
{
	export class DescriptorUpdater : public Lib::Unique
	{
	public:
		DescriptorUpdater(Graphics::LogicalDevice &device) noexcept;

		void reset() noexcept;

		void addBufferInfo(
			const VkDescriptorSet hDescSet, const uint32_t dstBinding,
			const uint32_t dstArrayElement, const uint32_t descriptorCount,
			const VkDescriptorType descriptorType, const BufferChunk *const *const ppBuffers) noexcept;

		void addTextureInfo(
			const VkDescriptorSet hDescSet, const uint32_t dstBinding,
			const uint32_t dstArrayElement, const uint32_t descriptorCount,
			const VkDescriptorType descriptorType, const Texture *const *const ppTextures) noexcept;

		void addSamplerInfo(
			const VkDescriptorSet hDescSet, const uint32_t dstBinding,
			const uint32_t dstArrayElement, const uint32_t descriptorCount,
			const VkDescriptorType descriptorType, const Graphics::Sampler *const *const ppSamplers) noexcept;

		void update();

	private:
		Graphics::LogicalDevice &__device;

		std::vector<VkWriteDescriptorSet> __descWrites;
		std::vector<std::vector<VkDescriptorBufferInfo>> __bufferInfoPlaceholder;
		std::vector<std::vector<VkDescriptorImageInfo>> __imageInfoPlaceholder;
	};
}

module: private;

namespace Engine
{
	DescriptorUpdater::DescriptorUpdater(Graphics::LogicalDevice &device) noexcept :
		__device{ device }
	{}

	void DescriptorUpdater::reset() noexcept
	{
		__descWrites.clear();
		__bufferInfoPlaceholder.clear();
		__imageInfoPlaceholder.clear();
	}

	void DescriptorUpdater::addBufferInfo(
		const VkDescriptorSet hDescSet, const uint32_t dstBinding,
		const uint32_t dstArrayElement, const uint32_t descriptorCount,
		const VkDescriptorType descriptorType, const BufferChunk *const *const ppBuffers) noexcept
	{
		auto &bufferInfos{ __bufferInfoPlaceholder.emplace_back() };

		for (uint32_t bufferIter{ }; bufferIter < descriptorCount; ++bufferIter)
		{
			const auto pBuffer	{ ppBuffers[bufferIter] };
			auto &bufferInfo	{ bufferInfos.emplace_back() };
			bufferInfo.buffer	= pBuffer->getBuffer().getHandle();
			bufferInfo.offset	= pBuffer->getOffset();
			bufferInfo.range	= pBuffer->getSize();
		}
		
		auto &writeInfo				{ __descWrites.emplace_back() };
		writeInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet			= hDescSet;
		writeInfo.dstBinding		= dstBinding;
		writeInfo.dstArrayElement	= dstArrayElement;
		writeInfo.descriptorCount	= descriptorCount;
		writeInfo.descriptorType	= descriptorType;
		writeInfo.pBufferInfo		= bufferInfos.data();
	}

	void DescriptorUpdater::addTextureInfo(
		const VkDescriptorSet hDescSet, const uint32_t dstBinding,
		const uint32_t dstArrayElement, const uint32_t descriptorCount,
		const VkDescriptorType descriptorType, const Texture *const *const ppTextures) noexcept
	{
		auto &imageInfos{ __imageInfoPlaceholder.emplace_back() };

		for (uint32_t imageIter{ }; imageIter < descriptorCount; ++imageIter)
		{
			const auto pTexture		{ ppTextures[imageIter] };
			auto &imageInfo			{ imageInfos.emplace_back() };
			imageInfo.imageView		= pTexture->getImageView().getHandle();
			imageInfo.imageLayout	= VkImageLayout::VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		}
		
		auto &writeInfo				{ __descWrites.emplace_back() };
		writeInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet			= hDescSet;
		writeInfo.dstBinding		= dstBinding;
		writeInfo.dstArrayElement	= dstArrayElement;
		writeInfo.descriptorCount	= descriptorCount;
		writeInfo.descriptorType	= descriptorType;
		writeInfo.pImageInfo		= imageInfos.data();
	}

	void DescriptorUpdater::addSamplerInfo(
		const VkDescriptorSet hDescSet, const uint32_t dstBinding,
		const uint32_t dstArrayElement, const uint32_t descriptorCount,
		const VkDescriptorType descriptorType, const Graphics::Sampler *const *const ppSamplers) noexcept
	{
		auto &imageInfos{ __imageInfoPlaceholder.emplace_back() };

		for (uint32_t samplerIter{ }; samplerIter < descriptorCount; ++samplerIter)
		{
			const auto pSampler		{ ppSamplers[samplerIter] };
			auto &imageInfo			{ imageInfos.emplace_back() };
			imageInfo.sampler		= pSampler->getHandle();
		}
		
		auto &writeInfo				{ __descWrites.emplace_back() };
		writeInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		writeInfo.dstSet			= hDescSet;
		writeInfo.dstBinding		= dstBinding;
		writeInfo.dstArrayElement	= dstArrayElement;
		writeInfo.descriptorCount	= descriptorCount;
		writeInfo.descriptorType	= descriptorType;
		writeInfo.pImageInfo		= imageInfos.data();
	}

	void DescriptorUpdater::update()
	{
		__device.updateDescriptorSets(
			static_cast<uint32_t>(__descWrites.size()), __descWrites.data(), 0U, nullptr);
	}
}
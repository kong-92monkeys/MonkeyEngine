module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.MemoryAllocator;

import :Memory;
export import :Buffer;
export import :Image;
import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.MathUtil;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import <memory>;
import <optional>;
import <unordered_map>;

namespace Engine
{
	export class MemoryAllocator : public Lib::Unique
	{
	public:
		MemoryAllocator(
			const Graphics::PhysicalDevice &physicalDevice, Graphics::LogicalDevice &logicalDevice,
			const size_t memoryBlockSize, const size_t bufferBlockSize,
			const VkDeviceSize uniformBufferAlignment, const VkDeviceSize storageBufferAlignment) noexcept;

		virtual ~MemoryAllocator() noexcept;

		[[nodiscard]]
		BufferChunk *allocateBuffer(
			const VkMemoryPropertyFlags memoryProps,
			const VkDeviceSize size,
			const VkBufferUsageFlags usage) noexcept;

		[[nodiscard]]
		ImageChunk *allocateImage(
			const VkMemoryPropertyFlags memoryProps,
			const Graphics::LogicalDevice::ImageCreateInfo &createInfo) noexcept;

	private:
		const Graphics::PhysicalDevice &__physicalDevice;
		Graphics::LogicalDevice &__logicalDevice;

		const size_t __memoryBlockSize;
		const size_t __bufferBlockSize;

		const VkDeviceSize __uniformBufferAlignment;
		const VkDeviceSize __storageBufferAlignment;

		std::unordered_map<VkMemoryPropertyFlags, std::unordered_map<VkBufferUsageFlags, std::vector<std::unique_ptr<BufferBlock>>>> __bufferBlockMap;

		// <memory type index, memory block>
		std::unordered_map<uint32_t, std::vector<std::unique_ptr<MemoryBlock>>> __bufferMemoryBlockMap;
		std::unordered_map<uint32_t, std::vector<std::unique_ptr<MemoryBlock>>> __imageMemoryBlockMap;

		[[nodiscard]]
		constexpr size_t __resolveBufferAlignmentOf(const VkBufferUsageFlags usage) const noexcept;
	};
}

module: private;

namespace Engine
{
	MemoryAllocator::MemoryAllocator(
		const Graphics::PhysicalDevice &physicalDevice, Graphics::LogicalDevice &logicalDevice,
		const size_t memoryBlockSize, const size_t bufferBlockSize,
		const VkDeviceSize uniformBufferAlignment, const VkDeviceSize storageBufferAlignment) noexcept :
		__physicalDevice			{ physicalDevice },
		__logicalDevice				{ logicalDevice },
		__memoryBlockSize			{ memoryBlockSize },
		__bufferBlockSize			{ bufferBlockSize },
		__uniformBufferAlignment	{ uniformBufferAlignment },
		__storageBufferAlignment	{ storageBufferAlignment }
	{}

	MemoryAllocator::~MemoryAllocator() noexcept
	{
		__bufferBlockMap.clear();
		__bufferMemoryBlockMap.clear();
	}

	BufferChunk *MemoryAllocator::allocateBuffer(
		const VkMemoryPropertyFlags memoryProps,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage) noexcept
	{
		const size_t alignment{ __resolveBufferAlignmentOf(usage) };

		BufferChunk *pRetVal{ };

		auto &bufferBlocks{ __bufferBlockMap[memoryProps][usage] };
		for (const auto &pBufferBlock : bufferBlocks)
		{
			pRetVal = pBufferBlock->allocateChunk(size, alignment);
			if (pRetVal)
				break;
		}

		if (!pRetVal)
		{
			const size_t newBufferBlockSize		{ std::max(__bufferBlockSize, size) };
			auto pNewBufferBlock				{ std::make_unique<BufferBlock>(__logicalDevice, newBufferBlockSize, usage) };

			auto &buffer						{ pNewBufferBlock->getBuffer() };
			const auto &bufferBlockMemReq		{ buffer.getMemoryRequirements() };
			const auto &memoryTypes				{ __physicalDevice.getMemoryProps().memoryTypes };
			
			std::optional<uint32_t> memoryTypeIndex;

			uint32_t memoryTypeIter{ };
			for (uint32_t memoryTypeBits{ bufferBlockMemReq.memoryTypeBits }; memoryTypeBits; memoryTypeBits >>= 1ULL)
			{
				if (memoryTypeBits % 2U)
				{
					const auto &memoryType{ memoryTypes[memoryTypeIter] };
					if ((memoryType.propertyFlags & memoryProps) == memoryProps)
					{
						memoryTypeIndex = memoryTypeIter;
						break;
					}
				}

				++memoryTypeIter;
			}

			if (memoryTypeIndex.has_value())
			{
				if (buffer.needDedicatedAllocation())
				{
					const auto pDedicatedMemory
					{
						__logicalDevice.createMemory(
							bufferBlockMemReq.size, memoryTypeIndex.value(),
							VK_NULL_HANDLE, buffer.getHandle())
					};

					pNewBufferBlock->bindMemory(std::unique_ptr<AbstractMemory>{ new AbstractMemory_Memory{ std::unique_ptr<Graphics::Memory>{ pDedicatedMemory } } });
				}
				else
				{
					MemoryChunk *pNewMemoryChunk{ };

					auto &memoryBlocks{ __bufferMemoryBlockMap[memoryTypeIndex.value()] };
					for (const auto &pBufferMemoryBlock : memoryBlocks)
					{
						pNewMemoryChunk = pBufferMemoryBlock->allocateChunk(bufferBlockMemReq.size, bufferBlockMemReq.alignment);
						if (pNewMemoryChunk)
							break;
					}

					if (!pNewMemoryChunk)
					{
						const size_t newMemoryBlockSize{ std::max(__memoryBlockSize, bufferBlockMemReq.size) };
						auto pNewMemoryBlock{ std::make_unique<MemoryBlock>(__logicalDevice, newMemoryBlockSize, memoryTypeIndex.value()) };

						pNewMemoryChunk = pNewMemoryBlock->allocateChunk(bufferBlockMemReq.size, bufferBlockMemReq.alignment);
						memoryBlocks.emplace_back(std::move(pNewMemoryBlock));
					}

					pNewBufferBlock->bindMemory(std::unique_ptr<AbstractMemory>{ new AbstractMemory_MemoryChunk{ std::unique_ptr<MemoryChunk>{ pNewMemoryChunk } } });
				}

				pRetVal = pNewBufferBlock->allocateChunk(size, alignment);
				bufferBlocks.emplace_back(std::move(pNewBufferBlock));
			}
		}

		return pRetVal;
	}

	ImageChunk *MemoryAllocator::allocateImage(
		const VkMemoryPropertyFlags memoryProps, const Graphics::LogicalDevice::ImageCreateInfo &createInfo) noexcept
	{
		ImageChunk *pRetVal{ new ImageChunk{ __logicalDevice, createInfo } };

		auto &image					{ pRetVal->getImage() };
		const auto &imageMemReq		{ image.getMemoryRequirements() };
		const auto &memoryTypes		{ __physicalDevice.getMemoryProps().memoryTypes };
			
		std::optional<uint32_t> memoryTypeIndex;

		uint32_t memoryTypeIter{ };
		for (uint32_t memoryTypeBits{ imageMemReq.memoryTypeBits }; memoryTypeBits; memoryTypeBits >>= 1ULL)
		{
			if (memoryTypeBits % 2U)
			{
				const auto &memoryType{ memoryTypes[memoryTypeIter] };
				if ((memoryType.propertyFlags & memoryProps) == memoryProps)
				{
					memoryTypeIndex = memoryTypeIter;
					break;
				}
			}

			++memoryTypeIter;
		}

		if (memoryTypeIndex.has_value())
		{
			if (image.needDedicatedAllocation())
			{
				const auto pDedicatedMemory
				{
					__logicalDevice.createMemory(
						imageMemReq.size, memoryTypeIndex.value(),
						image.getHandle(), VK_NULL_HANDLE)
				};

				pRetVal->bindMemory(std::unique_ptr<AbstractMemory>{ new AbstractMemory_Memory{ std::unique_ptr<Graphics::Memory>{ pDedicatedMemory } } });
			}
			else
			{
				MemoryChunk *pNewMemoryChunk{ };

				auto &memoryBlocks{ __imageMemoryBlockMap[memoryTypeIndex.value()] };
				for (const auto &pBufferMemoryBlock : memoryBlocks)
				{
					pNewMemoryChunk = pBufferMemoryBlock->allocateChunk(imageMemReq.size, imageMemReq.alignment);
					if (pNewMemoryChunk)
						break;
				}

				if (!pNewMemoryChunk)
				{
					const size_t newMemoryBlockSize{ std::max(__memoryBlockSize, imageMemReq.size) };
					auto pNewMemoryBlock{ std::make_unique<MemoryBlock>(__logicalDevice, newMemoryBlockSize, memoryTypeIndex.value()) };

					pNewMemoryChunk = pNewMemoryBlock->allocateChunk(imageMemReq.size, imageMemReq.alignment);
					memoryBlocks.emplace_back(std::move(pNewMemoryBlock));
				}

				pRetVal->bindMemory(std::unique_ptr<AbstractMemory>{ new AbstractMemory_MemoryChunk{ std::unique_ptr<MemoryChunk>{ pNewMemoryChunk } } });
			}
		}
		else
		{
			delete pRetVal;
			pRetVal = nullptr;
		}

		return pRetVal;
	}

	constexpr size_t MemoryAllocator::__resolveBufferAlignmentOf(const VkBufferUsageFlags usage) const noexcept
	{
		size_t alignment{ 1ULL };

		if (usage & VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
			alignment = Lib::MathUtil::getLCMOf(alignment, __uniformBufferAlignment);

		if (usage & VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
			alignment = Lib::MathUtil::getLCMOf(alignment, __storageBufferAlignment);

		return alignment;
	}
}
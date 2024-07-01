module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.MemoryAllocator;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.MathUtil;
import ntmonkeys.com.Lib.RegionAllocator;
import ntmonkeys.com.Graphics.Memory;
import ntmonkeys.com.Graphics.Buffer;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import <memory>;
import <unordered_map>;
import <optional>;

namespace Engine
{
	class MemoryChunk : public Lib::Unique
	{
	public:
		MemoryChunk(Graphics::Memory &memory, std::unique_ptr<Lib::Region> &&pRegion) noexcept;

		[[nodiscard]]
		constexpr Graphics::Memory &getMemory() noexcept;

		[[nodiscard]]
		constexpr size_t getSize() const noexcept;

		[[nodiscard]]
		constexpr size_t getOffset() const noexcept;

	private:
		Graphics::Memory &__memory;
		std::unique_ptr<Lib::Region> __pRegion;
	};

	class MemoryBlock : public Lib::Unique
	{
	public:
		MemoryBlock(
			Graphics::LogicalDevice &device,
			const VkDeviceSize allocationSize,
			const uint32_t memoryTypeIndex);

		[[nodiscard]]
		MemoryChunk *allocateChunk(const size_t size, const size_t alignment) noexcept;

	private:
		Lib::RegionAllocator __regionAllocator;
		std::unique_ptr<Graphics::Memory> __pMemory;
	};

	export class BufferChunk : public Lib::Unique
	{
	public:
		BufferChunk(Graphics::Buffer &buffer, std::unique_ptr<Lib::Region> &&pRegion) noexcept;

		[[nodiscard]]
		constexpr Graphics::Buffer &getBuffer() noexcept;

		[[nodiscard]]
		constexpr size_t getSize() const noexcept;

		[[nodiscard]]
		constexpr size_t getOffset() const noexcept;

	private:
		Graphics::Buffer &__buffer;
		std::unique_ptr<Lib::Region> __pRegion;
	};

	class BufferBlock : public Lib::Unique
	{
	public:
		BufferBlock(
			Graphics::LogicalDevice &device,
			const VkDeviceSize size,
			const VkBufferUsageFlags usage);

		[[nodiscard]]
		constexpr Graphics::Buffer &getBuffer() noexcept;

		void bindMemory(std::unique_ptr<Graphics::Memory> &&pMemory) noexcept;
		void bindMemory(std::unique_ptr<MemoryChunk> &&pMemory) noexcept;

		[[nodiscard]]
		BufferChunk *allocateChunk(const size_t size, const size_t alignment) noexcept;

	private:
		Lib::RegionAllocator __regionAllocator;
		std::unique_ptr<Graphics::Buffer> __pBuffer;

		std::unique_ptr<Graphics::Memory> __pDedicatedMemory;
		std::unique_ptr<MemoryChunk> __pBoundMemory;
	};

	export class MemoryAllocator : public Lib::Unique
	{
	public:
		MemoryAllocator(
			const Graphics::PhysicalDevice &physicalDevice, Graphics::LogicalDevice &logicalDevice,
			const size_t memoryBlockSize, const size_t bufferBlockSize,
			const VkDeviceSize uniformBufferAlignment, const VkDeviceSize storageBufferAlignment) noexcept;

		virtual ~MemoryAllocator() noexcept = default;

		[[nodiscard]]
		BufferChunk *allocateBuffer(
			const VkMemoryPropertyFlags memoryProps,
			const VkDeviceSize size,
			const VkBufferUsageFlags usage) noexcept;

	private:
		const Graphics::PhysicalDevice &__physicalDevice;
		Graphics::LogicalDevice &__logicalDevice;

		const size_t __memoryBlockSize;
		const size_t __bufferBlockSize;

		const VkDeviceSize __uniformBufferAlignment;
		const VkDeviceSize __storageBufferAlignment;

		std::unordered_map<VkMemoryPropertyFlags, std::unordered_map<VkBufferUsageFlags, std::vector<std::unique_ptr<BufferBlock>>>> __bufferBlockMap;
		std::unordered_map<uint32_t, std::vector<std::unique_ptr<MemoryBlock>>> __bufferMemoryBlockMap;

		[[nodiscard]]
		constexpr size_t __resolveAlignmentOf(const VkBufferUsageFlags usage) const noexcept;
	};

	constexpr Graphics::Memory &MemoryChunk::getMemory() noexcept
	{
		return __memory;
	}

	constexpr size_t MemoryChunk::getSize() const noexcept
	{
		return __pRegion->getSize();
	}

	constexpr size_t MemoryChunk::getOffset() const noexcept
	{
		return __pRegion->getOffset();
	}

	constexpr Graphics::Buffer &BufferChunk::getBuffer() noexcept
	{
		return __buffer;
	}

	constexpr size_t BufferChunk::getSize() const noexcept
	{
		return __pRegion->getSize();
	}

	constexpr size_t BufferChunk::getOffset() const noexcept
	{
		return __pRegion->getOffset();
	}

	constexpr Graphics::Buffer &BufferBlock::getBuffer() noexcept
	{
		return *__pBuffer;
	}
}

module: private;

namespace Engine
{
	MemoryChunk::MemoryChunk(Graphics::Memory &memory, std::unique_ptr<Lib::Region> &&pRegion) noexcept :
		__memory	{ memory },
		__pRegion	{ std::move(pRegion) }
	{}

	MemoryBlock::MemoryBlock(
		Graphics::LogicalDevice &device,
		const VkDeviceSize allocationSize,
		const uint32_t memoryTypeIndex) :
		__regionAllocator{ allocationSize }
	{
		__pMemory = std::unique_ptr<Graphics::Memory>
		{
			device.createMemory(
				allocationSize, memoryTypeIndex,
				VK_NULL_HANDLE, VK_NULL_HANDLE)
		};
	}

	MemoryChunk *MemoryBlock::allocateChunk(const size_t size, const size_t alignment) noexcept
	{
		try
		{
			auto pRegion{ std::make_unique<Lib::Region>(__regionAllocator, size, alignment) };
			return new MemoryChunk{ *__pMemory, std::move(pRegion) };
		}
		catch (...)
		{
			return nullptr;
		}
	}

	BufferChunk::BufferChunk(Graphics::Buffer &buffer, std::unique_ptr<Lib::Region> &&pRegion) noexcept :
		__buffer	{ buffer },
		__pRegion	{ std::move(pRegion) }
	{}

	BufferBlock::BufferBlock(
		Graphics::LogicalDevice &device,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage) :
		__regionAllocator{ size }
	{
		__pBuffer = std::unique_ptr<Graphics::Buffer>{ device.createBuffer(size, usage) };
	}

	void BufferBlock::bindMemory(std::unique_ptr<Graphics::Memory> &&pMemory) noexcept
	{
		__pBuffer->bindMemory(pMemory->getHandle(), 0U);
		__pDedicatedMemory = std::move(pMemory);
	}

	void BufferBlock::bindMemory(std::unique_ptr<MemoryChunk> &&pMemory) noexcept
	{
		__pBuffer->bindMemory(pMemory->getMemory().getHandle(), pMemory->getOffset());
		__pBoundMemory = std::move(pMemory);
	}

	BufferChunk *BufferBlock::allocateChunk(const size_t size, const size_t alignment) noexcept
	{
		try
		{
			auto pRegion{ std::make_unique<Lib::Region>(__regionAllocator, size, alignment) };
			return new BufferChunk{ *__pBuffer, std::move(pRegion) };
		}
		catch (...)
		{
			return nullptr;
		}
	}

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

	BufferChunk *MemoryAllocator::allocateBuffer(
		const VkMemoryPropertyFlags memoryProps,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage) noexcept
	{
		const size_t alignment{ __resolveAlignmentOf(usage) };

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
					std::unique_ptr<Graphics::Memory> pDedicatedMemory
					{
						__logicalDevice.createMemory(
							bufferBlockMemReq.size, memoryTypeIndex.value(),
							VK_NULL_HANDLE, buffer.getHandle())
					};

					pNewBufferBlock->bindMemory(std::move(pDedicatedMemory));
				}
				else
				{
					MemoryChunk *pNewBufferMemoryChunk{ };

					auto &memoryBlocks{ __bufferMemoryBlockMap[memoryTypeIndex.value()] };
					for (const auto &pBufferMemoryBlock : memoryBlocks)
					{
						pNewBufferMemoryChunk = pBufferMemoryBlock->allocateChunk(bufferBlockMemReq.size, bufferBlockMemReq.alignment);
						if (pNewBufferMemoryChunk)
							break;
					}

					if (!pNewBufferMemoryChunk)
					{
						const size_t newMemoryBlockSize{ std::max(__memoryBlockSize, bufferBlockMemReq.size) };
						auto pNewBufferMemoryBlock{ std::make_unique<MemoryBlock>(__logicalDevice, newMemoryBlockSize, memoryTypeIndex.value()) };

						pNewBufferMemoryChunk = pNewBufferMemoryBlock->allocateChunk(bufferBlockMemReq.size, bufferBlockMemReq.alignment);
						memoryBlocks.emplace_back(std::move(pNewBufferMemoryBlock));
					}

					pNewBufferBlock->bindMemory(std::unique_ptr<MemoryChunk>{ pNewBufferMemoryChunk });
				}

				pRetVal = pNewBufferBlock->allocateChunk(size, alignment);
				bufferBlocks.emplace_back(std::move(pNewBufferBlock));
			}
		}

		return pRetVal;
	}

	constexpr size_t MemoryAllocator::__resolveAlignmentOf(const VkBufferUsageFlags usage) const noexcept
	{
		size_t alignment{ 1ULL };

		if (usage & VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT)
			alignment = Lib::MathUtil::getLCMOf(alignment, __uniformBufferAlignment);

		if (usage & VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
			alignment = Lib::MathUtil::getLCMOf(alignment, __storageBufferAlignment);

		return alignment;
	}
}
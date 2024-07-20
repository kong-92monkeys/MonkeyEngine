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
import <cstddef>;
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

	class AbstractMemory : public Lib::Unique
	{
	public:
		[[nodiscard]]
		virtual Graphics::Memory &getMemory() noexcept = 0;

		[[nodiscard]]
		virtual size_t getSize() const noexcept = 0;

		[[nodiscard]]
		virtual size_t getOffset() const noexcept = 0;

		[[nodiscard]]
		void *getMappedMemory() noexcept;
	};

	class AbstractMemory_Memory : public AbstractMemory
	{
	public:
		AbstractMemory_Memory(std::unique_ptr<Graphics::Memory> &&pMemory) noexcept;

		[[nodiscard]]
		virtual Graphics::Memory &getMemory() noexcept override;

		[[nodiscard]]
		virtual size_t getSize() const noexcept override;

		[[nodiscard]]
		virtual size_t getOffset() const noexcept override;

	private:
		std::unique_ptr<Graphics::Memory> __pMemory;
	};

	class AbstractMemory_MemoryChunk : public AbstractMemory
	{
	public:
		AbstractMemory_MemoryChunk(std::unique_ptr<MemoryChunk> &&pMemoryChunk) noexcept;

		[[nodiscard]]
		virtual Graphics::Memory &getMemory() noexcept override;

		[[nodiscard]]
		virtual size_t getSize() const noexcept override;

		[[nodiscard]]
		virtual size_t getOffset() const noexcept override;

	private:
		std::unique_ptr<MemoryChunk> __pMemoryChunk;
	};

	export class BufferChunk : public Lib::Unique
	{
	public:
		BufferChunk(const Graphics::Buffer &buffer, AbstractMemory &memory, std::unique_ptr<Lib::Region> &&pRegion) noexcept;

		[[nodiscard]]
		constexpr const Graphics::Buffer &getBuffer() const noexcept;

		[[nodiscard]]
		constexpr size_t getSize() const noexcept;

		[[nodiscard]]
		constexpr size_t getOffset() const noexcept;

		[[nodiscard]]
		void *getMappedMemory() noexcept;

	private:
		const Graphics::Buffer &__buffer;
		AbstractMemory &__memory;
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

		void bindMemory(std::unique_ptr<AbstractMemory> &&pMemory) noexcept;

		[[nodiscard]]
		BufferChunk *allocateChunk(const size_t size, const size_t alignment) noexcept;

	private:
		Lib::RegionAllocator __regionAllocator;
		std::unique_ptr<Graphics::Buffer> __pBuffer;
		std::unique_ptr<AbstractMemory> __pBoundMemory;
	};

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

	constexpr const Graphics::Buffer &BufferChunk::getBuffer() const noexcept
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

	void *AbstractMemory::getMappedMemory() noexcept
	{
		const auto pMappedMemory{ static_cast<std::byte *>(getMemory().getMappedMemory()) };
		return (pMappedMemory + getOffset());
	}

	AbstractMemory_Memory::AbstractMemory_Memory(std::unique_ptr<Graphics::Memory> &&pMemory) noexcept
		:__pMemory{ std::move(pMemory) }
	{}

	Graphics::Memory &AbstractMemory_Memory::getMemory() noexcept
	{
		return *__pMemory;
	}

	size_t AbstractMemory_Memory::getSize() const noexcept
	{
		return __pMemory->getSize();
	}

	size_t AbstractMemory_Memory::getOffset() const noexcept
	{
		return 0ULL;
	}

	AbstractMemory_MemoryChunk::AbstractMemory_MemoryChunk(std::unique_ptr<MemoryChunk> &&pMemoryChunk) noexcept
		:__pMemoryChunk{ std::move(pMemoryChunk) }
	{}

	Graphics::Memory &AbstractMemory_MemoryChunk::getMemory() noexcept
	{
		return __pMemoryChunk->getMemory();
	}

	size_t AbstractMemory_MemoryChunk::getSize() const noexcept
	{
		return __pMemoryChunk->getSize();
	}

	size_t AbstractMemory_MemoryChunk::getOffset() const noexcept
	{
		return __pMemoryChunk->getOffset();
	}

	BufferChunk::BufferChunk(const Graphics::Buffer &buffer, AbstractMemory &memory, std::unique_ptr<Lib::Region> &&pRegion) noexcept :
		__buffer	{ buffer },
		__memory	{ memory },
		__pRegion	{ std::move(pRegion) }
	{}

	void *BufferChunk::getMappedMemory() noexcept
	{
		return (static_cast<std::byte *>(__memory.getMappedMemory()) + getOffset());
	}

	BufferBlock::BufferBlock(
		Graphics::LogicalDevice &device,
		const VkDeviceSize size,
		const VkBufferUsageFlags usage) :
		__regionAllocator{ size }
	{
		__pBuffer = std::unique_ptr<Graphics::Buffer>{ device.createBuffer(size, usage) };
	}

	void BufferBlock::bindMemory(std::unique_ptr<AbstractMemory> &&pMemory) noexcept
	{
		__pBuffer->bindMemory(pMemory->getMemory().getHandle(), pMemory->getOffset());
		__pBoundMemory = std::move(pMemory);
	}

	BufferChunk *BufferBlock::allocateChunk(const size_t size, const size_t alignment) noexcept
	{
		try
		{
			auto pRegion{ std::make_unique<Lib::Region>(__regionAllocator, size, alignment) };
			return new BufferChunk{ *__pBuffer, *__pBoundMemory, std::move(pRegion) };
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

					pNewBufferBlock->bindMemory(std::unique_ptr<AbstractMemory>{ new AbstractMemory_MemoryChunk{ std::unique_ptr<MemoryChunk>{ pNewBufferMemoryChunk } } });
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
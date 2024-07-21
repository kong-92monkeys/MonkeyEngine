module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.MemoryAllocator:Memory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.RegionAllocator;
import ntmonkeys.com.Graphics.Memory;
import ntmonkeys.com.Graphics.LogicalDevice;
import <cstddef>;
import <memory>;

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

	AbstractMemory_Memory::AbstractMemory_Memory(std::unique_ptr<Graphics::Memory> &&pMemory) noexcept :
		__pMemory{ std::move(pMemory) }
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

	AbstractMemory_MemoryChunk::AbstractMemory_MemoryChunk(std::unique_ptr<MemoryChunk> &&pMemoryChunk) noexcept :
		__pMemoryChunk{ std::move(pMemoryChunk) }
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
}
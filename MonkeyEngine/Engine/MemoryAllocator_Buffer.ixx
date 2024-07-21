module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.MemoryAllocator:Buffer;

import :Memory;
import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.RegionAllocator;
import ntmonkeys.com.Graphics.Memory;
import ntmonkeys.com.Graphics.Buffer;
import ntmonkeys.com.Graphics.LogicalDevice;
import <memory>;

namespace Engine
{
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
}
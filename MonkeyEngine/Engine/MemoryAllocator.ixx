module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.MemoryAllocator;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.MathUtil;
import ntmonkeys.com.Graphics.Memory;
import ntmonkeys.com.Graphics.Buffer;
import ntmonkeys.com.Graphics.LogicalDevice;
import <memory>;
import <unordered_map>;

namespace Engine
{
	class MemoryChunk : public Lib::Unique
	{
	public:
	};

	class MemoryBlock : public Lib::Unique
	{
	public:


	private:

	};

	export class BufferChunk : public Lib::Unique
	{
	public:
	};

	class BufferBlock : public Lib::Unique
	{
	public:
		BufferBlock(
			Graphics::LogicalDevice &device,
			const VkDeviceSize size,
			const VkBufferUsageFlags usage);

		[[nodiscard]]
		constexpr const VkMemoryRequirements &getMemoryRequirements() const noexcept;

		[[nodiscard]]
		BufferChunk *allocateChunk(const size_t size, const size_t alignment) noexcept;

	private:
		Graphics::LogicalDevice &__device;
	};

	export class MemoryAllocator : public Lib::Unique
	{
	public:
		MemoryAllocator(
			Graphics::LogicalDevice &device,
			const size_t memoryBlockSize, const size_t bufferBlockSize,
			const VkDeviceSize uniformBufferAlignment, const VkDeviceSize storageBufferAlignment) noexcept;

		virtual ~MemoryAllocator() noexcept = default;

		[[nodiscard]]
		BufferChunk *allocateBuffer(
			const VkMemoryPropertyFlags memoryProps,
			const VkDeviceSize size,
			const VkBufferUsageFlags usage) noexcept;

	private:
		Graphics::LogicalDevice &__device;
		const size_t __memoryBlockSize;
		const size_t __bufferBlockSize;
		const VkDeviceSize __uniformBufferAlignment;
		const VkDeviceSize __storageBufferAlignment;

		std::unordered_map<VkMemoryPropertyFlags, std::unordered_map<VkBufferUsageFlags, std::vector<std::unique_ptr<BufferBlock>>>> __bufferBlockMap;

		[[nodiscard]]
		constexpr size_t __resolveAlignmentOf(const VkBufferUsageFlags usage) const noexcept;
	};

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

module: private;

namespace Engine
{
	MemoryAllocator::MemoryAllocator(
		Graphics::LogicalDevice &device,
		const size_t memoryBlockSize, const size_t bufferBlockSize,
		const VkDeviceSize uniformBufferAlignment, const VkDeviceSize storageBufferAlignment) noexcept :
		__device					{ device },
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
			const size_t newBufferBlockSize{ std::max(__bufferBlockSize, size) };
			auto pNewBufferBlock{ std::make_unique<BufferBlock>(__device, size, usage) };

			const auto &bufferBlockMemReq{ pNewBufferBlock->getMemoryRequirements() };
			
			for (uint32_t memoryTypeBits{ bufferBlockMemReq.memoryTypeBits }; memoryTypeBits; memoryTypeBits >>= 1ULL)
			{


			}
		}

		return nullptr;
	}
}
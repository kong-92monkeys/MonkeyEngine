module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.LayerResourcePool;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Lib.LazyRecycler;
import ntmonkeys.com.Engine.MemoryAllocator;
import <memory>;

namespace Engine
{
	export class LayerResourcePool : public Lib::Unique
	{
	public:
		LayerResourcePool(Lib::LazyDeleter &lazyDeleter, MemoryAllocator &memoryAllocator) noexcept;
		virtual ~LayerResourcePool() noexcept override = default;

		[[nodiscard]]
		std::shared_ptr<BufferChunk> getStorageBuffer(const size_t size);

		[[nodiscard]]
		void recycleStorageBuffer(std::shared_ptr<BufferChunk> &&pBuffer) noexcept;

	private:
		MemoryAllocator &__memoryAllocator;

		Lib::LazyRecycler<BufferChunk> __storageBufferRecycler;
	};
}

module: private;

namespace Engine
{
	LayerResourcePool::LayerResourcePool(Lib::LazyDeleter &lazyDeleter, MemoryAllocator &memoryAllocator) noexcept :
		__memoryAllocator{ memoryAllocator }, __storageBufferRecycler{ lazyDeleter }
	{}

	std::shared_ptr<BufferChunk> LayerResourcePool::getStorageBuffer(const size_t size)
	{
		std::shared_ptr<BufferChunk> pRetVal
		{
			__storageBufferRecycler.retrieveWhere([size] (const auto &elem)
			{
				return (elem.getSize() >= size);
			})
		};

		if (!pRetVal)
		{
			pRetVal = std::shared_ptr<BufferChunk>
			{
				__memoryAllocator.allocateBuffer(
					VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
					VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					size, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT)
			};
		}

		return pRetVal;
	}

	void LayerResourcePool::recycleStorageBuffer(std::shared_ptr<BufferChunk> &&pBuffer) noexcept
	{
		__storageBufferRecycler.recycle(std::move(pBuffer));
	}
}
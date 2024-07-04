module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Memory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class Memory : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkDeviceSize size{ };
			uint32_t typeIndex{ };
			VkImage hDedicatedImage{ };
			VkBuffer hDedicatedBuffer{ };
		};

		explicit Memory(const CreateInfo &createInfo) noexcept;
		virtual ~Memory() noexcept override;

		[[nodiscard]]
		constexpr VkDeviceSize getSize() const noexcept;

		[[nodiscard]]
		constexpr uint32_t getTypeIndex() const noexcept;

		[[nodiscard]]
		void *getMappedMemory() noexcept;

		[[nodiscard]]
		constexpr const VkDeviceMemory &getHandle() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const VkDeviceSize __size;
		const uint32_t __typeIndex;

		VkDeviceMemory __handle{ };
		void *__pMapped{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr VkDeviceSize Memory::getSize() const noexcept
	{
		return __size;
	}

	constexpr uint32_t Memory::getTypeIndex() const noexcept
	{
		return __typeIndex;
	}

	constexpr const VkDeviceMemory &Memory::getHandle() noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Memory::Memory(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice },
		__size			{ createInfo.size },
		__typeIndex		{ createInfo.typeIndex }
	{
		__create(createInfo);
	}

	Memory::~Memory() noexcept
	{
		if (__pMapped)
			__deviceProc.vkUnmapMemory(__hDevice, __handle);

		__deviceProc.vkFreeMemory(__hDevice, __handle, nullptr);
	}

	void *Memory::getMappedMemory() noexcept
	{
		if (!__pMapped)
			__deviceProc.vkMapMemory(__hDevice, __handle, 0U, VK_WHOLE_SIZE, 0U, &__pMapped);

		return __pMapped;
	}

	void Memory::__create(const CreateInfo &createInfo)
	{
		const bool isDedicatedMemory
		{
			createInfo.hDedicatedImage ||
			createInfo.hDedicatedBuffer
		};

		const VkMemoryDedicatedAllocateInfo dedicatedInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO },
			.image		{ createInfo.hDedicatedImage },
			.buffer		{ createInfo.hDedicatedBuffer }
		};

		const VkMemoryAllocateInfo vkCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO },
			.pNext				{ isDedicatedMemory ? &dedicatedInfo : nullptr },
			.allocationSize		{ createInfo.size },
			.memoryTypeIndex	{ createInfo.typeIndex }
		};

		__deviceProc.vkAllocateMemory(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Memory." };
	}
}
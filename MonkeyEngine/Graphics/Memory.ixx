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
			VkDeviceSize allocationSize{ };
			uint32_t memoryTypeIndex{ };
			VkImage hDedicatedImage{ };
			VkBuffer hDedicatedBuffer{ };
		};

		explicit Memory(const CreateInfo &createInfo) noexcept;
		virtual ~Memory() noexcept override;

		[[nodiscard]]
		constexpr const VkDeviceMemory &getHandle() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkDeviceMemory __handle{ };

		void __create(const CreateInfo &createInfo);
	};

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
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	Memory::~Memory() noexcept
	{
		__deviceProc.vkFreeMemory(__hDevice, __handle, nullptr);
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
			.allocationSize		{ createInfo.allocationSize },
			.memoryTypeIndex	{ createInfo.memoryTypeIndex }
		};

		__deviceProc.vkAllocateMemory(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Memory." };
	}
}
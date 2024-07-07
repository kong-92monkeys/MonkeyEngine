module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Buffer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class Buffer : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkDeviceSize size{ };
			VkBufferUsageFlags usage{ };
		};

		explicit Buffer(const CreateInfo &createInfo) noexcept;
		virtual ~Buffer() noexcept override;

		[[nodiscard]]
		constexpr bool needDedicatedAllocation() const noexcept;

		[[nodiscard]]
		constexpr const VkMemoryRequirements &getMemoryRequirements() const noexcept;

		VkResult bindMemory(const VkDeviceMemory hMemory, const VkDeviceSize offset) noexcept;

		[[nodiscard]]
		constexpr const VkBuffer &getHandle() const noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkMemoryDedicatedRequirements __memDedicatedReq{ };
		VkMemoryRequirements2 __memReq2{ };

		VkBuffer __handle{ };

		void __create(const CreateInfo &createInfo);
		void __resolveMemoryRequirements() noexcept;
	};

	constexpr bool Buffer::needDedicatedAllocation() const noexcept
	{
		return (__memDedicatedReq.prefersDedicatedAllocation || __memDedicatedReq.requiresDedicatedAllocation);
	}

	constexpr const VkMemoryRequirements &Buffer::getMemoryRequirements() const noexcept
	{
		return __memReq2.memoryRequirements;
	}

	constexpr const VkBuffer &Buffer::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Buffer::Buffer(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
		__resolveMemoryRequirements();
	}

	Buffer::~Buffer() noexcept
	{
		__deviceProc.vkDestroyBuffer(__hDevice, __handle, nullptr);
	}

	VkResult Buffer::bindMemory(const VkDeviceMemory hMemory, const VkDeviceSize offset) noexcept
	{
		const VkBindBufferMemoryInfo bindInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO },
			.buffer			{ __handle },
			.memory			{ hMemory },
			.memoryOffset	{ offset }
		};

		return __deviceProc.vkBindBufferMemory2(__hDevice, 1U, &bindInfo);
	}

	void Buffer::__create(const CreateInfo &createInfo)
	{
		const VkBufferCreateInfo vkCreateInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO },
			.size			{ createInfo.size },
			.usage			{ createInfo.usage },
			.sharingMode	{ VkSharingMode::VK_SHARING_MODE_EXCLUSIVE }
		};

		__deviceProc.vkCreateBuffer(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Buffer." };
	}

	void Buffer::__resolveMemoryRequirements() noexcept
	{
		__memDedicatedReq.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_DEDICATED_REQUIREMENTS;

		__memReq2.sType = VkStructureType::VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		__memReq2.pNext = &__memDedicatedReq;

		const VkBufferMemoryRequirementsInfo2 resolveInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2 },
			.buffer	{ __handle }
		};
		
		__deviceProc.vkGetBufferMemoryRequirements2(__hDevice, &resolveInfo, &__memReq2);
	}
}
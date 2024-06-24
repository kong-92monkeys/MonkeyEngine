module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.CommandPool;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.CommandBuffer;
import <stdexcept>;
import <vector>;

namespace Graphics
{
	export class CommandPool : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			uint32_t queueFamilyIndex{ };
		};

		explicit CommandPool(const CreateInfo &createInfo);
		virtual ~CommandPool() noexcept override;

		uint32_t allocateCommandBuffers(const VkCommandBufferLevel level, const uint32_t count);

		[[nodiscard]]
		constexpr CommandBuffer getCommandBuffer(const uint32_t index) noexcept;

		VkResult reset(const VkCommandPoolResetFlags flags);

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkCommandPool __handle{ };
		std::vector<VkCommandBuffer> __bufferHandles;

		void __create(const CreateInfo &createInfo);
	};

	constexpr CommandBuffer CommandPool::getCommandBuffer(const uint32_t index) noexcept
	{
		const CommandBuffer::MakeInfo makeInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.handle			{ __bufferHandles[index] }
		};

		return CommandBuffer{ makeInfo };
	}
}

module: private;

namespace Graphics
{
	CommandPool::CommandPool(const CreateInfo &createInfo) :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	CommandPool::~CommandPool() noexcept
	{
		__deviceProc.vkDestroyCommandPool(__hDevice, __handle, nullptr);
	}

	uint32_t CommandPool::allocateCommandBuffers(const VkCommandBufferLevel level, const uint32_t count)
	{
		const VkCommandBufferAllocateInfo allocInfo
		{
			.sType					{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO },
			.commandPool			{ __handle },
			.level					{ level },
			.commandBufferCount		{ count }
		};

		const size_t prevBufferCount{ __bufferHandles.size() };
		__bufferHandles.resize(prevBufferCount + count);
		__deviceProc.vkAllocateCommandBuffers(__hDevice, &allocInfo, __bufferHandles.data() + prevBufferCount);

		return static_cast<uint32_t>(prevBufferCount);
	}

	VkResult CommandPool::reset(const VkCommandPoolResetFlags flags)
	{
		return __deviceProc.vkResetCommandPool(__hDevice, __handle, flags);
	}

	void CommandPool::__create(const CreateInfo &createInfo)
	{
		const VkCommandPoolCreateInfo vkCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO },
			.flags				{ VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT },
			.queueFamilyIndex	{ createInfo.queueFamilyIndex }
		};

		__deviceProc.vkCreateCommandPool(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a CommandPool." };
	}
}
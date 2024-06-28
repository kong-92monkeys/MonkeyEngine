module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.CommandBufferCirculator;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.CommandPool;
import ntmonkeys.com.Graphics.CommandBuffer;
import <vector>;
import <memory>;

namespace Engine
{
	export class CommandBufferCirculator : public Lib::Unique
	{
	public:
		CommandBufferCirculator(
			Graphics::LogicalDevice &logicalDevice, const VkCommandBufferLevel level,
			const uint32_t poolCount, const uint32_t bufferCount);

		[[nodiscard]]
		Graphics::CommandBuffer getNext();

	private:
		const uint32_t __poolCount;
		const uint32_t __bufferCount;

		uint32_t __poolIdx{ };
		uint32_t __bufferIdx{ };

		std::vector<std::unique_ptr<Graphics::CommandPool>> __pools;

		void __create(Graphics::LogicalDevice &logicalDevice, const VkCommandBufferLevel level);
		void __advanceIndex();
	};
}

module: private;

namespace Engine
{
	CommandBufferCirculator::CommandBufferCirculator(
		Graphics::LogicalDevice &logicalDevice, const VkCommandBufferLevel level,
		const uint32_t poolCount, const uint32_t bufferCount) :
		__poolCount		{ poolCount },
		__bufferCount	{ bufferCount }
	{
		__create(logicalDevice, level);
	}

	Graphics::CommandBuffer CommandBufferCirculator::getNext()
	{
		__advanceIndex();
		return __pools[__poolIdx]->getCommandBuffer(__bufferIdx);
	}

	void CommandBufferCirculator::__create(Graphics::LogicalDevice &logicalDevice, const VkCommandBufferLevel level)
	{
		for (uint32_t poolIter{ }; poolIter < __poolCount; ++poolIter)
		{
			auto pPool{ logicalDevice.createCommandPool(VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_TRANSIENT_BIT) };
			pPool->allocateCommandBuffers(level, __bufferCount);
			__pools.emplace_back(std::move(pPool));
		}
	}

	void CommandBufferCirculator::__advanceIndex()
	{
		++__bufferIdx;
		if (__bufferIdx < __bufferCount)
			return;

		__bufferIdx = 0U;
		__poolIdx = ((__poolIdx + 1U) % __poolCount);
		__pools[__poolIdx]->reset(0U);
	}
}
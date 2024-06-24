module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.CommandExecutor;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.CommandBufferCirculator;
import <memory>;

namespace Engine
{
	export class CommandExecutor : public Lib::Unique
	{
	public:
		class Execution : public Lib::Unique
		{
		public:
			virtual void beginRenderPass(
				const VkRenderPass hRenderPass,
				const VkFramebuffer hFramebuffer,
				const VkRect2D &renderArea,
				const uint32_t clearValueCount,
				const VkClearValue *const pClearValues,
				const VkSubpassContents subpassContents) noexcept = 0;

			virtual void endRenderPass() noexcept = 0;
		};

		CommandExecutor(Graphics::LogicalDevice &logicalDevice);

		[[nodiscard]]
		std::unique_ptr<Execution> makeExecution();
		void execute(std::unique_ptr<Execution> pExecution);

	private:
		class __ExecutionImpl : public Execution
		{
		public:
			__ExecutionImpl(Graphics::CommandBuffer commandBuffer) noexcept;
			virtual ~__ExecutionImpl() noexcept override = default;

			Graphics::CommandBuffer end() noexcept;

			virtual void beginRenderPass(
				const VkRenderPass hRenderPass,
				const VkFramebuffer hFramebuffer,
				const VkRect2D &renderArea,
				const uint32_t clearValueCount,
				const VkClearValue *const pClearValues,
				const VkSubpassContents subpassContents) noexcept override;

			virtual void endRenderPass() noexcept override;

		private:
			Graphics::CommandBuffer __commandBuffer;
		};

		Graphics::LogicalDevice &__logicalDevice;

		std::unique_ptr<CommandBufferCirculator> __pCBCirculator;
	};
}

module: private;

namespace Engine
{
	CommandExecutor::CommandExecutor(Graphics::LogicalDevice &logicalDevice) :
		__logicalDevice{ logicalDevice }
	{
		__pCBCirculator = std::make_unique<CommandBufferCirculator>(
			logicalDevice, VkCommandBufferLevel::VK_COMMAND_BUFFER_LEVEL_PRIMARY, 2U, 100U);
	}

	std::unique_ptr<CommandExecutor::Execution> CommandExecutor::makeExecution()
	{
		return std::make_unique<__ExecutionImpl>(__pCBCirculator->getNext());
	}

	void CommandExecutor::execute(std::unique_ptr<Execution> pExecution)
	{
		const auto pExecutionImpl{ static_cast<__ExecutionImpl *>(pExecution.get()) };
		const auto commandBuffer{ pExecutionImpl->end() };

		auto &queue{ __logicalDevice.getQueue() };
	}

	CommandExecutor::__ExecutionImpl::__ExecutionImpl(Graphics::CommandBuffer commandBuffer) noexcept :
		__commandBuffer{ std::move(commandBuffer) }
	{
		const VkCommandBufferBeginInfo cbBeginInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO },
			.flags{ VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT }
		};

		__commandBuffer.begin(cbBeginInfo);
	}

	Graphics::CommandBuffer CommandExecutor::__ExecutionImpl::end() noexcept
	{
		__commandBuffer.end();
		return std::move(__commandBuffer);
	}

	void CommandExecutor::__ExecutionImpl::beginRenderPass(
		const VkRenderPass hRenderPass,
		const VkFramebuffer hFramebuffer,
		const VkRect2D &renderArea,
		const uint32_t clearValueCount,
		const VkClearValue *const pClearValues,
		const VkSubpassContents subpassContents) noexcept
	{
		const VkRenderPassBeginInfo renderPassBeginInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
			.renderPass			{ hRenderPass },
			.framebuffer		{ hFramebuffer },
			.renderArea			{ renderArea },
			.clearValueCount	{ clearValueCount },
			.pClearValues		{ pClearValues }
		};
		
		const VkSubpassBeginInfo subpassBeginInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO },
			.contents	{ subpassContents }
		};
		
		__commandBuffer.beginRenderPass(renderPassBeginInfo, subpassBeginInfo);
	}

	void CommandExecutor::__ExecutionImpl::endRenderPass() noexcept
	{
		const VkSubpassEndInfo subpassEndInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		__commandBuffer.endRenderPass(subpassEndInfo);
	}
}
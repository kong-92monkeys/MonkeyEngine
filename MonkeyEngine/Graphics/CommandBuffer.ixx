module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;

namespace Graphics
{
	export class CommandBuffer
	{
	public:
		struct MakeInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkCommandBuffer handle{ };
		};

		constexpr explicit CommandBuffer(const MakeInfo &makeInfo);

		VkResult begin(const VkCommandBufferBeginInfo &beginInfo) const noexcept;
		VkResult end() const noexcept;

		void beginRenderPass(
			const VkRenderPassBeginInfo &renderPassBeginInfo,
			const VkSubpassBeginInfo &subpassBeginInfo) const noexcept;

		void endRenderPass(const VkSubpassEndInfo &subpassEndInfo) const noexcept;

		void bindPipeline(const VkPipelineBindPoint pipelineBindPoint, const VkPipeline pipeline) const noexcept;
		void setViewport(const uint32_t firstViewport, const uint32_t viewportCount, const VkViewport *const pViewports) const noexcept;
		void setScissor(const uint32_t firstScissor, const uint32_t scissorCount, const VkRect2D *const pScissors) const noexcept;

		void draw(
			const uint32_t vertexCount, const uint32_t instanceCount,
			const uint32_t firstVertex, const uint32_t firstInstance) const noexcept;

		void drawIndexed(
			const uint32_t indexCount, const uint32_t instanceCount,
			const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance) const noexcept;

		void copyBuffer(const VkCopyBufferInfo2 &copyInfo) const noexcept;
		void copyBufferToImage(const VkCopyBufferToImageInfo2 &copyInfo) const noexcept;

		void pipelineBarrier(const VkDependencyInfo &dependencyInfo) const noexcept;
		
		void bindVertexBuffers(
			const uint32_t firstBinding, const uint32_t bindingCount,
			const VkBuffer *const pBuffers, const VkDeviceSize *const pOffsets) const noexcept;

		void bindIndexBuffer(const VkBuffer buffer, const VkDeviceSize offset, const VkIndexType indexType) const noexcept;

		void bindDescriptorSets(
			const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout layout,
			const uint32_t firstSet, const uint32_t descriptorSetCount, const VkDescriptorSet *const pDescriptorSets,
			const uint32_t dynamicOffsetCount, const uint32_t *const pDynamicOffsets) const noexcept;

		void executeCommands(const uint32_t commandBufferCount, const VkCommandBuffer *const pCommandBuffers) const noexcept;

		[[nodiscard]]
		constexpr const VkCommandBuffer &getHandle() const noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkCommandBuffer __handle;
	};

	constexpr CommandBuffer::CommandBuffer(const MakeInfo &makeInfo) :
		__deviceProc	{ *(makeInfo.pDeviceProc) },
		__handle		{makeInfo.handle }
	{}

	constexpr const VkCommandBuffer &CommandBuffer::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	VkResult CommandBuffer::begin(const VkCommandBufferBeginInfo &beginInfo) const noexcept
	{
		return __deviceProc.vkBeginCommandBuffer(__handle, &beginInfo);
	}

	VkResult CommandBuffer::end() const noexcept
	{
		return __deviceProc.vkEndCommandBuffer(__handle);
	}

	void CommandBuffer::beginRenderPass(
		const VkRenderPassBeginInfo &renderPassBeginInfo,
		const VkSubpassBeginInfo &subpassBeginInfo) const noexcept
	{
		__deviceProc.vkCmdBeginRenderPass2(__handle, &renderPassBeginInfo, &subpassBeginInfo);
	}

	void CommandBuffer::endRenderPass(const VkSubpassEndInfo &subpassEndInfo) const noexcept
	{
		__deviceProc.vkCmdEndRenderPass2(__handle, &subpassEndInfo);
	}

	void CommandBuffer::bindPipeline(const VkPipelineBindPoint pipelineBindPoint, const VkPipeline pipeline) const noexcept
	{
		__deviceProc.vkCmdBindPipeline(__handle, pipelineBindPoint, pipeline);
	}

	void CommandBuffer::setViewport(const uint32_t firstViewport, const uint32_t viewportCount, const VkViewport *const pViewports) const noexcept
	{
		__deviceProc.vkCmdSetViewport(__handle, firstViewport, viewportCount, pViewports);
	}

	void CommandBuffer::setScissor(const uint32_t firstScissor, const uint32_t scissorCount, const VkRect2D *const pScissors) const noexcept
	{
		__deviceProc.vkCmdSetScissor(__handle, firstScissor, scissorCount, pScissors);
	}

	void CommandBuffer::draw(
		const uint32_t vertexCount, const uint32_t instanceCount,
		const uint32_t firstVertex, const uint32_t firstInstance) const noexcept
	{
		__deviceProc.vkCmdDraw(__handle, vertexCount, instanceCount, firstVertex, firstInstance);
	}

	void CommandBuffer::drawIndexed(
		const uint32_t indexCount, const uint32_t instanceCount,
		const uint32_t firstIndex, const int32_t vertexOffset, const uint32_t firstInstance) const noexcept
	{
		__deviceProc.vkCmdDrawIndexed(__handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
	}

	void CommandBuffer::copyBuffer(const VkCopyBufferInfo2 &copyInfo) const noexcept
	{
		__deviceProc.vkCmdCopyBuffer2(__handle, &copyInfo);
	}

	void CommandBuffer::copyBufferToImage(const VkCopyBufferToImageInfo2 &copyInfo) const noexcept
	{
		__deviceProc.vkCmdCopyBufferToImage2(__handle, &copyInfo);
	}

	void CommandBuffer::pipelineBarrier(const VkDependencyInfo &dependencyInfo) const noexcept
	{
		__deviceProc.vkCmdPipelineBarrier2(__handle, &dependencyInfo);
	}

	void CommandBuffer::bindVertexBuffers(
		const uint32_t firstBinding, const uint32_t bindingCount,
		const VkBuffer *const pBuffers, const VkDeviceSize *const pOffsets) const noexcept
	{
		__deviceProc.vkCmdBindVertexBuffers(__handle, firstBinding, bindingCount, pBuffers, pOffsets);
	}

	void CommandBuffer::bindIndexBuffer(
		const VkBuffer buffer, const VkDeviceSize offset, const VkIndexType indexType) const noexcept
	{
		__deviceProc.vkCmdBindIndexBuffer(__handle, buffer, offset, indexType);
	}

	void CommandBuffer::bindDescriptorSets(
		const VkPipelineBindPoint pipelineBindPoint, const VkPipelineLayout layout,
		const uint32_t firstSet, const uint32_t descriptorSetCount, const VkDescriptorSet *const pDescriptorSets,
		const uint32_t dynamicOffsetCount, const uint32_t *const pDynamicOffsets) const noexcept
	{
		__deviceProc.vkCmdBindDescriptorSets(
			__handle, pipelineBindPoint, layout,
			firstSet, descriptorSetCount, pDescriptorSets,
			dynamicOffsetCount, pDynamicOffsets);
	}

	void CommandBuffer::executeCommands(const uint32_t commandBufferCount, const VkCommandBuffer *const pCommandBuffers) const noexcept
	{
		__deviceProc.vkCmdExecuteCommands(__handle, commandBufferCount, pCommandBuffers);
	}
}
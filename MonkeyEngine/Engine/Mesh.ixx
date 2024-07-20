module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Mesh;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.EngineContext;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Lib.LazyDeleter;
import <unordered_map>;
import <memory>;

namespace Engine
{
	export class Mesh : public Lib::Unique
	{
	public:
		Mesh(const EngineContext &context) noexcept;
		virtual ~Mesh() noexcept override;

		void createVertexBuffer(const uint32_t bindingIndex, const void *const pData, const size_t size) noexcept;
		void updateVertexBuffer(const uint32_t bindingIndex, const void *const pData, const size_t size, const size_t offset) noexcept;
		void clearVertexBuffer(const uint32_t bindingIndex) noexcept;
		void clearVertexBuffers() noexcept;

		void createIndexBuffer(const VkIndexType type, const void *const pData, const size_t size) noexcept;
		void updateIndexBuffer(const void *const pData, const size_t size, const size_t offset) noexcept;
		void clearIndexBuffer() noexcept;

		void bind(Graphics::CommandBuffer &commandBuffer) const noexcept;

	private:
		const EngineContext &__context;

		std::unordered_map<uint32_t, std::shared_ptr<BufferChunk>> __vertexBuffers;

		mutable bool __cmdParamInvalidated{ };
		mutable std::vector<VkBuffer> __cmdParam_vertexBufferHandles;
		mutable std::vector<VkDeviceSize> __cmdParam_vertexBufferOffsets;

		VkIndexType __indexType{};
		std::shared_ptr<BufferChunk> __pIndexBuffer;

		void __updateData(
			BufferChunk &dst, const void *const pData, const size_t size, const size_t offset,
			const VkPipelineStageFlags2 stageMask, const VkAccessFlags2 accessMask) noexcept;

		void __validateCmdParams() const noexcept;
	};
}

module: private;

namespace Engine
{
	Mesh::Mesh(const EngineContext &context) noexcept :
		__context{ context }
	{}

	Mesh::~Mesh() noexcept
	{
		const auto pLazyDeleter{ __context.pLazyDeleter };

		for (const auto &[_, pVertexBuffer] : __vertexBuffers)
		{
			if (pVertexBuffer)
				pLazyDeleter->reserve(std::move(pVertexBuffer));
		}
		
		if (__pIndexBuffer)
			pLazyDeleter->reserve(std::move(__pIndexBuffer));
	}

	void Mesh::createVertexBuffer(const uint32_t bindingIndex, const void *const pData, const size_t size) noexcept
	{
		const auto pLazyDeleter			{ __context.pLazyDeleter };
		const auto pMemoryAllocator		{ __context.pMemoryAllocator };

		auto &pVertexBuffer{ __vertexBuffers[bindingIndex] };
		if (pVertexBuffer)
			pLazyDeleter->reserve(std::move(pVertexBuffer));

		pVertexBuffer = std::shared_ptr<BufferChunk>
		{
			pMemoryAllocator->allocateBuffer(
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT)
		};

		__updateData(
			*pVertexBuffer, pData, size, 0ULL,
			VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT, VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT);

		__cmdParamInvalidated = true;
	}

	void Mesh::updateVertexBuffer(const uint32_t bindingIndex, const void *const pData, const size_t size, const size_t offset) noexcept
	{
		__updateData(
			*(__vertexBuffers[bindingIndex]), pData, size, offset,
			VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT, VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT);
	}

	void Mesh::clearVertexBuffer(const uint32_t bindingIndex) noexcept
	{
		auto &pVertexBuffer{ __vertexBuffers[bindingIndex] };
		if (!pVertexBuffer)
			return;

		const auto pLazyDeleter{ __context.pLazyDeleter };
		pLazyDeleter->reserve(std::move(pVertexBuffer));

		__cmdParamInvalidated = true;
	}

	void Mesh::clearVertexBuffers() noexcept
	{
		const auto pLazyDeleter{ __context.pLazyDeleter };

		for (const auto &[bindingIndex, pVertexBuffer] : __vertexBuffers)
		{
			if (!pVertexBuffer)
				continue;

			pLazyDeleter->reserve(std::move(pVertexBuffer));
		}

		__cmdParamInvalidated = true;
	}

	void Mesh::createIndexBuffer(const VkIndexType type, const void *const pData, const size_t size) noexcept
	{
		__indexType = type;

		const auto pLazyDeleter			{ __context.pLazyDeleter };
		const auto pMemoryAllocator		{ __context.pMemoryAllocator };

		if (__pIndexBuffer)
			pLazyDeleter->reserve(std::move(__pIndexBuffer));

		__pIndexBuffer = std::shared_ptr<BufferChunk>
		{
			pMemoryAllocator->allocateBuffer(
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT)
		};

		__updateData(
			*__pIndexBuffer, pData, size, 0ULL,
			VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT, VK_ACCESS_2_INDEX_READ_BIT);
	}

	void Mesh::updateIndexBuffer(const void *const pData, const size_t size, const size_t offset) noexcept
	{
		__updateData(
			*__pIndexBuffer, pData, size, offset,
			VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT, VK_ACCESS_2_INDEX_READ_BIT);
	}

	void Mesh::clearIndexBuffer() noexcept
	{
		if (!__pIndexBuffer)
			return;

		const auto pLazyDeleter{ __context.pLazyDeleter };
		pLazyDeleter->reserve(std::move(__pIndexBuffer));
	}

	void Mesh::bind(Graphics::CommandBuffer &commandBuffer) const noexcept
	{
		__validateCmdParams();

		commandBuffer.bindVertexBuffers(
			0U, static_cast<uint32_t>(__cmdParam_vertexBufferHandles.size()),
			__cmdParam_vertexBufferHandles.data(), __cmdParam_vertexBufferOffsets.data());

		if (__pIndexBuffer)
		{
			commandBuffer.bindIndexBuffer(
				__pIndexBuffer->getBuffer().getHandle(),
				__pIndexBuffer->getOffset(), __indexType);
		}
	}

	void Mesh::__updateData(
		BufferChunk &dst, const void *const pData, const size_t size, const size_t offset,
		const VkPipelineStageFlags2 stageMask, const VkAccessFlags2 accessMask) noexcept
	{
		const auto pLazyDeleter			{ __context.pLazyDeleter };
		const auto pMemoryAllocator		{ __context.pMemoryAllocator };
		const auto pCommandExecutor		{ __context.pCommandExecutor };

		const auto pStagingBuffer
		{
			pMemoryAllocator->allocateBuffer(
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
				VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
				size,
				VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT)
		};

		std::memcpy(pStagingBuffer->getMappedMemory(), pData, size);

		pCommandExecutor->reserve([=, &dst, pSrc{ pStagingBuffer }] (auto &commandBuffer)
		{
			// 주의: 파라미터로 받은 accessMask는 read access만 들어온다고 가정, before barrier에서 입력하지 않음
			const VkMemoryBarrier2 beforeMemoryBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
				.srcStageMask		{ stageMask },
				.srcAccessMask		{ 0U },
				.dstStageMask		{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.dstAccessMask		{ VK_ACCESS_2_TRANSFER_WRITE_BIT }
			};

			const VkDependencyInfo beforeBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.memoryBarrierCount	{ 1U },
				.pMemoryBarriers	{ &beforeMemoryBarrier },
			};

			commandBuffer.pipelineBarrier(beforeBarrier);

			const VkBufferCopy2 region
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_BUFFER_COPY_2 },
				.srcOffset		{ pSrc->getOffset() },
				.dstOffset		{ dst.getOffset() + offset },
				.size			{ size }
			};

			const VkCopyBufferInfo2 copyInfo
			{
				.sType			{ VkStructureType::VK_STRUCTURE_TYPE_COPY_BUFFER_INFO_2 },
				.srcBuffer		{ pSrc->getBuffer().getHandle() },
				.dstBuffer		{ dst.getBuffer().getHandle() },
				.regionCount	{ 1U },
				.pRegions		{ &region }
			};

			commandBuffer.copyBuffer(copyInfo);

			const VkMemoryBarrier2 afterMemoryBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
				.srcStageMask		{ VK_PIPELINE_STAGE_2_COPY_BIT },
				.srcAccessMask		{ VK_ACCESS_2_TRANSFER_WRITE_BIT },
				.dstStageMask		{ stageMask },
				.dstAccessMask		{ accessMask }
			};

			const VkDependencyInfo afterBarrier
			{
				.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEPENDENCY_INFO },
				.memoryBarrierCount	{ 1U },
				.pMemoryBarriers	{ &afterMemoryBarrier },
			};

			commandBuffer.pipelineBarrier(afterBarrier);
		});

		pLazyDeleter->reserve(std::shared_ptr<BufferChunk>{ pStagingBuffer });
	}

	void Mesh::__validateCmdParams() const noexcept
	{
		if (!__cmdParamInvalidated)
			return;

		__cmdParam_vertexBufferHandles.clear();
		__cmdParam_vertexBufferOffsets.clear();

		for (const auto &[bindingIndex, pVertexBuffer] : __vertexBuffers)
		{
			if (!pVertexBuffer)
				continue;

			if (bindingIndex >= __cmdParam_vertexBufferHandles.size())
			{
				__cmdParam_vertexBufferHandles.resize(bindingIndex + 1U);
				__cmdParam_vertexBufferOffsets.resize(bindingIndex + 1U);
			}

			__cmdParam_vertexBufferHandles[bindingIndex] = pVertexBuffer->getBuffer().getHandle();
			__cmdParam_vertexBufferOffsets[bindingIndex] = pVertexBuffer->getOffset();
		}

		__cmdParamInvalidated = false;
	}
}
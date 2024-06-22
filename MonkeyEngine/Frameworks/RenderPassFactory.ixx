module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.RenderPassFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Engine.RenderingEngine;
import <unordered_map>;
import <memory>;

namespace Frameworks
{
	export enum class RenderPassType
	{
		COLOR
	};

	export class RenderPassFactory : public Lib::Unique
	{
	public:
		RenderPassFactory(Engine::RenderingEngine &engine);
		virtual ~RenderPassFactory() noexcept override = default;

		[[nodiscard]]
		const Graphics::RenderPass &getInstance(const RenderPassType type) const noexcept;

		[[nodiscard]]
		const Graphics::RenderPass &getCustomInstance(const uint32_t typeId) const noexcept;
		void registerCustomInstance(const uint32_t typeId, std::unique_ptr<Graphics::RenderPass> pInstance) noexcept;

	protected:
		Engine::RenderingEngine &__engine;

		std::unordered_map<RenderPassType, std::unique_ptr<Graphics::RenderPass>> __instanceMap;
		std::unordered_map<uint32_t, std::unique_ptr<Graphics::RenderPass>> __customInstanceMap;

		void __registerInstance_color();
	};
}

module: private;

namespace Frameworks
{
	RenderPassFactory::RenderPassFactory(Engine::RenderingEngine &engine) : __engine{ engine }
	{
		__registerInstance_color();
	}

	const Graphics::RenderPass &RenderPassFactory::getInstance(const RenderPassType type) const noexcept
	{
		return *(__instanceMap.at(type));
	}

	const Graphics::RenderPass &RenderPassFactory::getCustomInstance(const uint32_t typeId) const noexcept
	{
		return *(__customInstanceMap.at(typeId));
	}

	void RenderPassFactory::registerCustomInstance(const uint32_t typeId, std::unique_ptr<Graphics::RenderPass> pInstance) noexcept
	{
		__customInstanceMap[typeId] = std::move(pInstance);
	}

	void RenderPassFactory::__registerInstance_color()
	{
		const VkAttachmentDescription2 attachmentDesc
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2 },
			.format				{ VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
			.samples			{ VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT },
			.loadOp				{ VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR },
			.storeOp			{ VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE },
			.stencilLoadOp		{ VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE },
			.stencilStoreOp		{ VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE },
			.initialLayout		{ VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED },
			.finalLayout		{ VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }
		};

		const VkAttachmentReference2 attachment
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2 },
			.attachment		{ 0U },
			.layout			{ VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
			.aspectMask		{ VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT }
		};

		const VkSubpassDescription2 subpass
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2 },
			.pipelineBindPoint			{ VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS },
			.colorAttachmentCount		{ 1U },
			.pColorAttachments			{ &attachment }
		};

		const VkMemoryBarrier2 memoryBarrier
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
			.srcStageMask	{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.srcAccessMask	{ VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT },
			.dstStageMask	{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.dstAccessMask	{ VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT }
		};

		const VkSubpassDependency2 dependency
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2 },
			.pNext				{ &memoryBarrier },
			.srcSubpass			{ VK_SUBPASS_EXTERNAL },
			.dstSubpass			{ 0U }
		};

		__instanceMap[RenderPassType::COLOR] = __engine.createRenderPass(1U, &attachmentDesc, 1U, &subpass, 1U, &dependency);
	}
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderPassFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.RenderPass;
import <unordered_map>;
import <memory>;

namespace Engine
{
	export enum class RenderPassType
	{
		COLOR
	};

	export class RenderPassFactory : public Lib::Unique
	{
	public:
		RenderPassFactory(Graphics::LogicalDevice &device);
		virtual ~RenderPassFactory() noexcept override = default;

		[[nodiscard]]
		const Graphics::RenderPass &getInstance(const RenderPassType type) const noexcept;

	protected:
		Graphics::LogicalDevice &__device;

		std::unordered_map<RenderPassType, std::unique_ptr<Graphics::RenderPass>> __instanceMap;

		void __createInstance_color();
	};
}

module: private;

namespace Engine
{
	RenderPassFactory::RenderPassFactory(Graphics::LogicalDevice &device) : __device{ device }
	{
		__createInstance_color();
	}

	const Graphics::RenderPass &RenderPassFactory::getInstance(const RenderPassType type) const noexcept
	{
		return *(__instanceMap.at(type));
	}

	void RenderPassFactory::__createInstance_color()
	{
		// loadOp은 dependency 수행 이후 수행, memory barrier를 잘 쳐야 layout transition과 충돌 안남
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

		// VkImageMemoryBarrier와 동일 기능 수행. layout 전환은 description에 기술 된대로
		const VkMemoryBarrier2 memoryBarrier
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_MEMORY_BARRIER_2 },
			.srcStageMask	{ VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT },
			.srcAccessMask	{ VK_ACCESS_2_NONE },
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

		__instanceMap[RenderPassType::COLOR] = std::unique_ptr<Graphics::RenderPass>
		{
			__device.createRenderPass(1U, &attachmentDesc, 1U, &subpass, 1U, &dependency)
		};
	}
}
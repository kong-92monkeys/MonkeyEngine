module;

#include "../Library/GLM.h"
#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Layer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Graphics.Semaphore;
import ntmonkeys.com.Graphics.CommandBuffer;
import ntmonkeys.com.Engine.RenderPassFactory;
import ntmonkeys.com.Engine.FramebufferFactory;
import <memory>;

namespace Engine
{
	export class Layer : public Lib::Unique
	{
	public:
		struct DrawInfo
		{
		public:
			Graphics::ImageView *pImageView{ };
			VkRect2D renderArea{ };
			const glm::vec4 *pClearColor{ };
			FramebufferFactory *pFramebufferFactory{ };
		};

		Layer(const RenderPassFactory &renderPassFactory) noexcept;
		virtual ~Layer() noexcept override;

		void draw(const DrawInfo &drawInfo, Graphics::CommandBuffer &commandBuffer);

	private:
		const RenderPassFactory &__renderPassFactory;
	};
}

module: private;

namespace Engine
{
	Layer::Layer(const RenderPassFactory &renderPassFactory) noexcept :
		__renderPassFactory{ renderPassFactory }
	{}

	Layer::~Layer() noexcept
	{
	}

	void Layer::draw(const DrawInfo &drawInfo, Graphics::CommandBuffer &commandBuffer)
	{
		// TODO
		// renderer를 render pass type 단위로 묶어 관리 (for loop)
		// 초기화용 render pass, 렌더링용 render pass 구분 필요
		// 조사하기: framebuffer 초기화 성능? beginRenderPass? clear color?

		const auto &renderPass		{ __renderPassFactory.getInstance(RenderPassType::COLOR) };
		const auto &framebuffer		{ drawInfo.pFramebufferFactory->getInstance(RenderPassType::COLOR) };

		const VkRenderPassAttachmentBeginInfo attachmentInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_ATTACHMENT_BEGIN_INFO },
			.attachmentCount	{ 1U },
			.pAttachments		{ &(drawInfo.pImageView->getHandle()) }
		};

		VkClearValue clearValue{ };
		
		auto &color{ clearValue.color };
		color.float32[0] = drawInfo.pClearColor->r;
		color.float32[1] = drawInfo.pClearColor->g;
		color.float32[2] = drawInfo.pClearColor->b;
		color.float32[3] = drawInfo.pClearColor->a;

		const VkRenderPassBeginInfo renderPassBeginInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO },
			.pNext				{ &attachmentInfo },
			.renderPass			{ renderPass.getHandle() },
			.framebuffer		{ framebuffer.getHandle() },
			.renderArea			{ drawInfo.renderArea },
			.clearValueCount	{ 1U },
			.pClearValues		{ &clearValue }
		};

		const VkSubpassBeginInfo subpassBeginInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO },
			.contents	{ VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE }
		};

		commandBuffer.beginRenderPass(renderPassBeginInfo, subpassBeginInfo);

		const VkSubpassEndInfo subpassEndInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_SUBPASS_END_INFO }
		};

		commandBuffer.endRenderPass(subpassEndInfo);
	}
}
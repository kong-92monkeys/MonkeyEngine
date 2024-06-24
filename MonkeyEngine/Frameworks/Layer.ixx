module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.Layer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.CommandExecutor;
import ntmonkeys.com.Frameworks.RenderPassFactory;
import ntmonkeys.com.Frameworks.FramebufferFactory;
import <memory>;

namespace Frameworks
{
	export class Layer : public Lib::Unique
	{
	public:
		Layer(const RenderPassFactory &renderPassFactory) noexcept;
		virtual ~Layer() noexcept override;

		void draw(
			const VkRect2D &renderArea,
			FramebufferFactory &framebufferFactory,
			Engine::CommandExecutor::Execution &execution);

	private:
		const RenderPassFactory &__renderPassFactory;
	};
}

module: private;

namespace Frameworks
{
	Layer::Layer(const RenderPassFactory &renderPassFactory) noexcept :
		__renderPassFactory{ renderPassFactory }
	{}

	Layer::~Layer() noexcept
	{
	}

	void Layer::draw(
		const VkRect2D &renderArea,
		FramebufferFactory &framebufferFactory,
		Engine::CommandExecutor::Execution &execution)
	{
		// TODO
		// 초기화용 render pass, 렌더링용 render pass 구분 필요
		// 조사하기: framebuffer 초기화 성능? beginRenderPass? clear color?

		const auto &renderPass		{ __renderPassFactory.getInstance(RenderPassType::COLOR) };
		const auto &framebuffer		{ framebufferFactory.getInstance(RenderPassType::COLOR) };

		VkClearValue clearValue{ };

		// TODO: VkRenderPassAttachmentBeginInfo 주입
		execution.beginRenderPass(
			renderPass.getHandle(), framebuffer.getHandle(),
			renderArea, 1U, &clearValue, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);

		execution.endRenderPass();
	}
}
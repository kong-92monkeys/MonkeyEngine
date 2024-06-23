module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.FramebufferFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.Framebuffer;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Frameworks.RenderPassFactory;
import <unordered_map>;
import <memory>;

namespace Frameworks
{
	export class FramebufferFactory : public Lib::Unique
	{
	public:
		FramebufferFactory(Engine::RenderingEngine &engine, const RenderPassFactory &renderPassFactory);
		virtual ~FramebufferFactory() noexcept override = default;

		void invalidate(const uint32_t width, const uint32_t height) noexcept;

		[[nodiscard]]
		const Graphics::Framebuffer &getInstance(const RenderPassType type) noexcept;

	protected:
		Engine::RenderingEngine &__engine;
		const RenderPassFactory &__renderPassFactory;

		uint32_t __width{ };
		uint32_t __height{ };

		std::unordered_map<RenderPassType, std::unique_ptr<Graphics::Framebuffer>> __instanceMap;

		void __createInstance_color();
	};
}

module: private;

namespace Frameworks
{
	FramebufferFactory::FramebufferFactory(
		Engine::RenderingEngine &engine, const RenderPassFactory &renderPassFactory) :
		__engine				{ engine },
		__renderPassFactory		{ renderPassFactory }
	{}

	const Graphics::Framebuffer &FramebufferFactory::getInstance(const RenderPassType type) noexcept
	{
		return *(__instanceMap.at(type));
	}

	void FramebufferFactory::invalidate(const uint32_t width, const uint32_t height) noexcept
	{
		__width = width;
		__height = height;
		__instanceMap.clear();

		__createInstance_color();
	}

	void FramebufferFactory::__createInstance_color()
	{
		const Graphics::Framebuffer::AttachmentInfo attachmentInfo
		{
			.format	{ VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
			.usage	{ VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT }
		};

		__instanceMap[RenderPassType::COLOR] = __engine.createFramebuffer(
			__renderPassFactory.getInstance(RenderPassType::COLOR), __width, __height, 1U, &attachmentInfo);
	}
}
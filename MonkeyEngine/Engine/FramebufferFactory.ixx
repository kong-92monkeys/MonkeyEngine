module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.FramebufferFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.Framebuffer;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Engine.RenderPassFactory;
import <unordered_map>;
import <memory>;

namespace Engine
{
	export class FramebufferFactory : public Lib::Unique
	{
	public:
		FramebufferFactory(Graphics::LogicalDevice &device, const RenderPassFactory &renderPassFactory);
		virtual ~FramebufferFactory() noexcept override = default;

		void invalidate(const uint32_t width, const uint32_t height) noexcept;

		[[nodiscard]]
		const Graphics::Framebuffer &getInstance(const RenderPassType type) noexcept;

	private:
		using __InstanceGenerator = std::unique_ptr<Graphics::Framebuffer>(FramebufferFactory::*)();

		Graphics::LogicalDevice &__device;
		const RenderPassFactory &__renderPassFactory;

		uint32_t __width{ };
		uint32_t __height{ };

		std::unordered_map<RenderPassType, __InstanceGenerator> __instanceGeneratorMap;
		std::unordered_map<RenderPassType, std::unique_ptr<Graphics::Framebuffer>> __instanceMap;

		[[nodiscard]]
		std::unique_ptr<Graphics::Framebuffer> __createInstance_clearColor();

		[[nodiscard]]
		std::unique_ptr<Graphics::Framebuffer> __createInstance_color();
	};
}

module: private;

namespace Engine
{
	FramebufferFactory::FramebufferFactory(
		Graphics::LogicalDevice &device, const RenderPassFactory &renderPassFactory) :
		__device				{ device },
		__renderPassFactory		{ renderPassFactory }
	{
		__instanceGeneratorMap[RenderPassType::CLEAR_COLOR]		= &FramebufferFactory::__createInstance_clearColor;
		__instanceGeneratorMap[RenderPassType::COLOR]			= &FramebufferFactory::__createInstance_color;
	}

	const Graphics::Framebuffer &FramebufferFactory::getInstance(const RenderPassType type) noexcept
	{
		auto &pRetVal{ __instanceMap[type] };
		if (!pRetVal)
			pRetVal = (this->*(__instanceGeneratorMap[type]))();

		return *pRetVal;
	}

	void FramebufferFactory::invalidate(const uint32_t width, const uint32_t height) noexcept
	{
		__width = width;
		__height = height;
		__instanceMap.clear();
	}

	std::unique_ptr<Graphics::Framebuffer> FramebufferFactory::__createInstance_clearColor()
	{
		const Graphics::Framebuffer::AttachmentInfo attachmentInfo
		{
			.format	{ VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
			.usage	{ VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT }
		};

		return std::unique_ptr<Graphics::Framebuffer>
		{
			__device.createFramebuffer(
				__renderPassFactory.getInstance(RenderPassType::CLEAR_COLOR).getHandle(),
				__width, __height, 1U, &attachmentInfo)
		};
	}

	std::unique_ptr<Graphics::Framebuffer> FramebufferFactory::__createInstance_color()
	{
		const Graphics::Framebuffer::AttachmentInfo attachmentInfo
		{
			.format	{ VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
			.usage	{ VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT }
		};

		return std::unique_ptr<Graphics::Framebuffer>
		{
			__device.createFramebuffer(
				__renderPassFactory.getInstance(RenderPassType::COLOR).getHandle(),
				__width, __height, 1U, &attachmentInfo)
		};
	}
}
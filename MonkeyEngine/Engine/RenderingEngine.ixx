module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.RenderingEngine;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.PhysicalDevice;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.Shader;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.PipelineLayout;
import ntmonkeys.com.Graphics.RenderPass;
import ntmonkeys.com.Graphics.Framebuffer;
import ntmonkeys.com.Graphics.Pipeline;
import ntmonkeys.com.Graphics.ConversionUtil;
import ntmonkeys.com.Engine.RenderTarget;
import ntmonkeys.com.Engine.CommandExecutor;
import <stdexcept>;
import <memory>;
import <concepts>;

namespace Engine
{
	export class RenderingEngine : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const Graphics::PhysicalDevice *pPhysicalDevice{ };
		};

		explicit RenderingEngine(const CreateInfo &createInfo);
		virtual ~RenderingEngine() noexcept override;

		[[nodiscard]]
		RenderTarget *createRenderTarget(const HINSTANCE hinstance, const HWND hwnd);

		[[nodiscard]]
		Graphics::Shader *createShader(const size_t codeSize, const uint32_t *const pCode);

		[[nodiscard]]
		Graphics::DescriptorSetLayout *createDescriptorSetLayout(
			const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings);

		[[nodiscard]]
		Graphics::PipelineLayout *createPipelineLayout(
			const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
			const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges);

		[[nodiscard]]
		Graphics::RenderPass *createRenderPass(
			const uint32_t attachmentCount, const VkAttachmentDescription2 *const pAttachments,
			const uint32_t subpassCount, const VkSubpassDescription2 *const pSubpasses,
			const uint32_t dependencyCount, const VkSubpassDependency2 *const pDependencies);

		[[nodiscard]]
		Graphics::Framebuffer *createFramebuffer(
			const VkRenderPass hRenderPass, const uint32_t width, const uint32_t height,
			const uint32_t attachmentCount, const Graphics::Framebuffer::AttachmentInfo *const pAttachments);

		[[nodiscard]]
		Graphics::Pipeline *createPipeline(const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo);

		[[nodiscard]]
		constexpr CommandExecutor &getCommandExecutor() noexcept;

	private:
		const Graphics::PhysicalDevice &__physicalDevice;

		std::unique_ptr<Graphics::LogicalDevice> __pLogicalDevice;
		std::unique_ptr<CommandExecutor> __pCommandExecutor;
	};

	[[nodiscard]]
	constexpr CommandExecutor &RenderingEngine::getCommandExecutor() noexcept
	{
		return *__pCommandExecutor;
	}
}

module: private;

namespace Engine
{
	RenderingEngine::RenderingEngine(const CreateInfo &createInfo) :
		__physicalDevice{ *(createInfo.pPhysicalDevice) }
	{
		__pLogicalDevice = std::unique_ptr<Graphics::LogicalDevice>{ __physicalDevice.createLogicalDevice() };
		__pCommandExecutor = std::make_unique<CommandExecutor>(*__pLogicalDevice);
	}

	RenderingEngine::~RenderingEngine() noexcept
	{
		__pCommandExecutor = nullptr;
		__pLogicalDevice = nullptr;
	}

	RenderTarget *RenderingEngine::createRenderTarget(const HINSTANCE hinstance, const HWND hwnd)
	{
		const RenderTarget::CreateInfo createInfo
		{
			.pLogicalDevice	{ __pLogicalDevice.get() },
			.hinstance		{ hinstance },
			.hwnd			{ hwnd }
		};

		return new RenderTarget{ createInfo };
	}

	Graphics::Shader *RenderingEngine::createShader(const size_t codeSize, const uint32_t *const pCode)
	{
		return __pLogicalDevice->createShader(codeSize, pCode);
	}

	Graphics::DescriptorSetLayout *RenderingEngine::createDescriptorSetLayout(
		const uint32_t bindingCount, const VkDescriptorSetLayoutBinding *const pBindings)
	{
		return __pLogicalDevice->createDescriptorSetLayout(bindingCount, pBindings);
	}

	Graphics::PipelineLayout *RenderingEngine::createPipelineLayout(
		const uint32_t setLayoutCount, const VkDescriptorSetLayout *const pSetLayouts,
		const uint32_t pushConstantRangeCount, const VkPushConstantRange *const pPushConstantRanges)
	{
		return __pLogicalDevice->createPipelineLayout(
			setLayoutCount, pSetLayouts, pushConstantRangeCount, pPushConstantRanges);
	}

	Graphics::RenderPass *RenderingEngine::createRenderPass(
		const uint32_t attachmentCount, const VkAttachmentDescription2 *const pAttachments,
		const uint32_t subpassCount, const VkSubpassDescription2 *const pSubpasses,
		const uint32_t dependencyCount, const VkSubpassDependency2 *const pDependencies)
	{
		return __pLogicalDevice->createRenderPass(
			attachmentCount, pAttachments,
			subpassCount, pSubpasses,
			dependencyCount, pDependencies);
	}

	Graphics::Framebuffer *RenderingEngine::createFramebuffer(
		const VkRenderPass hRenderPass, const uint32_t width, const uint32_t height,
		const uint32_t attachmentCount, const Graphics::Framebuffer::AttachmentInfo *const pAttachments)
	{
		return __pLogicalDevice->createFramebuffer(hRenderPass, width, height, attachmentCount, pAttachments);
	}

	Graphics::Pipeline *RenderingEngine::createPipeline(const Graphics::LogicalDevice::GraphicsPipelineCreateInfo &createInfo)
	{
		return __pLogicalDevice->createPipeline(createInfo);
	}
}
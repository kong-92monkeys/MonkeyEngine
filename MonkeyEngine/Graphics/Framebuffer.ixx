module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Framebuffer;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;
import <vector>;

namespace Graphics
{
	export class Framebuffer : public Lib::Unique
	{
	public:
		struct AttachmentInfo
		{
		public:
			VkFormat format{ };
			VkImageUsageFlags usage{ };
		};

		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkRenderPass hRenderPass{ };
			uint32_t width{ };
			uint32_t height{ };
			uint32_t attachmentCount{ };
			const AttachmentInfo *pAttachments{ };
		};

		explicit Framebuffer(const CreateInfo &createInfo);
		virtual ~Framebuffer() noexcept override;

		[[nodiscard]]
		constexpr VkFramebuffer getHandle() const noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkFramebuffer __handle{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr VkFramebuffer Framebuffer::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Framebuffer::Framebuffer(const CreateInfo &createInfo) :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	Framebuffer::~Framebuffer() noexcept
	{
		__deviceProc.vkDestroyFramebuffer(__hDevice, __handle, nullptr);
	}

	void Framebuffer::__create(const CreateInfo &createInfo)
	{
		std::vector<VkFramebufferAttachmentImageInfo> imageInfos;
		imageInfos.resize(createInfo.attachmentCount);

		for (uint32_t imageIter{ }; imageIter < createInfo.attachmentCount; ++imageIter)
		{
			const auto &externalInfo{ createInfo.pAttachments[imageIter] };

			auto &imageInfo				{ imageInfos[imageIter] };
			imageInfo.sType				= VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENT_IMAGE_INFO;
			imageInfo.usage				= externalInfo.usage;
			imageInfo.width				= createInfo.width;
			imageInfo.height			= createInfo.height;
			imageInfo.layerCount		= 1U;
			imageInfo.viewFormatCount	= 1U;
			imageInfo.pViewFormats		= &(externalInfo.format);
		}

		const VkFramebufferAttachmentsCreateInfo attachmentInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_ATTACHMENTS_CREATE_INFO },
			.attachmentImageInfoCount	{ static_cast<uint32_t>(imageInfos.size()) },
			.pAttachmentImageInfos		{ imageInfos.data() }
		};

		const VkFramebufferCreateInfo vkCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO },
			.pNext				{ &attachmentInfo },
			.flags				{ VkFramebufferCreateFlagBits::VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT },
			.renderPass			{ createInfo.hRenderPass },
			.attachmentCount	{ static_cast<uint32_t>(imageInfos.size()) },
			.width				{ createInfo.width },
			.height				{ createInfo.height },
			.layers				{ 1U }
		};

		__deviceProc.vkCreateFramebuffer(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a ImageView." };
	}
}
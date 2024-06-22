module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.RenderPass;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class RenderPass : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			uint32_t attachmentCount{ };
			const VkAttachmentDescription2 *pAttachments{ };
			uint32_t subpassCount{ };
			const VkSubpassDescription2 *pSubpasses{ };
			uint32_t dependencyCount{ };
			const VkSubpassDependency2 *pDependencies{ };
		};

		explicit RenderPass(const CreateInfo &createInfo) noexcept;
		virtual ~RenderPass() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkRenderPass __handle{ };

		void __create(const CreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	RenderPass::RenderPass(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	RenderPass::~RenderPass() noexcept
	{
		__deviceProc.vkDestroyRenderPass(__hDevice, __handle, nullptr);
	}

	void RenderPass::__create(const CreateInfo &createInfo)
	{
		const VkRenderPassCreateInfo2 vkCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2 },
			.attachmentCount			{ createInfo.attachmentCount },
			.pAttachments				{ createInfo.pAttachments },
			.subpassCount				{ createInfo.subpassCount },
			.pSubpasses					{ createInfo.pSubpasses },
			.dependencyCount			{ createInfo.dependencyCount },
			.pDependencies				{ createInfo.pDependencies }
		};

		__deviceProc.vkCreateRenderPass2(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a RenderPass." };
	}
}
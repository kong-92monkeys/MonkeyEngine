module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Layer:LayerDrawInfo;

import ntmonkeys.com.Graphics.ImageView;
import ntmonkeys.com.Engine.FramebufferFactory;
import ntmonkeys.com.Engine.CommandBufferCirculator;
import <vector>;
import <memory>;

namespace Engine
{
	export struct LayerDrawInfo
	{
	public:
		VkRect2D renderArea{ };
		VkViewport viewport{ };
		const Graphics::ImageView *pColorAttachment{ };
		FramebufferFactory *pFramebufferFactory{ };
		const std::vector<std::unique_ptr<CommandBufferCirculator>> *pSecondaryCBCirculators{ };
	};
}
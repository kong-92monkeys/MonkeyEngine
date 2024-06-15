module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Swapchain;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;
import <memory>;

namespace Graphics
{
	export class Swapchain : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkSurfaceKHR hSurface{ };
			const VkSwapchainCreateInfoKHR *vkCreateInfo{ };
		};

		Swapchain(const CreateInfo &createInfo) noexcept;

		virtual ~Swapchain() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const VkSurfaceKHR __hSurface;

		VkSwapchainKHR __handle{ };

		void __create(const VkSwapchainCreateInfoKHR &createInfo);
	};
}

module: private;

namespace Graphics
{
	Swapchain::Swapchain(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice },
		__hSurface		{ createInfo.hSurface }
	{}

	Swapchain::~Swapchain() noexcept
	{

	}

	void Swapchain::__create(const VkSwapchainCreateInfoKHR &createInfo)
	{
		// TODO: RenderTarget »ý¼º
	}
}
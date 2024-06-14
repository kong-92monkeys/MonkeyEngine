module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Surface;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;

namespace Graphics
{
	export class Surface : public Lib::Unique
	{
	public:
		Surface(
			const VK::InstanceProc &proc,
			const VkInstance hInstance,
			const VkWin32SurfaceCreateInfoKHR &createInfo) noexcept;

		virtual ~Surface() noexcept override;

	private:
		const VK::InstanceProc &__proc;
		const VkInstance __hInstance;

		VkSurfaceKHR __handle{ };

		void __create(const VkWin32SurfaceCreateInfoKHR &createInfo);
	};
}

module: private;

namespace Graphics
{
	Surface::Surface(
		const VK::InstanceProc &proc,
		const VkInstance hInstance,
		const VkWin32SurfaceCreateInfoKHR &createInfo) noexcept :
		__proc		{ proc },
		__hInstance	{ hInstance }
	{
		__create(createInfo);
	}

	Surface::~Surface() noexcept
	{
		__proc.vkDestroySurfaceKHR(__hInstance, __handle, nullptr);
	}

	void Surface::__create(const VkWin32SurfaceCreateInfoKHR &createInfo)
	{
		__proc.vkCreateWin32SurfaceKHR(__hInstance, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Surface." };
	}
}
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
	
}
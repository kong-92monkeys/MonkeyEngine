module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Queue;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;

namespace Graphics
{
	export class Queue : public Lib::Unique
	{
	public:
		Queue(const VK::DeviceProc &proc, const VkQueue handle) noexcept;

		VkResult waitIdle() noexcept;
		/*void submit();
		void present();*/

	private:
		const VK::DeviceProc &__proc;
		const VkQueue __handle;
	};
}

module: private;

namespace Graphics
{
	Queue::Queue(const VK::DeviceProc &proc, const VkQueue handle) noexcept :
		__proc{ proc }, __handle{ handle }
	{}

	VkResult Queue::waitIdle() noexcept
	{
		return __proc.vkQueueWaitIdle(__handle);
	}
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Queue;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;

namespace Graphics
{
	export class Queue : public Lib::Unique
	{
	public:
		Queue(
			const VK::DeviceProc &proc,
			const VkDevice hDevice,
			const uint32_t familyIndex,
			const uint32_t queueIndex) noexcept;

		VkResult waitIdle() noexcept;

	private:
		const VK::DeviceProc &__proc;
		const uint32_t __familyIndex;
		const uint32_t __queueIndex;

		VkQueue __handle{ };

		void __resolve(const VkDevice hDevice);
	};
}

module: private;

namespace Graphics
{
	Queue::Queue(
		const VK::DeviceProc &proc,
		const VkDevice hDevice,
		const uint32_t familyIndex,
		const uint32_t queueIndex) noexcept :
		__proc{ proc }, __familyIndex{ familyIndex }, __queueIndex{ queueIndex }
	{
		__resolve(hDevice);
	}

	VkResult Queue::waitIdle() noexcept
	{
		return __proc.vkQueueWaitIdle(__handle);
	}

	void Queue::__resolve(const VkDevice hDevice)
	{
		const VkDeviceQueueInfo2 queueInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2 },
			.queueFamilyIndex	{ __familyIndex },
			.queueIndex			{ 0U }
		};

		__proc.vkGetDeviceQueue2(hDevice, &queueInfo, &__handle);

		if (!__handle)
			throw std::runtime_error{ "Cannot resolve a queue." };
	}
}
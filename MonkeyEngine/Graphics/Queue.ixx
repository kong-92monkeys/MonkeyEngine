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
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			uint32_t familyIndex{ };
			uint32_t queueIndex{ };
		};

		Queue(const CreateInfo &createInfo) noexcept;

		VkResult waitIdle() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const uint32_t __familyIndex;
		const uint32_t __queueIndex;

		VkQueue __handle{ };

		void __retrieve(const VkDevice hDevice);
	};
}

module: private;

namespace Graphics
{
	Queue::Queue(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__familyIndex	{ createInfo.familyIndex },
		__queueIndex	{ createInfo.queueIndex }
	{
		__retrieve(createInfo.hDevice);
	}

	VkResult Queue::waitIdle() noexcept
	{
		return __deviceProc.vkQueueWaitIdle(__handle);
	}

	void Queue::__retrieve(const VkDevice hDevice)
	{
		const VkDeviceQueueInfo2 queueInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2 },
			.queueFamilyIndex	{ __familyIndex },
			.queueIndex			{ __queueIndex }
		};

		__deviceProc.vkGetDeviceQueue2(hDevice, &queueInfo, &__handle);

		if (!__handle)
			throw std::runtime_error{ "Cannot retrieve a queue." };
	}
}
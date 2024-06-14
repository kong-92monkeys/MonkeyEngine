module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.LogicalDevice;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.Queue;
import <memory>;
import <stdexcept>;

namespace Graphics
{
	export class LogicalDevice : public Lib::Unique
	{
	public:
		LogicalDevice(const VK::DeviceProc &proc, const VkDevice handle, const uint32_t queueFamilyIndex) noexcept;
		virtual ~LogicalDevice() noexcept override;
		
		[[nodiscard]]
		constexpr Queue &getQueue() noexcept;

	private:
		const VK::DeviceProc __proc;
		const VkDevice __handle;
		const uint32_t __queueFamilyIndex;

		std::unique_ptr<Queue> __pQueue;

		void __resolveQueue();
	};

	constexpr Queue &LogicalDevice::getQueue() noexcept
	{
		return *__pQueue;
	}
}

module: private;

namespace Graphics
{
	LogicalDevice::LogicalDevice(const VK::DeviceProc &proc, const VkDevice handle, const uint32_t queueFamilyIndex) noexcept :
		__proc{ proc }, __handle{ handle }, __queueFamilyIndex{ queueFamilyIndex }
	{
		__resolveQueue();
	}

	LogicalDevice::~LogicalDevice() noexcept
	{
		__proc.vkDestroyDevice(__handle, nullptr);
	}

	void LogicalDevice::__resolveQueue()
	{
		const VkDeviceQueueInfo2 queueInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2 },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.queueIndex			{ 0U }
		};

		VkQueue hQueue{ };
		__proc.vkGetDeviceQueue2(__handle, &queueInfo, &hQueue);

		if (!hQueue)
			throw std::runtime_error{ "Cannot resolve a queue." };

		__pQueue = std::make_unique<Queue>(__proc, hQueue);
	}
}
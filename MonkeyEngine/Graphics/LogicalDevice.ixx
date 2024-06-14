module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.LogicalDevice;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;

namespace Graphics
{
	export class LogicalDevice : public Lib::Unique
	{
	public:
		LogicalDevice(const VK::DeviceProc &proc, const VkDevice handle) noexcept;
		virtual ~LogicalDevice() noexcept override;

	private:
		const VK::DeviceProc __proc;
		const VkDevice __handle;
	};
}

module: private;

namespace Graphics
{
	LogicalDevice::LogicalDevice(const VK::DeviceProc &proc, const VkDevice handle) noexcept :
		__proc{ proc }, __handle{ handle }
	{}

	LogicalDevice::~LogicalDevice() noexcept
	{
		__proc.vkDestroyDevice(__handle, nullptr);
	}
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.DeviceInfo;

namespace Graphics
{
	export class DeviceInfo
	{
	public:
		explicit DeviceInfo(const VkPhysicalDevice hPhysicalDevice) noexcept;

	private:
		const VkPhysicalDevice __hPhysicalDevice;
	};

	DeviceInfo::DeviceInfo(const VkPhysicalDevice hPhysicalDevice) noexcept :
		__hPhysicalDevice{ hPhysicalDevice }
	{}
}
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
			const VkPhysicalDevice hPhysicalDevice,
			const uint32_t familyQueueIndex,
			const VkWin32SurfaceCreateInfoKHR &createInfo) noexcept;

		virtual ~Surface() noexcept override;

	private:
		const VK::InstanceProc &__proc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;
		const uint32_t __familyQueueIndex;

		VkSurfaceKHR __handle{ };

		void __create(const VkWin32SurfaceCreateInfoKHR &createInfo);
		void __checkDeviceSupport();
	};
}

module: private;

namespace Graphics
{
	Surface::Surface(
		const VK::InstanceProc &proc,
		const VkInstance hInstance,
		const VkPhysicalDevice hPhysicalDevice,
		const uint32_t familyQueueIndex,
		const VkWin32SurfaceCreateInfoKHR &createInfo) noexcept :
		__proc				{ proc },
		__hInstance			{ hInstance },
		__hPhysicalDevice	{ hPhysicalDevice },
		__familyQueueIndex	{ familyQueueIndex }
	{
		__create(createInfo);
		__checkDeviceSupport();
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

	void Surface::__checkDeviceSupport()
	{
		VkBool32 supported{ };
		__proc.vkGetPhysicalDeviceSurfaceSupportKHR(__hPhysicalDevice, __familyQueueIndex, __handle, &supported);

		if (!supported)
			throw std::runtime_error{ "The device doesn't support current surface." };
	}
}
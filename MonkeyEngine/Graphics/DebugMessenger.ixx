module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.DebugMessenger;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;

namespace Graphics
{
	export class DebugMessenger : public Lib::Unique
	{
	public:
		DebugMessenger(
			const VK::InstanceProc &proc,
			const VkInstance hInstance,
			const VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept;

		virtual ~DebugMessenger() noexcept override;

	private:
		const VK::InstanceProc &__proc;
		const VkInstance __hInstance;

		VkDebugUtilsMessengerEXT __handle{ };

		void __create(const VkDebugUtilsMessengerCreateInfoEXT &createInfo);
	};
}

module: private;

namespace Graphics
{
	DebugMessenger::DebugMessenger(
		const VK::InstanceProc &proc,
		const VkInstance hInstance,
		const VkDebugUtilsMessengerCreateInfoEXT &createInfo) noexcept :
		__proc{ proc }, __hInstance{ hInstance }
	{
		__create(createInfo);
	}

	DebugMessenger::~DebugMessenger() noexcept
	{
		__proc.vkDestroyDebugUtilsMessengerEXT(__hInstance, __handle, nullptr);
	}

	void DebugMessenger::__create(const VkDebugUtilsMessengerCreateInfoEXT &createInfo)
	{
		__proc.vkCreateDebugUtilsMessengerEXT(__hInstance, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a DebugMessenger." };
	}
}
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
		struct CreateInfo
		{
		public:
			const VK::InstanceProc *pInstanceProc{ };
			VkInstance hInstance{ };
			
			VkDebugUtilsMessageSeverityFlagsEXT messageSeverity{ };
			VkDebugUtilsMessageTypeFlagsEXT messageType{ };
			PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback{ };
			void *pUserData{ };
		};

		DebugMessenger(const CreateInfo &createInfo) noexcept;

		virtual ~DebugMessenger() noexcept override;

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;

		VkDebugUtilsMessengerEXT __handle{ };

		void __create(
			const VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageType,
			const PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback,
			void *const pUserData);
	};
}

module: private;

namespace Graphics
{
	DebugMessenger::DebugMessenger(const CreateInfo &createInfo) noexcept :
		__instanceProc	{ *(createInfo.pInstanceProc) },
		__hInstance		{ createInfo.hInstance }
	{
		__create(
			createInfo.messageSeverity, createInfo.messageType,
			createInfo.pfnUserCallback, createInfo.pUserData);
	}

	DebugMessenger::~DebugMessenger() noexcept
	{
		__instanceProc.vkDestroyDebugUtilsMessengerEXT(__hInstance, __handle, nullptr);
	}

	void DebugMessenger::__create(
		const VkDebugUtilsMessageSeverityFlagsEXT messageSeverity,
		const VkDebugUtilsMessageTypeFlagsEXT messageType,
		const PFN_vkDebugUtilsMessengerCallbackEXT pfnUserCallback,
		void *const pUserData)
	{
		const VkDebugUtilsMessengerCreateInfoEXT createInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT },
			.messageSeverity	{ messageSeverity },
			.messageType		{ messageType },
			.pfnUserCallback	{ pfnUserCallback },
			.pUserData			{ pUserData }
		};

		__instanceProc.vkCreateDebugUtilsMessengerEXT(__hInstance, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a DebugMessenger." };
	}
}
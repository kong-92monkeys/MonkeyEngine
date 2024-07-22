module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.Core;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.Logger;
import ntmonkeys.com.Lib.Version;
import ntmonkeys.com.Graphics.RenderContext;
import ntmonkeys.com.Engine.RenderingEngine;
import <memory>;
import <string>;

namespace Engine
{
	export class Core : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			std::string vulkanLoaderLibName;

			std::string appName;
			Lib::Version appVersion;

			std::string engineName;
			Lib::Version engineVersion;
		};

		explicit Core(const CreateInfo &createInfo);
		virtual ~Core() noexcept;

		[[nodiscard]]
		RenderingEngine *createEngine();

	private:
		std::unique_ptr<Graphics::RenderContext> __pRenderContext;

		void __createRenderContext(const CreateInfo &createInfo);

		static VkBool32 __vkDebugUtilsMessengerCallbackEXT(
			const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			const VkDebugUtilsMessageTypeFlagsEXT messageTypes,
			const VkDebugUtilsMessengerCallbackDataEXT *const pCallbackData,
			void *const pUserData) noexcept;
	};
}

module: private;

namespace Engine
{
	Core::Core(const CreateInfo &createInfo)
	{
		__createRenderContext(createInfo);
	}

	Core::~Core() noexcept
	{
		__pRenderContext = nullptr;
	}

	RenderingEngine *Core::createEngine()
    {
        const auto &devices{ __pRenderContext->getPhysicalDevices() };

        const RenderingEngine::CreateInfo createInfo
        {
            .pPhysicalDevice{ devices.front().get() },
        };

		return new RenderingEngine{ createInfo };
    }

	void Core::__createRenderContext(const CreateInfo &createInfo)
	{
		const Graphics::RenderContext::CreateInfo renderContextCreateInfo
		{
			.vulkanLoaderLibName	{ createInfo.vulkanLoaderLibName },

#ifndef NDEBUG
			.debugMode				{ true },
			.debugCallback			{ __vkDebugUtilsMessengerCallbackEXT },
#endif

			.appName				{ createInfo.appName },
			.appVersion				{ createInfo.appVersion },
			.engineName				{ createInfo.engineName },
			.engineVersion			{ createInfo.engineVersion }
		};

		__pRenderContext = std::make_unique<Graphics::RenderContext>(renderContextCreateInfo);
	}

	VkBool32 Core::__vkDebugUtilsMessengerCallbackEXT(
		const VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		const VkDebugUtilsMessageTypeFlagsEXT messageTypes,
		const VkDebugUtilsMessengerCallbackDataEXT *const pCallbackData,
		void *const pUserData) noexcept
	{
		Lib::Logger::Severity logSeverity{ };

		switch (messageSeverity)
		{
			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
				logSeverity = Lib::Logger::Severity::FATAL;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
				logSeverity = Lib::Logger::Severity::WARNING;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
				logSeverity = Lib::Logger::Severity::INFO;
				break;

			case VkDebugUtilsMessageSeverityFlagBitsEXT::VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
				logSeverity = Lib::Logger::Severity::VERBOSE;
				break;
		}

		Lib::Logger::log(logSeverity, pCallbackData->pMessage);
		return VK_FALSE;
	}
}
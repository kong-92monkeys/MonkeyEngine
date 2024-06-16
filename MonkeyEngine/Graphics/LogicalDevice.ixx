module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.LogicalDevice;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import ntmonkeys.com.Graphics.Queue;
import ntmonkeys.com.Graphics.PipelineCache;
import ntmonkeys.com.Graphics.Surface;
import <vector>;
import <memory>;
import <stdexcept>;

namespace Graphics
{
	export class LogicalDevice : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::InstanceProc *pInstanceProc{ };
			VkInstance hInstance{ };
			VkPhysicalDevice hPhysicalDevice{ };
			uint32_t queueFamilyIndex{ };
			const VkPhysicalDeviceFeatures2 *pFeatures{ };
			const std::vector<const char *> *pExtensions{ };
		};

		explicit LogicalDevice(const CreateInfo &createInfo) noexcept;
		virtual ~LogicalDevice() noexcept override;
		
		[[nodiscard]]
		constexpr Queue &getQueue() noexcept;

		[[nodiscard]]
		std::unique_ptr<PipelineCache> createPipelineCache();

		[[nodiscard]]
		std::unique_ptr<Surface> createSurface(const HINSTANCE hAppInstance, const HWND hwnd);

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;
		const uint32_t __queueFamilyIndex;

		VkDevice __handle{ };
		VK::DeviceProc __deviceProc;

		std::unique_ptr<Queue> __pQueue;

		void __create(const VkPhysicalDeviceFeatures2 &features, const std::vector<const char *> &extensions);
		void __loadDeviceProc() noexcept;
		void __resolveQueue();
	};

	constexpr Queue &LogicalDevice::getQueue() noexcept
	{
		return *__pQueue;
	}
}

module: private;

#pragma warning(disable: 5103)
#define LOAD_DEVICE_PROC(funcName) (__deviceProc.##funcName = reinterpret_cast<PFN_##funcName>(__instanceProc.vkGetDeviceProcAddr(__handle, #funcName)))

namespace Graphics
{
	LogicalDevice::LogicalDevice(const CreateInfo &createInfo) noexcept :
		__instanceProc		{ *(createInfo.pInstanceProc) },
		__hInstance			{ createInfo.hInstance },
		__hPhysicalDevice	{ createInfo.hPhysicalDevice },
		__queueFamilyIndex	{ createInfo.queueFamilyIndex }
	{
		__create(*(createInfo.pFeatures), *(createInfo.pExtensions));
		__loadDeviceProc();
		__resolveQueue();
	}

	LogicalDevice::~LogicalDevice() noexcept
	{
		__deviceProc.vkDestroyDevice(__handle, nullptr);
	}

	std::unique_ptr<PipelineCache> LogicalDevice::createPipelineCache()
	{
		const PipelineCache::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle }
		};

		return std::make_unique<PipelineCache>(createInfo);
	}

	std::unique_ptr<Surface> LogicalDevice::createSurface(const HINSTANCE hAppInstance, const HWND hwnd)
	{
		const Surface::CreateInfo createInfo
		{
			.pInstanceProc		{ &__instanceProc },
			.hInstance			{ __hInstance },
			.hPhysicalDevice	{ __hPhysicalDevice },
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.hAppInstance		{ hAppInstance },
			.hwnd				{ hwnd }
		};

		return std::make_unique<Surface>(createInfo);
	}

	void LogicalDevice::__create(const VkPhysicalDeviceFeatures2 &features, const std::vector<const char *> &extensions)
	{
		static constexpr float queuePriority{ 1.0f };

		const VkDeviceQueueCreateInfo queueCreateInfo
		{
			.sType				{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.queueCount			{ 1U },
			.pQueuePriorities	{ &queuePriority }
		};

		const VkDeviceCreateInfo createInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO },
			.pNext						{ &features },
			.queueCreateInfoCount		{ 1U },
			.pQueueCreateInfos			{ &queueCreateInfo },
			.enabledExtensionCount		{ static_cast<uint32_t>(extensions.size()) },
			.ppEnabledExtensionNames	{ extensions.data() }
		};

		__instanceProc.vkCreateDevice(__hPhysicalDevice, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a logical device." };
	}

	void LogicalDevice::__loadDeviceProc() noexcept
	{
		// Device
		LOAD_DEVICE_PROC(vkDeviceWaitIdle);
		LOAD_DEVICE_PROC(vkDestroyDevice);

		// Queue
		LOAD_DEVICE_PROC(vkGetDeviceQueue2);
		LOAD_DEVICE_PROC(vkQueueWaitIdle);
		LOAD_DEVICE_PROC(vkQueueSubmit2);
		LOAD_DEVICE_PROC(vkQueuePresentKHR);

		// Pipeline cache
		LOAD_DEVICE_PROC(vkCreatePipelineCache);
		LOAD_DEVICE_PROC(vkDestroyPipelineCache);

		// Swapchain
		LOAD_DEVICE_PROC(vkCreateSwapchainKHR);
		LOAD_DEVICE_PROC(vkDestroySwapchainKHR);
		LOAD_DEVICE_PROC(vkGetSwapchainImagesKHR);

		LOAD_DEVICE_PROC(vkCreateImageView);
		LOAD_DEVICE_PROC(vkDestroyImageView);
	}

	void LogicalDevice::__resolveQueue()
	{
		const Queue::CreateInfo createInfo
		{
			.pDeviceProc	{ &__deviceProc },
			.hDevice		{ __handle },
			.familyIndex	{__queueFamilyIndex },
			.queueIndex		{ 0U }
		};

		__pQueue = std::make_unique<Queue>(createInfo);
	}
}
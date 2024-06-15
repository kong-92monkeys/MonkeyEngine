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
		LogicalDevice(
			const VK::InstanceProc &instanceProc,
			const VkInstance hInstance,
			const VkPhysicalDevice hPhysicalDevice,
			const uint32_t queueFamilyIndex,
			const VkPhysicalDeviceFeatures2 &features,
			const std::vector<const char *> &extensions) noexcept;

		virtual ~LogicalDevice() noexcept override;
		
		[[nodiscard]]
		constexpr Queue &getQueue() noexcept;

		[[nodiscard]]
		std::unique_ptr<PipelineCache> createPipelineCache(const VkPipelineCacheCreateInfo &createInfo);

		[[nodiscard]]
		std::unique_ptr<Surface> createSurface(const VkWin32SurfaceCreateInfoKHR &vkCreateInfo);

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
	LogicalDevice::LogicalDevice(
		const VK::InstanceProc &instanceProc,
		const VkInstance hInstance,
		const VkPhysicalDevice hPhysicalDevice,
		const uint32_t queueFamilyIndex,
		const VkPhysicalDeviceFeatures2 &features,
		const std::vector<const char *> &extensions) noexcept :
		__instanceProc		{ instanceProc },
		__hInstance			{ hInstance },
		__hPhysicalDevice	{ hPhysicalDevice },
		__queueFamilyIndex	{ queueFamilyIndex }
	{
		__create(features, extensions);
		__loadDeviceProc();
		__resolveQueue();
	}

	LogicalDevice::~LogicalDevice() noexcept
	{
		__deviceProc.vkDestroyDevice(__handle, nullptr);
	}

	std::unique_ptr<PipelineCache> LogicalDevice::createPipelineCache(const VkPipelineCacheCreateInfo &createInfo)
	{
		return std::make_unique<PipelineCache>(__deviceProc, __handle, createInfo);
	}

	std::unique_ptr<Surface> LogicalDevice::createSurface(const VkWin32SurfaceCreateInfoKHR &vkCreateInfo)
	{
		const Surface::CreateInfo createInfo
		{
			.pInstanceProc		{ &__instanceProc },
			.hInstance			{ __hInstance },
			.hPhysicalDevice	{ __hPhysicalDevice },
			.pDeviceProc		{ &__deviceProc },
			.hDevice			{ __handle },
			.queueFamilyIndex	{ __queueFamilyIndex },
			.vkCreateInfo		{ &vkCreateInfo }
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
	}

	void LogicalDevice::__resolveQueue()
	{
		__pQueue = std::make_unique<Queue>(__deviceProc, __handle, __queueFamilyIndex, 0U);
	}
}
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
		std::unique_ptr<Surface> createSurface(const VkWin32SurfaceCreateInfoKHR &createInfo);

	private:
		const VK::InstanceProc &__instanceProc;
		const VkInstance __hInstance;
		const VkPhysicalDevice __hPhysicalDevice;
		const uint32_t __queueFamilyIndex;

		VkDevice __handle{ };
		VK::DeviceProc __proc;

		std::unique_ptr<Queue> __pQueue;

		void __create(const VkPhysicalDeviceFeatures2 &features, const std::vector<const char *> &extensions);
		void __loadProc() noexcept;
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
		__loadProc();
		__resolveQueue();
	}

	LogicalDevice::~LogicalDevice() noexcept
	{
		__proc.vkDestroyDevice(__handle, nullptr);
	}

	std::unique_ptr<PipelineCache> LogicalDevice::createPipelineCache(const VkPipelineCacheCreateInfo &createInfo)
	{
		return std::make_unique<PipelineCache>(__proc, __handle, createInfo);
	}

	std::unique_ptr<Surface> LogicalDevice::createSurface(const VkWin32SurfaceCreateInfoKHR &createInfo)
	{
		return std::make_unique<Surface>(__instanceProc, __hInstance, __hPhysicalDevice, __queueFamilyIndex, createInfo);
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

	void LogicalDevice::__loadProc() noexcept
	{
		__proc.vkDeviceWaitIdle =
			reinterpret_cast<PFN_vkDeviceWaitIdle>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkDeviceWaitIdle"));

		__proc.vkDestroyDevice =
			reinterpret_cast<PFN_vkDestroyDevice>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkDestroyDevice"));

		__proc.vkGetDeviceQueue2 =
			reinterpret_cast<PFN_vkGetDeviceQueue2>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkGetDeviceQueue2"));

		__proc.vkQueueWaitIdle =
			reinterpret_cast<PFN_vkQueueWaitIdle>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkQueueWaitIdle"));

		__proc.vkQueueSubmit2 =
			reinterpret_cast<PFN_vkQueueSubmit2>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkQueueSubmit2"));

		__proc.vkQueuePresentKHR =
			reinterpret_cast<PFN_vkQueuePresentKHR>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkQueuePresentKHR"));

		__proc.vkCreatePipelineCache =
			reinterpret_cast<PFN_vkCreatePipelineCache>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkCreatePipelineCache"));

		__proc.vkDestroyPipelineCache =
			reinterpret_cast<PFN_vkDestroyPipelineCache>(
				__instanceProc.vkGetDeviceProcAddr(__handle, "vkDestroyPipelineCache"));
	}

	void LogicalDevice::__resolveQueue()
	{
		__pQueue = std::make_unique<Queue>(__proc, __handle, __queueFamilyIndex, 0U);
	}
}
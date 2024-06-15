module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.PipelineCache;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;

namespace Graphics
{
	export class PipelineCache : public Lib::Unique
	{
	public:
		PipelineCache(
			const VK::DeviceProc &deviceProc,
			const VkDevice hDevice,
			const VkPipelineCacheCreateInfo &createInfo) noexcept;

		virtual ~PipelineCache() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkPipelineCache __handle{ };

		void __create(const VkPipelineCacheCreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	PipelineCache::PipelineCache(
		const VK::DeviceProc &deviceProc,
		const VkDevice hDevice,
		const VkPipelineCacheCreateInfo &createInfo) noexcept :
		__deviceProc{ deviceProc }, __hDevice{ hDevice }
	{
		__create(createInfo);
	}

	PipelineCache::~PipelineCache() noexcept
	{
		__deviceProc.vkDestroyPipelineCache(__hDevice, __handle, nullptr);
	}

	void PipelineCache::__create(const VkPipelineCacheCreateInfo &createInfo)
	{
		__deviceProc.vkCreatePipelineCache(__hDevice, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a VkPipelineCache." };
	}
}
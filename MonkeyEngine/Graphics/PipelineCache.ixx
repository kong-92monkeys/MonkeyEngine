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
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
		};

		explicit PipelineCache(const CreateInfo &createInfo) noexcept;
		virtual ~PipelineCache() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkPipelineCache __handle{ };

		void __create();
	};
}

module: private;

namespace Graphics
{
	PipelineCache::PipelineCache(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create();
	}

	PipelineCache::~PipelineCache() noexcept
	{
		__deviceProc.vkDestroyPipelineCache(__hDevice, __handle, nullptr);
	}

	void PipelineCache::__create()
	{
		const VkPipelineCacheCreateInfo createInfo
		{
			.sType{ VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO }
		};

		__deviceProc.vkCreatePipelineCache(__hDevice, &createInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a VkPipelineCache." };
	}
}
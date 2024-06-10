module;

#include <cstdint>
#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.ConversionUtil;

import ntmonkeys.com.Lib.Version;

namespace Graphics
{
	export namespace ConversionUtil
	{
		constexpr Lib::Version fromVulkanVersion(const uint32_t version) noexcept
		{
			return
			{
				.major		{ VK_API_VERSION_MAJOR(version) },
				.minor		{ VK_API_VERSION_MINOR(version) },
				.patch		{ VK_API_VERSION_PATCH(version) },
				.variant	{ VK_API_VERSION_VARIANT(version) }
			};
		}
	};
}
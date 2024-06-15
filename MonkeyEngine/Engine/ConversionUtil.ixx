module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.ConversionUtil;

import ntmonkeys.com.Lib.Version;

namespace Engine
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

		constexpr uint32_t toVulkanVersion(const Lib::Version &version) noexcept
		{
			return VK_MAKE_API_VERSION(version.variant, version.major, version.minor, version.patch);
		}
	};
}
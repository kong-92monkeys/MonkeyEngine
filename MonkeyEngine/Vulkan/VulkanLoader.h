#pragma once

#include "Vulkan.h"
#include "../Library/Unique.h"

namespace VK
{
	class VulkanLoader : public Lib::Unique
	{
	public:
		virtual ~VulkanLoader() noexcept;

		[[nodiscard]]
		constexpr bool isVulkanSupported() const noexcept;

		[[nodiscard]]
		static VulkanLoader &__getInstance() noexcept;

	private:
		HMODULE __hLib{ };
		static constexpr auto __libName{ "vulkan-1.dll" };

		VulkanLoader() noexcept;
	};

	constexpr bool VulkanLoader::isVulkanSupported() const noexcept
	{
		return __hLib;
	}
}

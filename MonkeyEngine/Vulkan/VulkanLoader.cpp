#include "VulkanLoader.h"

namespace VK
{
	VulkanLoader::VulkanLoader() noexcept
	{
		__hLib = LoadLibraryA(__libName);
	}

	VulkanLoader::~VulkanLoader() noexcept
	{
		if (__hLib)
			FreeLibrary(__hLib);
	}

	VulkanLoader &VulkanLoader::__getInstance() noexcept
	{
		static VulkanLoader instance;
		return instance;
	}
}
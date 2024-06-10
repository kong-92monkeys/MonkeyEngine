module;

#include <Windows.h>

export module ntmonkeys.com.VK.VulkanLoader;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;

namespace VK
{
	export class VulkanLoader : public Lib::Unique
	{
	public:
		virtual ~VulkanLoader() noexcept;

		[[nodiscard]]
		constexpr bool isVulkanSupported() const noexcept;

		[[nodiscard]]
		static VulkanLoader &__getInstance() noexcept;

	private:
		HMODULE __hLib{ };
		GlobalProc __globalProc;

		static constexpr auto __libName{ "vulkan-1.dll" };

		VulkanLoader() noexcept;

		void __loadGlobalProc() noexcept;
	};

	VulkanLoader::VulkanLoader() noexcept
	{
		__hLib = LoadLibraryA(__libName);
		if (!__hLib)
			return;

		__loadGlobalProc();
	}

	VulkanLoader::~VulkanLoader() noexcept
	{
		if (__hLib)
			FreeLibrary(__hLib);
	}

	constexpr bool VulkanLoader::isVulkanSupported() const noexcept
	{
		return __hLib;
	}

	VulkanLoader &VulkanLoader::__getInstance() noexcept
	{
		static VulkanLoader instance;
		return instance;
	}

	void VulkanLoader::__loadGlobalProc() noexcept
	{
		const auto vkGetInstanceProcAddr
		{
			reinterpret_cast<PFN_vkGetInstanceProcAddr>(
				GetProcAddress(__hLib, "vkGetInstanceProcAddr"))
		};

		__globalProc.vkGetInstanceProcAddr = vkGetInstanceProcAddr;

		__globalProc.vkEnumerateInstanceVersion =
			reinterpret_cast<PFN_vkEnumerateInstanceVersion>(
				vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

		__globalProc.vkEnumerateInstanceExtensionProperties =
			reinterpret_cast<PFN_vkEnumerateInstanceExtensionProperties>(
				vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceExtensionProperties"));

		__globalProc.vkEnumerateInstanceLayerProperties =
			reinterpret_cast<PFN_vkEnumerateInstanceLayerProperties>(
				vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceLayerProperties"));
	}
}

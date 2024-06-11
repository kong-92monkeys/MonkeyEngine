module;

#include <Windows.h>

export module ntmonkeys.com.VK.VulkanLoader;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <string>;
import <string_view>;

namespace VK
{
	export class VulkanLoader : public Lib::Unique
	{
	public:
		VulkanLoader(const std::string_view &libName) noexcept;
		virtual ~VulkanLoader() noexcept;

		[[nodiscard]]
		constexpr bool isVulkanSupported() const noexcept;

		[[nodiscard]]
		constexpr const GlobalProc &getGlobalProc() const noexcept;

	private:
		const std::string __libName;

		HMODULE __hLib{ };
		GlobalProc __globalProc;

		void __loadGlobalProc() noexcept;
	};

	VulkanLoader::VulkanLoader(const std::string_view &libName) noexcept :
		__libName{ libName }
	{
		__hLib = LoadLibraryA(__libName.c_str());
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

	constexpr const GlobalProc &VulkanLoader::getGlobalProc() const noexcept
	{
		return __globalProc;
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

		__globalProc.vkCreateInstance =
			reinterpret_cast<PFN_vkCreateInstance>(
				vkGetInstanceProcAddr(nullptr, "vkCreateInstance"));
	}
}
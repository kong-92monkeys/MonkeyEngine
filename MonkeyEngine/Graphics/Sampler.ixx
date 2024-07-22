module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Sampler;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <stdexcept>;

namespace Graphics
{
	export class Sampler : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkSamplerCreateFlags flags{ };
			VkFilter magFilter{ };
			VkFilter minFilter{ };
			VkSamplerMipmapMode mipmapMode{ };
			VkSamplerAddressMode addressModeU{ };
			VkSamplerAddressMode addressModeV{ };
			VkSamplerAddressMode addressModeW{ };
			float mipLodBias{ };
			VkBool32 anisotropyEnable{ };
			float maxAnisotropy{ };
			VkBool32 compareEnable{ };
			VkCompareOp compareOp{ };
			float minLod{ };
			float maxLod{ };
			VkBorderColor borderColor{ };
			VkBool32 unnormalizedCoordinates{ };
		};

		explicit Sampler(const CreateInfo &createInfo);
		virtual ~Sampler() noexcept override;

		[[nodiscard]]
		constexpr const VkSampler &getHandle() const noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkSampler __handle{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr const VkSampler &Sampler::getHandle() const noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Sampler::Sampler(const CreateInfo &createInfo) :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	Sampler::~Sampler() noexcept
	{
		__deviceProc.vkDestroySampler(__hDevice, __handle, nullptr);
	}

	void Sampler::__create(const CreateInfo &createInfo)
	{
		const VkSamplerCreateInfo vkCreateInfo
		{
			.sType						{ VkStructureType::VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO },
			.flags						{ createInfo.flags },
			.magFilter					{ createInfo.magFilter },
			.minFilter					{ createInfo.minFilter },
			.mipmapMode					{ createInfo.mipmapMode },
			.addressModeU				{ createInfo.addressModeU },
			.addressModeV				{ createInfo.addressModeV },
			.addressModeW				{ createInfo.addressModeW },
			.mipLodBias					{ createInfo.mipLodBias },
			.anisotropyEnable			{ createInfo.anisotropyEnable },
			.maxAnisotropy				{ createInfo.maxAnisotropy },
			.compareEnable				{ createInfo.compareEnable },
			.compareOp					{ createInfo.compareOp },
			.minLod						{ createInfo.minLod },
			.maxLod						{ createInfo.maxLod },
			.borderColor				{ createInfo.borderColor },
			.unnormalizedCoordinates	{ createInfo.unnormalizedCoordinates }
		};

		__deviceProc.vkCreateSampler(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Sampler." };
	}
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Shader;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class Shader : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			const std::vector<uint32_t> *pCode;
		};

		explicit Shader(const CreateInfo &createInfo) noexcept;
		virtual ~Shader() noexcept override;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkShaderModule __handle{ };

		void __create(const CreateInfo &createInfo);
	};
}

module: private;

namespace Graphics
{
	Shader::Shader(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	Shader::~Shader() noexcept
	{
		__deviceProc.vkDestroyShaderModule(__hDevice, __handle, nullptr);
	}

	void Shader::__create(const CreateInfo &createInfo)
	{
		const VkShaderModuleCreateInfo vkCreateInfo
		{
			.sType		{ VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO },
			.codeSize	{ createInfo.pCode->size() * sizeof(uint32_t) },
			.pCode		{ createInfo.pCode->data() }
		};

		__deviceProc.vkCreateShaderModule(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a ShaderModule." };
	}
}
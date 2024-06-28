module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Semaphore;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class Semaphore : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			VkSemaphoreType type{ };
			uint64_t initialValue{ };
		};

		explicit Semaphore(const CreateInfo &createInfo) noexcept;
		virtual ~Semaphore() noexcept override;

		[[nodiscard]]
		constexpr VkSemaphoreType getType() const noexcept;

		[[nodiscard]]
		constexpr VkSemaphore getHandle() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;
		const VkSemaphoreType __type;

		VkSemaphore __handle{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr VkSemaphoreType Semaphore::getType() const noexcept
	{
		return __type;
	}

	constexpr VkSemaphore Semaphore::getHandle() noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Semaphore::Semaphore(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice },
		__type			{ createInfo.type }
	{
		__create(createInfo);
	}

	Semaphore::~Semaphore() noexcept
	{
		__deviceProc.vkDestroySemaphore(__hDevice, __handle, nullptr);
	}

	void Semaphore::__create(const CreateInfo &createInfo)
	{
		const VkSemaphoreTypeCreateInfo typeInfo
		{
			.sType			{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO },
			.semaphoreType	{ createInfo.type },
			.initialValue	{ createInfo.initialValue }
		};

		const VkSemaphoreCreateInfo vkCreateInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO },
			.pNext	{ &typeInfo }
		};

		__deviceProc.vkCreateSemaphore(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Semaphore." };
	}
}
module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Graphics.Fence;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.VK.VulkanProc;
import <vector>;
import <stdexcept>;

namespace Graphics
{
	export class Fence : public Lib::Unique
	{
	public:
		struct CreateInfo
		{
		public:
			const VK::DeviceProc *pDeviceProc{ };
			VkDevice hDevice{ };
			bool signaled{ };
		};

		explicit Fence(const CreateInfo &createInfo) noexcept;
		virtual ~Fence() noexcept override;

		VkResult wait(const uint64_t timeout) noexcept;
		VkResult reset();

		[[nodiscard]]
		constexpr const VkFence &getHandle() noexcept;

	private:
		const VK::DeviceProc &__deviceProc;
		const VkDevice __hDevice;

		VkFence __handle{ };

		void __create(const CreateInfo &createInfo);
	};

	constexpr const VkFence &Fence::getHandle() noexcept
	{
		return __handle;
	}
}

module: private;

namespace Graphics
{
	Fence::Fence(const CreateInfo &createInfo) noexcept :
		__deviceProc	{ *(createInfo.pDeviceProc) },
		__hDevice		{ createInfo.hDevice }
	{
		__create(createInfo);
	}

	Fence::~Fence() noexcept
	{
		__deviceProc.vkDestroyFence(__hDevice, __handle, nullptr);
	}

	VkResult Fence::wait(const uint64_t timeout) noexcept
	{
		return __deviceProc.vkWaitForFences(__hDevice, 1U, &__handle, VK_FALSE, timeout);
	}

	VkResult Fence::reset() 
	{
		return __deviceProc.vkResetFences(__hDevice, 1U, &__handle);
	}

	void Fence::__create(const CreateInfo &createInfo)
	{
		const VkFenceCreateInfo vkCreateInfo
		{
			.sType	{ VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO },
			.flags	{ createInfo.signaled ? VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT : 0U }
		};

		__deviceProc.vkCreateFence(__hDevice, &vkCreateInfo, nullptr, &__handle);
		if (!__handle)
			throw std::runtime_error{ "Cannot create a Fence." };
	}
}
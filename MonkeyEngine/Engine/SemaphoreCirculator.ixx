module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.SemaphoreCirculator;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.Semaphore;
import ntmonkeys.com.Graphics.LogicalDevice;
import <vector>;
import <memory>;

namespace Engine
{
	export class SemaphoreCirculator : public Lib::Unique
	{
	public:
		SemaphoreCirculator(Graphics::LogicalDevice &device, const VkSemaphoreType type, const size_t count);
		virtual ~SemaphoreCirculator() noexcept override = default;

		[[nodiscard]]
		constexpr Graphics::Semaphore &getNext() noexcept;

	private:
		std::vector<std::unique_ptr<Graphics::Semaphore>> __semaphores;
		size_t __index{ };
	};

	constexpr Graphics::Semaphore &SemaphoreCirculator::getNext() noexcept
	{
		auto &retVal{ *(__semaphores[__index]) };
		__index = ((__index + 1ULL) % __semaphores.size());
		return retVal;
	}
}

module: private;

namespace Engine
{
	SemaphoreCirculator::SemaphoreCirculator(Graphics::LogicalDevice &device, const VkSemaphoreType type, const size_t count)
	{
		for (size_t iter{ }; iter < count; ++iter)
			__semaphores.emplace_back(device.createSemaphore(type, 0U));
	}
}
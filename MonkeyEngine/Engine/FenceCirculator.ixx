module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.FenceCirculator;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.Fence;
import ntmonkeys.com.Graphics.LogicalDevice;
import <vector>;
import <memory>;

namespace Engine
{
	export class FenceCirculator : public Lib::Unique
	{
	public:
		FenceCirculator(Graphics::LogicalDevice &device, const size_t count);
		virtual ~FenceCirculator() noexcept override = default;

		[[nodiscard]]
		constexpr Graphics::Fence &getNext() noexcept;

	private:
		std::vector<std::unique_ptr<Graphics::Fence>> __fences;
		size_t __index{ };
	};

	constexpr Graphics::Fence &FenceCirculator::getNext() noexcept
	{
		auto &retVal{ *(__fences[__index]) };
		__index = ((__index + 1ULL) % __fences.size());
		return retVal;
	}
}

module: private;

namespace Engine
{
	FenceCirculator::FenceCirculator(Graphics::LogicalDevice &device, const size_t count)
	{
		for (size_t iter{ }; iter < count; ++iter)
			__fences.emplace_back(device.createFence(true));
	}
}
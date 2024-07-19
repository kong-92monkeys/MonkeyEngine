module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.DescriptorSetCirculator;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.DescriptorPool;
import ntmonkeys.com.Graphics.DescriptorSet;
import <vector>;
import <memory>;

namespace Engine
{
	export class DescriptorSetCirculator : public Lib::Unique
	{
	public:
		DescriptorSetCirculator(Graphics::LogicalDevice &logicalDevice, const Graphics::DescriptorSetLayout &layout, const uint32_t setCount);

		[[nodiscard]]
		Graphics::DescriptorSet &getNext();

	private:
		const uint32_t __setCount;
		uint32_t __setIdx{ };

		std::unique_ptr<Graphics::DescriptorPool> __pPool;
		std::vector<std::unique_ptr<Graphics::DescriptorSet>> __sets;

		void __create(Graphics::LogicalDevice &logicalDevice, const Graphics::DescriptorSetLayout &layout);
		constexpr void __advanceIndex();
	};

	constexpr void DescriptorSetCirculator::__advanceIndex()
	{
		__setIdx = ((__setIdx + 1U) % __setCount);
	}
}

module: private;

namespace Engine
{
	DescriptorSetCirculator::DescriptorSetCirculator(
		Graphics::LogicalDevice &logicalDevice, const Graphics::DescriptorSetLayout &layout, const uint32_t setCount) :
		__setCount{ setCount }
	{
		__create(logicalDevice, layout);
	}

	Graphics::DescriptorSet &DescriptorSetCirculator::getNext()
	{
		__advanceIndex();
		return *(__sets[__setIdx]);
	}

	void DescriptorSetCirculator::__create(Graphics::LogicalDevice &logicalDevice, const Graphics::DescriptorSetLayout &layout)
	{
		std::vector<VkDescriptorPoolSize> poolSizes;

		for (const auto &[type, count] : layout.getDescriptorCountInfo())
			poolSizes.emplace_back(type, count);

		__pPool = std::unique_ptr<Graphics::DescriptorPool>
		{
			logicalDevice.createDescriptorPool(
				0U, __setCount, static_cast<uint32_t>(poolSizes.size()), poolSizes.data())
		};

		for (uint32_t setIter{ }; setIter < __setCount; ++setIter)
			__sets.emplace_back(__pPool->allocateDescriptorSet(layout.getHandle()));
	}
}
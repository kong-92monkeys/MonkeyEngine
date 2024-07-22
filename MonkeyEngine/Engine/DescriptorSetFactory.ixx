module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.DescriptorSetFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.DescriptorPool;
import ntmonkeys.com.Graphics.DescriptorSet;
import <vector>;
import <memory>;

namespace Engine
{
	export class DescriptorSetFactory : public Lib::Unique
	{
	public:
		DescriptorSetFactory(
			Graphics::LogicalDevice &logicalDevice,
			const std::vector<VkDescriptorPoolSize> &poolSizes,
			const uint32_t poolCount, const uint32_t maxSetCount);

		[[nodiscard]]
		Graphics::DescriptorSet *createInstance(
			const VkDescriptorSetLayout hLayout, const uint32_t *const pVariableDescriptorCount);

	private:
		const uint32_t __poolCount;

		uint32_t __poolIdx{ };
		std::vector<std::unique_ptr<Graphics::DescriptorPool>> __pools;

		void __create(
			Graphics::LogicalDevice &logicalDevice,
			const std::vector<VkDescriptorPoolSize> &poolSizes,
			const uint32_t maxSetCount);

		constexpr void __advancePoolIndex();
	};

	constexpr void DescriptorSetFactory::__advancePoolIndex()
	{
		__poolIdx = ((__poolIdx + 1U) % __poolCount);
	}
}

module: private;

namespace Engine
{
	DescriptorSetFactory::DescriptorSetFactory(
		Graphics::LogicalDevice &logicalDevice,
		const std::vector<VkDescriptorPoolSize> &poolSizes,
		const uint32_t poolCount, const uint32_t maxSetCount) :
		__poolCount{ poolCount }
	{
		__create(logicalDevice, poolSizes, maxSetCount);
	}

	Graphics::DescriptorSet *DescriptorSetFactory::createInstance(
		const VkDescriptorSetLayout hLayout, const uint32_t *const pVariableDescriptorCount)
	{
		Graphics::DescriptorSet *pRetVal{ };

		try
		{
			const auto &pPool{ __pools[__poolIdx] };
			pRetVal = pPool->allocateDescriptorSet(hLayout, pVariableDescriptorCount);
		}
		catch (const Graphics::DescriptorSet::CreationException &)
		{
			__advancePoolIndex();

			const auto &pPool{ __pools[__poolIdx] };
			pPool->reset();

			pRetVal = pPool->allocateDescriptorSet(hLayout, pVariableDescriptorCount);
		}

		return pRetVal;
	}

	void DescriptorSetFactory::__create(
		Graphics::LogicalDevice &logicalDevice, const std::vector<VkDescriptorPoolSize> &poolSizes, const uint32_t maxSetCount)
	{
		for (uint32_t poolIter{ }; poolIter < __poolCount; ++poolIter)
		{
			const auto pPool
			{
				logicalDevice.createDescriptorPool(
					0U, maxSetCount, static_cast<uint32_t>(poolSizes.size()), poolSizes.data())
			};

			__pools.emplace_back(pPool);
		}
	}
}
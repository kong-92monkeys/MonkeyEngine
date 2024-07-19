export module ntmonkeys.com.Engine.DescriptorSetFactory;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Graphics.DescriptorSetLayout;
import ntmonkeys.com.Graphics.DescriptorSet;
import ntmonkeys.com.Engine.DescriptorSetCirculator;
import <memory>;
import <unordered_map>;

namespace Engine
{
	export class DescriptorSetFactory : public Lib::Unique
	{
	public:
		DescriptorSetFactory(Graphics::LogicalDevice &logicalDevice, const uint32_t setCount);

		[[nodiscard]]
		Graphics::DescriptorSet &getNext(const Graphics::DescriptorSetLayout &layout);

	private:
		Graphics::LogicalDevice &__logicalDevice;
		const uint32_t __setCount;

		std::unordered_map<const Graphics::DescriptorSetLayout *, std::unique_ptr<DescriptorSetCirculator>> __circulators;
	};
}

module: private;

namespace Engine
{
	DescriptorSetFactory::DescriptorSetFactory(Graphics::LogicalDevice &logicalDevice, const uint32_t setCount) :
		__logicalDevice	{ logicalDevice },
		__setCount		{ setCount }
	{}

	Graphics::DescriptorSet &DescriptorSetFactory::getNext(const Graphics::DescriptorSetLayout &layout)
	{
		auto &pCirculator{ __circulators[&layout] };
		if (!pCirculator)
			pCirculator = std::make_unique<DescriptorSetCirculator>(__logicalDevice, layout, __setCount);

		return pCirculator->getNext();
	}
}
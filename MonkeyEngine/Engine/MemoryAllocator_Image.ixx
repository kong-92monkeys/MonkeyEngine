module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Engine.MemoryAllocator:Image;

import :Memory;
import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Graphics.Memory;
import ntmonkeys.com.Graphics.Image;
import ntmonkeys.com.Graphics.LogicalDevice;
import <memory>;

namespace Engine
{
	export class ImageChunk : public Lib::Unique
	{
	public:
		ImageChunk(Graphics::LogicalDevice &device, const Graphics::LogicalDevice::ImageCreateInfo &createInfo);

		[[nodiscard]]
		constexpr Graphics::Image &getImage() noexcept;

		void bindMemory(std::unique_ptr<AbstractMemory> &&pMemory) noexcept;

	private:
		std::unique_ptr<Graphics::Image> __pImage;
		std::unique_ptr<AbstractMemory> __pBoundMemory;
	};

	constexpr Graphics::Image &ImageChunk::getImage() noexcept
	{
		return *__pImage;
	}

	ImageChunk::ImageChunk(Graphics::LogicalDevice &device, const Graphics::LogicalDevice::ImageCreateInfo &createInfo)
	{
		__pImage = std::unique_ptr<Graphics::Image>{ device.createImage(createInfo) };
	}

	void ImageChunk::bindMemory(std::unique_ptr<AbstractMemory> &&pMemory) noexcept
	{
		__pImage->bindMemory(pMemory->getMemory().getHandle(), pMemory->getOffset());
		__pBoundMemory = std::move(pMemory);
	}
}
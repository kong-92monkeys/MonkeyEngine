module;

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

export module ntmonkeys.com.Lib.Bitmap;

import ntmonkeys.com.Lib.Unique;
import <cstdint>;
import <vector>;
import <stdexcept>;

namespace Lib
{
	export class Bitmap : public Unique
	{
	public:
		Bitmap(const size_t width, const size_t height, const size_t channelCount) noexcept;
		Bitmap(const size_t width, const size_t height, const size_t channelCount, const size_t stride) noexcept;

		Bitmap(const void *const pEncodedData, const size_t size);
		Bitmap(const void *const pEncodedData, const size_t size, const size_t stride);

		[[nodiscard]]
		constexpr size_t getWidth() const noexcept;

		[[nodiscard]]
		constexpr size_t getHeight() const noexcept;

		[[nodiscard]]
		constexpr size_t getChannelCount() const noexcept;

		[[nodiscard]]
		constexpr size_t getStride() const noexcept;

		[[nodiscard]]
		constexpr size_t getDataSize() const noexcept;

		[[nodiscard]]
		constexpr std::byte *getData() noexcept;

		[[nodiscard]]
		constexpr const std::byte *getData() const noexcept;

	private:
		size_t __width{ };
		size_t __height{ };
		size_t __channelCount{ };
		size_t __stride{ };

		std::vector<std::byte> __data;
	};

	constexpr size_t Bitmap::getWidth() const noexcept
	{
		return __width;
	}

	constexpr size_t Bitmap::getHeight() const noexcept
	{
		return __height;
	}

	constexpr size_t Bitmap::getChannelCount() const noexcept
	{
		return __channelCount;
	}

	constexpr size_t Bitmap::getStride() const noexcept
	{
		return __stride;
	}

	constexpr size_t Bitmap::getDataSize() const noexcept
	{
		return __data.size();
	}

	constexpr std::byte *Bitmap::getData() noexcept
	{
		return __data.data();
	}

	constexpr const std::byte *Bitmap::getData() const noexcept
	{
		return __data.data();
	}
}

module: private;

namespace Lib
{
	Bitmap::Bitmap(const size_t width, const size_t height, const size_t channelCount) noexcept :
		Bitmap{ width, height, channelCount, channelCount }
	{}

	Bitmap::Bitmap(const size_t width, const size_t height, const size_t channelCount, const size_t stride) noexcept :
		__width			{ width },
		__height		{ height },
		__channelCount	{ channelCount },
		__stride		{ stride }
	{
		__data.resize(width * height * stride);
	}

	Bitmap::Bitmap(const void *const pEncodedData, const size_t size) :
		Bitmap{ pEncodedData, size, 0ULL }
	{}

	Bitmap::Bitmap(const void *const pEncodedData, const size_t size, const size_t stride)
	{
		int width{ };
		int height{ };
		int channelCount{ };

		const auto pData
		{
			stbi_load_from_memory(
				static_cast<const stbi_uc *>(pEncodedData), static_cast<int>(size),
				&width, &height, &channelCount, static_cast<int>(stride))
		};

		if (!pData)
			throw std::runtime_error{ "Cannot decode the given data." };

		__width				= static_cast<size_t>(width);
		__height			= static_cast<size_t>(height);
		__channelCount		= static_cast<size_t>(channelCount);
		__stride			= stride;

		const size_t memSize{ __width * __height * __stride };
		__data.resize(memSize);

		std::memcpy(__data.data(), pData, memSize);
		stbi_image_free(pData);
	}
}
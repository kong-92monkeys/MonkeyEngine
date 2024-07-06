export module ntmonkeys.com.Frameworks.HostBuffer;

import <vector>;
import <cstddef>;
import <initializer_list>;

namespace Frameworks
{
	export class HostBuffer
	{
	public:
		void add(const void *const pData, const size_t size) noexcept;

		template <typename $T>
		void typedAdd(const $T *const pData, const size_t size) noexcept;

		template <typename $T>
		void typedAdd(const $T &data) noexcept;

		template <typename $T>
		void typedAdd(const std::initializer_list<$T> &data) noexcept;

		template <typename $T>
		void typedAdd(const std::vector<$T> &data) noexcept;

		void insert(const size_t offset, const void *const pData, const size_t size) noexcept;

		template <typename $T>
		void typedInsert(const size_t index, const $T *const pData, const size_t size) noexcept;

		template <typename $T>
		void typedInsert(const size_t index, const $T &data) noexcept;

		template <typename $T>
		void typedInsert(const size_t index, const std::initializer_list<$T> &data) noexcept;

		template <typename $T>
		void typedInsert(const size_t index, const std::vector<$T> &data) noexcept;

		void set(const size_t offset, const void *const pData, const size_t size) noexcept;

		template <typename $T>
		void typedSet(const size_t index, const $T *const pData, const size_t size) noexcept;

		template <typename $T>
		void typedSet(const size_t index, const $T &data) noexcept;

		[[nodiscard]]
		constexpr std::byte *getData() noexcept;

		[[nodiscard]]
		constexpr const std::byte *getData() const noexcept;

		template <typename $T>
		[[nodiscard]]
		constexpr $T *getTypedData() noexcept;

		template <typename $T>
		[[nodiscard]]
		constexpr const $T *getTypedData() const noexcept;

		[[nodiscard]]
		constexpr size_t getSize() const noexcept;

		template <typename $T>
		[[nodiscard]]
		constexpr size_t getTypedSize() const noexcept;

		void clear() noexcept;
		void resize(const size_t size) noexcept;

		template <typename $T>
		void typedResize(const size_t size) noexcept;

	private:
		std::vector<std::byte> __buffer;
	};

	template <typename $T>
	void HostBuffer::typedAdd(const $T *const pData, const size_t size) noexcept
	{
		add(pData, size * sizeof($T));
	}

	template <typename $T>
	void HostBuffer::typedAdd(const $T &data) noexcept
	{
		typedAdd<$T>(&data, 1ULL);
	}

	template <typename $T>
	void HostBuffer::typedAdd(const std::initializer_list<$T> &data) noexcept
	{
		typedAdd<$T>(data.begin(), data.size());
	}

	template <typename $T>
	void HostBuffer::typedAdd(const std::vector<$T> &data) noexcept
	{
		typedAdd<$T>(data.data(), data.size());
	}

	template <typename $T>
	void HostBuffer::typedInsert(const size_t index, const $T *const pData, const size_t size) noexcept
	{
		insert(index * sizeof($T), pData, size * sizeof($T));
	}

	template <typename $T>
	void HostBuffer::typedInsert(const size_t index, const $T &data) noexcept
	{
		typedInsert<$T>(index, &data, 1ULL);
	}

	template <typename $T>
	void HostBuffer::typedInsert(const size_t index, const std::initializer_list<$T> &data) noexcept
	{
		typedInsert<$T>(index, data.begin(), data.size());
	}

	template <typename $T>
	void HostBuffer::typedInsert(const size_t index, const std::vector<$T> &data) noexcept
	{
		typedInsert<$T>(index, data.data(), data.size());
	}

	template <typename $T>
	void HostBuffer::typedSet(const size_t index, const $T *const pData, const size_t size) noexcept
	{
		set(index * sizeof($T), pData, size * sizeof($T));
	}

	template <typename $T>
	void HostBuffer::typedSet(const size_t index, const $T &data) noexcept
	{
		typedSet<$T>(index, &data, 1ULL);
	}

	constexpr std::byte *HostBuffer::getData() noexcept
	{
		return __buffer.data();
	}

	constexpr const std::byte *HostBuffer::getData() const noexcept
	{
		return __buffer.data();
	}

	template <typename $T>
	constexpr $T *HostBuffer::getTypedData() noexcept
	{
		return reinterpret_cast<$T *>(getData());
	}

	template <typename $T>
	constexpr const $T *HostBuffer::getTypedData() const noexcept
	{
		return reinterpret_cast<const $T *>(getData());
	}

	constexpr size_t HostBuffer::getSize() const noexcept
	{
		return __buffer.size();
	}

	template <typename $T>
	constexpr size_t HostBuffer::getTypedSize() const noexcept
	{
		return (getSize() / sizeof($T));
	}

	template <typename $T>
	void HostBuffer::typedResize(const size_t size) noexcept
	{
		resize(size * sizeof($T));
	}
}

module: private;

namespace Frameworks
{
	void HostBuffer::add(const void *const pData, const size_t size) noexcept
	{
		const size_t prevSize{ __buffer.size() };
		__buffer.resize(prevSize + size);

		std::memcpy(__buffer.data() + prevSize, pData, size);
	}

	void HostBuffer::insert(const size_t offset, const void *const pData, const size_t size) noexcept
	{
		const auto iter{ __buffer.begin() + offset };
		const auto pSrc{ static_cast<const std::byte *>(pData) };
		__buffer.insert(iter, pSrc, pSrc + size);
	}

	void HostBuffer::set(const size_t offset, const void *const pData, const size_t size) noexcept
	{
		std::memcpy(__buffer.data() + offset, pData, size);
	}

	void HostBuffer::clear() noexcept
	{
		__buffer.clear();
	}

	void HostBuffer::resize(const size_t size) noexcept
	{
		__buffer.resize(size);
	}
}
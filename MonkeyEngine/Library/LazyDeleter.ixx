export module ntmonkeys.com.Lib.LazyDeleter;

import ntmonkeys.com.Lib.Unique;
import <deque>;
import <vector>;
import <any>;

namespace Lib
{
	export class LazyDeleter : public Unique
	{
	public:
		explicit LazyDeleter(const size_t queueSize) noexcept;
		virtual ~LazyDeleter() noexcept override;

		[[nodiscard]]
		size_t getQueueSize() const noexcept;
		void setQueueSize(const size_t size);

		template <typename $T>
		void reserve($T &&garbage) noexcept;

		void advance();
		void flush();

	private:
		std::deque<std::vector<std::any>> __garbageQueue;
	};

	template <typename $T>
	void LazyDeleter::reserve($T &&garbage) noexcept
	{
		__garbageQueue.back().emplace_back(std::forward<$T>(garbage));
	}
}

module: private;

namespace Lib
{
	LazyDeleter::LazyDeleter(const size_t queueSize) noexcept
	{
		__garbageQueue.resize(queueSize);
	}

	LazyDeleter::~LazyDeleter() noexcept
	{
		flush();
	}

	size_t LazyDeleter::getQueueSize() const noexcept
	{
		return __garbageQueue.size();
	}

	void LazyDeleter::setQueueSize(const size_t size)
	{
		const size_t oldSize{ getQueueSize() };
		if (size == oldSize)
			return;

		for (size_t iter{ size }; iter < oldSize; ++iter)
			__garbageQueue.pop_front();

		__garbageQueue.resize(size);
	}

	void LazyDeleter::advance()
	{
		auto holder{ std::move(__garbageQueue.front()) };
		holder.clear();

		__garbageQueue.pop_front();
		__garbageQueue.emplace_back(std::move(holder));
	}

	void LazyDeleter::flush()
	{
		for (auto &holder : __garbageQueue)
			holder.clear();
	}
}
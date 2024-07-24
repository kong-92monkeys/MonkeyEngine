export module ntmonkeys.com.Lib.ThreadPool;

import ntmonkeys.com.Lib.Unique;
import <thread>;
import <functional>;
import <optional>;
import <future>;
import <mutex>;
import <condition_variable>;
import <semaphore>;

namespace Lib
{
	export class ThreadPool : public Unique
	{
	public:
		using Job = std::function<void()>;

		ThreadPool(const size_t poolSize = std::thread::hardware_concurrency());
		virtual ~ThreadPool() noexcept override;

		void waitIdle(const size_t threadIndex);
		void waitIdle();

		std::future<void> run(const size_t threadIndex, Job &&job);
		std::future<void> run(Job &&job);

		void silentRun(const size_t threadIndex, Job &&job);
		void silentRun(Job &&job);

		[[nodiscard]]
		constexpr size_t getPoolSize() const noexcept;

	private:
		struct __JobInfo
		{
		public:
			Job job;
			std::optional<std::promise<void>> promise;

			void signal() noexcept;
		};

		struct __SlotInfo
		{
		public:
			std::thread thread;
			std::mutex loopMutex;
			std::vector<__JobInfo> jobInfos;
			std::condition_variable loopCV;
		};

		std::vector<std::unique_ptr<__SlotInfo>> __slotInfos;
		std::vector<std::thread::id> __threadIds;

		bool __running{ true };

		size_t __randomSlotIndex{ };

		void __loop(const size_t threadIndex);
	};

	constexpr size_t ThreadPool::getPoolSize() const noexcept
	{
		return __slotInfos.size();
	}
}

module: private;

namespace Lib
{
	ThreadPool::ThreadPool(const size_t poolSize)
	{
		__slotInfos.resize(poolSize);
		__threadIds.resize(poolSize);

		for (size_t slotIt{ }; slotIt < poolSize; ++slotIt)
		{
			auto &pSlotInfo{ __slotInfos[slotIt] };
			pSlotInfo = std::make_unique<__SlotInfo>();

			auto &thread{ pSlotInfo->thread };
			thread = std::thread{ &ThreadPool::__loop, this, slotIt };

			__threadIds[slotIt] = thread.get_id();
		}
	}

	ThreadPool::~ThreadPool() noexcept
	{
		__running = false;

		for (const auto &pSlotInfo : __slotInfos)
		{
			std::lock_guard loopLock	{ pSlotInfo->loopMutex };
			auto &loopCV				{ pSlotInfo->loopCV };
			loopCV.notify_all();
		}

		for (const auto &pSlotInfo : __slotInfos)
			pSlotInfo->thread.join();
	}

	void ThreadPool::waitIdle(const size_t threadIndex)
	{
		run(threadIndex, [] { }).wait();
	}

	void ThreadPool::waitIdle()
	{
		const size_t poolSize{ getPoolSize() };
		for (size_t slotIter{ }; slotIter < poolSize; ++slotIter)
			run(slotIter, [] { }).wait();
	}

	std::future<void> ThreadPool::run(const size_t threadIndex, Job &&job)
	{
		const auto &pSlotInfo	{ __slotInfos[threadIndex] };
		auto &loopCV			{ pSlotInfo->loopCV };
		auto &jobInfos			{ pSlotInfo->jobInfos };

		std::promise<void> promise;
		std::future<void> retVal{ promise.get_future() };

		__JobInfo jobInfo
		{
			.job		{ std::move(job) },
			.promise	{ std::move(promise) }
		};

		{
			std::lock_guard loopLock{ pSlotInfo->loopMutex };
			jobInfos.emplace_back(std::move(jobInfo));
			loopCV.notify_all();
		}

		return retVal;
	}

	std::future<void> ThreadPool::run(Job &&job)
	{
		__randomSlotIndex = ((__randomSlotIndex + 1ULL) % getPoolSize());
		return run(__randomSlotIndex, std::move(job));
	}

	void ThreadPool::silentRun(const size_t threadIndex, Job &&job)
	{
		const auto &pSlotInfo	{ __slotInfos[threadIndex] };
		auto &loopCV			{ pSlotInfo->loopCV };
		auto &jobInfos			{ pSlotInfo->jobInfos };

		__JobInfo jobInfo
		{
			.job{ std::move(job) }
		};

		{
			std::lock_guard loopLock{ pSlotInfo->loopMutex };
			jobInfos.emplace_back(std::move(jobInfo));
			loopCV.notify_all();
		}
	}

	void ThreadPool::silentRun(Job &&job)
	{
		__randomSlotIndex = ((__randomSlotIndex + 1ULL) % getPoolSize());
		silentRun(__randomSlotIndex, std::move(job));
	}

	void ThreadPool::__loop(const size_t threadIndex)
	{
		const auto &pSlotInfo		{ __slotInfos[threadIndex] };

		std::unique_lock loopLock	{ pSlotInfo->loopMutex, std::defer_lock };
		auto &loopCV				{ pSlotInfo->loopCV };
		auto &jobInfos				{ pSlotInfo->jobInfos };

		std::vector<__JobInfo> inFlightJobInfos;

		while (true)
		{
			loopLock.lock();

			loopCV.wait(loopLock, [this, &jobInfos]
			{
				return (!__running || jobInfos.size());
			});

			if (!__running)
				break;

			inFlightJobInfos.swap(jobInfos);
			loopLock.unlock();

			for (auto &jobInfo : inFlightJobInfos)
			{
				jobInfo.job();
				jobInfo.signal();
			}

			inFlightJobInfos.clear();
		}
	}

	void ThreadPool::__JobInfo::signal() noexcept
	{
		if (promise.has_value())
			promise.value().set_value();
	}
}
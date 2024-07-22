export module ntmonkeys.com.Lib.ThreadPool;

import ntmonkeys.com.Lib.Unique;
import <thread>;
import <queue>;
import <functional>;
import <future>;
import <mutex>;
import <condition_variable>;
import <random>;

namespace Lib
{
	export class ThreadPool : public Unique
	{
	public:
		using Job = std::function<void()>;

		ThreadPool(const size_t poolSize = std::thread::hardware_concurrency());
		virtual ~ThreadPool() noexcept override;

		std::future<void> run(const size_t slotIndex, Job &&job);
		std::future<void> run(Job &&job);

		[[nodiscard]]
		constexpr size_t getPoolSize() const noexcept;

	private:
		struct __JobInfo
		{
		public:
			std::promise<void> promise;
			Job job;
		};

		struct __SlotInfo
		{
		public:
			std::thread thread;
			std::mutex loopMutex;
			std::queue<__JobInfo> jobInfos;
			std::condition_variable loopCV;
		};

		std::vector<std::unique_ptr<__SlotInfo>> __slotInfos;
		bool __running{ true };

		std::default_random_engine __randEngine{ std::random_device{ }() };
		std::uniform_int_distribution<size_t> __slotIndexDist;

		void __loop(const size_t slotIndex);
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

		for (uint32_t threadIt{ }; threadIt < poolSize; ++threadIt)
		{
			auto &pSlotInfo{ __slotInfos[threadIt] };
			pSlotInfo = std::make_unique<__SlotInfo>();
			pSlotInfo->thread = std::thread{ std::bind(&ThreadPool::__loop, this, threadIt) };
		}

		__slotIndexDist = std::uniform_int_distribution<size_t>{ 0ULL, poolSize - 1ULL };
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

	std::future<void> ThreadPool::run(const size_t slotIndex, Job &&job)
	{
		const auto &pSlotInfo	{ __slotInfos[slotIndex] };
		auto &loopCV			{ pSlotInfo->loopCV };
		auto &jobInfos			{ pSlotInfo->jobInfos };

		std::promise<void> promise;
		std::future<void> retVal{ promise.get_future() };

		__JobInfo jobInfo
		{
			.promise	{ std::move(promise) },
			.job		{ std::move(job) }
		};

		{
			std::lock_guard loopLock{ pSlotInfo->loopMutex };
			jobInfos.emplace(std::move(jobInfo));
			loopCV.notify_all();
		}

		return retVal;
	}

	std::future<void> ThreadPool::run(Job &&job)
	{
		return run(__slotIndexDist(__randEngine), std::move(job));
	}

	void ThreadPool::__loop(const size_t slotIndex)
	{
		const auto &pSlotInfo		{ __slotInfos[slotIndex] };

		std::unique_lock loopLock	{ pSlotInfo->loopMutex, std::defer_lock };
		auto &loopCV				{ pSlotInfo->loopCV };
		auto &jobInfos				{ pSlotInfo->jobInfos };

		while (true)
		{
			loopLock.lock();

			loopCV.wait(loopLock, [this, &jobInfos]
			{
				return (!__running || jobInfos.size());
			});

			if (!__running)
				break;

			__JobInfo jobInfo{ std::move(jobInfos.front()) };
			jobInfos.pop();

			loopLock.unlock();

			jobInfo.job();
			jobInfo.promise.set_value();
		}
	}
}
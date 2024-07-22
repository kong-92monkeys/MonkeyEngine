export module ntmonkeys.com.Lib.ThreadPool;

import ntmonkeys.com.Lib.Unique;
import <thread>;
import <queue>;
import <functional>;
import <future>;
import <mutex>;
import <condition_variable>;

namespace Lib
{
	export class ThreadPool : public Unique
	{
	public:
		using Job = std::function<void(uint32_t)>;

		ThreadPool(const uint32_t threadCount = std::thread::hardware_concurrency());
		virtual ~ThreadPool() noexcept override;

		std::future<void> run(Job &&job);

		[[nodiscard]]
		constexpr uint32_t getThreadCount() const noexcept;

	private:
		struct __JobInfo
		{
		public:
			std::promise<void> promise;
			Job job;
		};

		std::vector<std::thread> __threads;
		
		std::mutex __loopMutex;

		std::queue<__JobInfo> __jobInfos;
		bool __running{ true };

		std::condition_variable __loopCV;

		void __loop(const uint32_t threadIndex);
	};

	constexpr uint32_t ThreadPool::getThreadCount() const noexcept
	{
		return static_cast<uint32_t>(__threads.size());
	}
}

module: private;

namespace Lib
{
	ThreadPool::ThreadPool(const uint32_t threadCount)
	{
		for (uint32_t threadIt{ }; threadIt < threadCount; ++threadIt)
			__threads.emplace_back(std::bind(&ThreadPool::__loop, this, threadIt));
	}

	ThreadPool::~ThreadPool() noexcept
	{
		std::unique_lock loopLock{ __loopMutex };

		__running = false;
		__loopCV.notify_all();

		loopLock.unlock();

		for (auto &thread : __threads)
			thread.join();
	}

	std::future<void> ThreadPool::run(Job &&job)
	{
		std::promise<void> promise;
		std::future<void> retVal{ promise.get_future() };

		__JobInfo jobInfo
		{
			.promise	{ std::move(promise) },
			.job		{ std::move(job) }
		};

		{
			std::lock_guard loopLock{ __loopMutex };
			__jobInfos.emplace(std::move(jobInfo));
			__loopCV.notify_one();
		}

		return retVal;
	}

	void ThreadPool::__loop(const uint32_t threadIndex)
	{
		std::unique_lock loopLock{ __loopMutex, std::defer_lock };

		while (true)
		{
			loopLock.lock();

			__loopCV.wait(loopLock, [this]
			{
				return (!__running || __jobInfos.size());
			});

			if (!__running)
				break;

			__JobInfo jobInfo{ std::move(__jobInfos.front()) };
			__jobInfos.pop();

			loopLock.unlock();

			jobInfo.job(threadIndex);
			jobInfo.promise.set_value();
		}
	}
}
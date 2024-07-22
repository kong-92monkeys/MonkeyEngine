export module ntmonkeys.com.Lib.ThreadSlot;

import ntmonkeys.com.Lib.Unique;
import <thread>;
import <queue>;
import <functional>;
import <future>;
import <mutex>;
import <condition_variable>;

namespace Lib
{
	export class ThreadSlot : public Unique
	{
	public:
		using Job = std::function<void()>;

		ThreadSlot(const size_t slotCount = std::thread::hardware_concurrency());
		virtual ~ThreadSlot() noexcept override;

		std::future<void> run(const size_t slotIndex, Job &&job);

		[[nodiscard]]
		constexpr size_t getSlotCount() const noexcept;

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

		void __loop(const size_t slotIndex);
	};

	constexpr size_t ThreadSlot::getSlotCount() const noexcept
	{
		return __slotInfos.size();
	}
}

module: private;

namespace Lib
{
	ThreadSlot::ThreadSlot(const size_t slotCount)
	{
		__slotInfos.resize(slotCount);

		for (uint32_t threadIt{ }; threadIt < slotCount; ++threadIt)
		{
			auto &pSlotInfo{ __slotInfos[threadIt] };
			pSlotInfo = std::make_unique<__SlotInfo>();
			pSlotInfo->thread = std::thread{ std::bind(&ThreadSlot::__loop, this, threadIt) };
		}
	}

	ThreadSlot::~ThreadSlot() noexcept
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

	std::future<void> ThreadSlot::run(const size_t slotIndex, Job &&job)
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

	void ThreadSlot::__loop(const size_t slotIndex)
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
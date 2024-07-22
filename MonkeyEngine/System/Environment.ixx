export module ntmonkeys.com.Sys.Environment;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Lib.ThreadPool;
import ntmonkeys.com.Lib.ThreadSlot;
import ntmonkeys.com.Lib.AssetManager;

namespace Sys
{
	export class Environment : public Lib::Unique
	{
	public:
		[[nodiscard]]
		constexpr Lib::AssetManager &getAssetManager() noexcept;

		[[nodiscard]]
		constexpr Lib::ThreadPool &getThreadPool() noexcept;

		[[nodiscard]]
		constexpr Lib::ThreadSlot &getThreadSlot() noexcept;

		[[nodiscard]]
		static Environment &getInstance();

	private:
		Environment() = default;

		Lib::AssetManager __assetManager;
		Lib::ThreadPool __threadPool;
		Lib::ThreadSlot __threadSlot;
	};

	constexpr Lib::AssetManager &Environment::getAssetManager() noexcept
	{
		return __assetManager;
	}

	constexpr Lib::ThreadPool &Environment::getThreadPool() noexcept
	{
		return __threadPool;
	}

	constexpr Lib::ThreadSlot &Environment::getThreadSlot() noexcept
	{
		return __threadSlot;
	}
}

module: private;

namespace Sys
{
	Environment &Environment::getInstance()
	{
		static Environment instance;
		return instance;
	}
}
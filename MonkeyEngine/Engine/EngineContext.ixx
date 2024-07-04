export module ntmonkeys.com.Engine.EngineContext;

import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Engine.CommandExecutor;
import ntmonkeys.com.Engine.MemoryAllocator;

namespace Engine
{
	export struct EngineContext
	{
	public:
		Lib::LazyDeleter *pLazyDeleter{ };
		CommandExecutor *pCommandExecutor{ };
		MemoryAllocator *pMemoryAllocator{ };
	};
}
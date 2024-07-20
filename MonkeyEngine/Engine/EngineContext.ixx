export module ntmonkeys.com.Engine.EngineContext;

import ntmonkeys.com.Lib.LazyDeleter;
import ntmonkeys.com.Graphics.LogicalDevice;
import ntmonkeys.com.Engine.CommandExecutor;
import ntmonkeys.com.Engine.MemoryAllocator;
import ntmonkeys.com.Engine.LayerResourcePool;
import ntmonkeys.com.Engine.RenderPassFactory;

namespace Engine
{
	export struct EngineContext
	{
	public:
		Lib::LazyDeleter *pLazyDeleter{ };
		Graphics::LogicalDevice *pLogicalDevice{ };
		CommandExecutor *pCommandExecutor{ };
		MemoryAllocator *pMemoryAllocator{ };
		LayerResourcePool *pLayerResourcePool{ };
		RenderPassFactory *pRenderPassFactory{ };
	};
}
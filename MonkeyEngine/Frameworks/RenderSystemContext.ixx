export module ntmonkeys.com.Frameworks.RenderSystemContext;

import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Frameworks.RendererFactory;

namespace Frameworks
{
	export struct RenderSystemContext
	{
	public:
		Engine::RenderingEngine *pEngine{ };
		RendererFactory *pRendererFactory{ };
	};
}
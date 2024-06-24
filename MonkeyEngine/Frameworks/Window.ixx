module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.Window;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Engine.RenderTarget;
import <memory>;

namespace Frameworks
{
	export class Window : public Lib::Unique
	{
	public:
		Window(Engine::RenderingEngine &engine, const HINSTANCE hinstance, const HWND hwnd);
		virtual ~Window() noexcept override = default;

		void sync();

	private:
		std::unique_ptr<Engine::RenderTarget> __pRenderTarget;
	};
}

module: private;

namespace Frameworks
{
	Window::Window(Engine::RenderingEngine &engine, const HINSTANCE hinstance, const HWND hwnd)
	{
		__pRenderTarget = std::unique_ptr<Engine::RenderTarget>{ engine.createRenderTarget(hinstance, hwnd) };
	}

	void Window::sync()
	{
		__pRenderTarget->sync();
	}
}
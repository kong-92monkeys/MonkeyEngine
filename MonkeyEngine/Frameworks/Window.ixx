module;

#include "../Vulkan/Vulkan.h"

export module ntmonkeys.com.Frameworks.Window;

import ntmonkeys.com.Lib.Unique;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Engine.RenderTarget;
import ntmonkeys.com.Engine.CommandExecutor;
import ntmonkeys.com.Frameworks.RenderPassFactory;
import ntmonkeys.com.Frameworks.FramebufferFactory;
import ntmonkeys.com.Frameworks.Layer;
import <memory>;
import <unordered_set>;

namespace Frameworks
{
	export class Window : public Lib::Unique
	{
	public:
		Window(
			Engine::RenderingEngine &engine,
			const RenderPassFactory &renderPassFactory,
			const HINSTANCE hinstance, const HWND hwnd);

		virtual ~Window() noexcept override;

		void sync();

		void addLayer(Layer &layer) noexcept;
		void removeLayer(Layer &layer) noexcept;

		void draw(Engine::CommandExecutor::Execution &execution);

	private:
		std::unique_ptr<FramebufferFactory> __pFramebufferFactory;
		std::unique_ptr<Engine::RenderTarget> __pRenderTarget;

		std::unordered_set<Layer *> __layers;
	};
}

module: private;

namespace Frameworks
{
	Window::Window(
		Engine::RenderingEngine &engine,
		const RenderPassFactory &renderPassFactory,
		const HINSTANCE hinstance, const HWND hwnd)
	{
		__pFramebufferFactory = std::make_unique<FramebufferFactory>(engine, renderPassFactory);
		__pRenderTarget = std::unique_ptr<Engine::RenderTarget>{ engine.createRenderTarget(hinstance, hwnd) };
		__pFramebufferFactory->invalidate(__pRenderTarget->getWidth(), __pRenderTarget->getHeight());
	}

	Window::~Window() noexcept
	{
		__pRenderTarget = nullptr;
		__pFramebufferFactory = nullptr;
	}

	void Window::sync()
	{
		__pRenderTarget->sync();
		__pFramebufferFactory->invalidate(__pRenderTarget->getWidth(), __pRenderTarget->getHeight());
	}

	void Window::addLayer(Layer &layer) noexcept
	{
		__layers.emplace(&layer);
	}

	void Window::removeLayer(Layer &layer) noexcept
	{
		__layers.erase(&layer);
	}

	void Window::draw(Engine::CommandExecutor::Execution &execution)
	{
		if (!(__pRenderTarget->isPresentable()))
			return;

		const auto pLayer{ *__layers.begin() };

		const VkRect2D renderArea
		{
			.offset	{ 0, 0 },
			.extent	{ __pRenderTarget->getWidth(), __pRenderTarget->getHeight() }
		};

		pLayer->draw(renderArea, *__pFramebufferFactory, execution);
	}
}
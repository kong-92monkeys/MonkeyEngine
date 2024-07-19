
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Library/GLM.h"
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Lib.AssetManager;
import ntmonkeys.com.Engine.Core;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Engine.RenderTarget;
import ntmonkeys.com.Engine.Layer;
import ntmonkeys.com.Engine.RenderObject;
import <memory>;

// CApp:
// See App.cpp for the implementation of this class
//

class CApp : public CWinApp
{
public:
	CApp() noexcept;


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

public:
	[[nodiscard]]
	constexpr Engine::RenderingEngine &getRenderingEngine() noexcept;

	void addRenderTarget(Engine::RenderTarget &renderTarget) noexcept;
	void removeRenderTarget(Engine::RenderTarget &renderTarget) noexcept;

	[[nodiscard]]
	constexpr Lib::EventView<> &getIdleEvent() const noexcept;

private:
	std::unique_ptr<Lib::AssetManager> __pAssetManager;

	std::unique_ptr<Engine::Core> __pCore;
	std::unique_ptr<Engine::RenderingEngine> __pRenderingEngine;

	std::shared_ptr<Engine::Layer> __pLayer;
	std::shared_ptr<Engine::RenderObject> __pRenderObject;

	mutable Lib::Event<> __idleEvent;

	void __onInitBeforeMainFrame();
	void __createGraphicsCore();
	void __createRenderingEngine();

public:
	virtual BOOL OnIdle(LONG lCount);
};

extern CApp theApp;

constexpr Engine::RenderingEngine &CApp::getRenderingEngine() noexcept
{
	return *__pRenderingEngine;
}

constexpr Lib::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}
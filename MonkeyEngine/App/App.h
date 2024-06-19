
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
import <memory>;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Engine.Core;
import ntmonkeys.com.Engine.RenderingEngine;
import ntmonkeys.com.Engine.Renderer;

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
	constexpr Engine::RenderingEngine &getEngine() noexcept;

	[[nodiscard]]
	constexpr Lib::EventView<> &getIdleEvent() const noexcept;

private:
	std::unique_ptr<Engine::Core> __pCore;
	std::unique_ptr<Engine::RenderingEngine> __pRenderingEngine;
	std::unique_ptr<Engine::Renderer> __pRenderer;

	mutable Lib::Event<> __idleEvent;

	void __onInitBeforeMainFrame();
	void __createGraphicsCore();
	void __createRenderingEngine();

public:
	virtual BOOL OnIdle(LONG lCount);
};

extern CApp theApp;

constexpr Engine::RenderingEngine &CApp::getEngine() noexcept
{
	return *__pRenderingEngine;
}

constexpr Lib::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}
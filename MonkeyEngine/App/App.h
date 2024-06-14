
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
import <memory>;
import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Graphics.Core;
import ntmonkeys.com.Graphics.Engine;

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
	constexpr Graphics::Core &getCore() noexcept;

	[[nodiscard]]
	constexpr Graphics::Engine &getEngine() noexcept;

	[[nodiscard]]
	constexpr Lib::EventView<> &getIdleEvent() const noexcept;

private:
	std::unique_ptr<Graphics::Core> __pGraphicsCore;
	std::unique_ptr<Graphics::Engine> __pGraphicsEngine;

	mutable Lib::Event<> __idleEvent;

	void __onInitBeforeMainFrame();
	void __createGraphicsCore();
	void __createGraphicsEngine();

public:
	virtual BOOL OnIdle(LONG lCount);
};

extern CApp theApp;

constexpr Graphics::Core &CApp::getCore() noexcept
{
	return *__pGraphicsCore;
}

constexpr Graphics::Engine &CApp::getEngine() noexcept
{
	return *__pGraphicsEngine;
}

constexpr Lib::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}
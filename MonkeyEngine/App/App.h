
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
	constexpr Lib::EventView<> &getIdleEvent() const noexcept;

private:
	std::unique_ptr<Graphics::Core> __pGraphicsCore;

	mutable Lib::Event<> __idleEvent;

	void __createVulkanCore() noexcept;

public:
	virtual BOOL OnIdle(LONG lCount);
};

extern CApp theApp;

constexpr Lib::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}
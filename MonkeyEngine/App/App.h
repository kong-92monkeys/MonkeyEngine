
// App.h : main header file for the App application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "../Library/Event.h"

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
	mutable Lib::Event<> __idleEvent;
public:
	virtual BOOL OnIdle(LONG lCount);
};

extern CApp theApp;

constexpr Lib::EventView<> &CApp::getIdleEvent() const noexcept
{
	return __idleEvent;
}
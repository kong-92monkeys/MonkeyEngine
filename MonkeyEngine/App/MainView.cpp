
// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "App.h"
#include "MainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

import ntmonkeys.com.Lib.Logger;

IMPLEMENT_DYNCREATE(CMainView, CWnd)

CMainView::CMainView()
{
}

CMainView::~CMainView()
{
}


BEGIN_MESSAGE_MAP(CMainView, CWnd)
	ON_WM_PAINT()
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_SIZE()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CChildView message handlers

BOOL CMainView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CMainView::OnPaint() 
{
	ValidateRect(nullptr);
}

int CMainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	if (__createWindow(lpCreateStruct->hInstance) == -1)
		return -1;

	Lib::Logger::log(Lib::Logger::Severity::INFO, "Main surface created.");

	return 0;
}

int CMainView::__createWindow(const HINSTANCE hInstance)
{
	auto &renderSystem{ theApp.getRenderSystem() };

	try
	{
		__pWindow = std::unique_ptr<Frameworks::Window>{ renderSystem.createWindow(hInstance, GetSafeHwnd()) };
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

void CMainView::OnDestroy()
{
	__pWindow = nullptr;
	CWnd::OnDestroy();
}


void CMainView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	__pWindow->sync();
}

BOOL CMainView::OnEraseBkgnd(CDC *pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}

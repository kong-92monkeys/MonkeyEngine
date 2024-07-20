
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

	auto &renderingEngine{ theApp.getRenderingEngine() };
	renderingEngine.render(*__pRenderTarget);
}

int CMainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	__pRenderTargetNeedRedrawListener =
		Lib::EventListener<const Engine::RenderTarget *>::bind(&CMainView::__onRenderTargetRedrawNeeded, this);

	if (__createRenderTarget(lpCreateStruct->hInstance) == -1)
		return -1;

	Lib::Logger::log(Lib::Logger::Severity::INFO, "Main view created.");

	return 0;
}

int CMainView::__createRenderTarget(const HINSTANCE hInstance)
{
	auto &renderingEngine{ theApp.getRenderingEngine() };

	try
	{
		__pRenderTarget = std::unique_ptr<Engine::RenderTarget>{ renderingEngine.createRenderTarget(hInstance, GetSafeHwnd()) };
		__pRenderTarget->getNeedRedrawEvent() += __pRenderTargetNeedRedrawListener;

		theApp.addRenderTarget(*__pRenderTarget);
		return 0;
	}
	catch (...)
	{
		return -1;
	}
}

void CMainView::__onRenderTargetRedrawNeeded() noexcept
{
	Invalidate(FALSE);
}

void CMainView::OnDestroy()
{
	theApp.removeRenderTarget(*__pRenderTarget);
	__pRenderTarget = nullptr;
	CWnd::OnDestroy();
}


void CMainView::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO: Add your message handler code here
	__pRenderTarget->sync();
}

BOOL CMainView::OnEraseBkgnd(CDC *pDC)
{
	// TODO: Add your message handler code here and/or call default
	//return CWnd::OnEraseBkgnd(pDC);
	return TRUE;
}


// ChildView.cpp : implementation of the CChildView class
//

#include "pch.h"
#include "framework.h"
#include "App.h"
#include "MainView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

import ntmonkeys.com.Graphics.Core;

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
	CPaintDC dc(this); // device context for painting

	// TODO: Add your message handler code here
	
	// Do not call CWnd::OnPaint() for painting messages
}



int CMainView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	__createSurface(lpCreateStruct->hInstance);

	return 0;
}

void CMainView::__createSurface(const HINSTANCE hInstance)
{
	auto &core{ theApp.getCore() };

	const VkWin32SurfaceCreateInfoKHR createInfo
	{
		.sType			{ VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR },
		.hinstance		{ hInstance },
		.hwnd			{ GetSafeHwnd() }
	};

	__pSurface = core.createSurface(createInfo);
}

void CMainView::OnDestroy()
{
	__pSurface = nullptr;
	CWnd::OnDestroy();
}

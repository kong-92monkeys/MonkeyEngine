
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "pch.h"
#include "framework.h"
#include "App.h"

#include "MainFrm.h"
#include "MainView.h"
#include "LogView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
END_MESSAGE_MAP()

// CMainFrame construction/destruction

CMainFrame::CMainFrame() noexcept
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	cs.cx = 1920;
	cs.cy = 1080;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext *pContext)
{
	// TODO: Add your specialized code here and/or call the base class
	if (!(windowSplitter.CreateStatic(this, 1, 2)))
	{
		TRACE("Cannot split the window.");
		return FALSE;
	}

	if (!(windowSplitter.CreateView(0, 0, RUNTIME_CLASS(CLogView), CSize{ 600, 0 }, pContext)))
	{
		TRACE("Cannot create the log view.");
		return FALSE;
	}

	if (!(windowSplitter.CreateView(0, 1, RUNTIME_CLASS(CMainView), CSize{ 0, 0 }, pContext)))
	{
		TRACE("Cannot create the main view.");
		return FALSE;
	}

	const auto pLogView{ STATIC_DOWNCAST(CLogView, windowSplitter.GetPane(0, 0)) };
	pLogView->emplaceLoggerImpl();

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}
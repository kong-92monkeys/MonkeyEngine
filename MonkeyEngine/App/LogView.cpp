// CLogView.cpp : implementation file
//

#include "pch.h"
#include "App.h"
#include "LogView.h"


// CLogView

IMPLEMENT_DYNCREATE(CLogView, CFormView)

CLogView::CLogView()
	: CFormView(IDD_LOGDIALOG)
{
	__pLoggerEngine = std::make_shared<ListBoxLoggerEngine>(__logListBox);
	__pAppIdleListener = Lib::EventListener<>::bind(&CLogView::__onIdle, this);
}

CLogView::~CLogView()
{
}

void CLogView::emplaceLoggerEngine() noexcept
{
	auto &logger{ Lib::Logger::getInstance() };

	logger.emplaceEngine(__pLoggerEngine);
	logger.log(Lib::Logger::Severity::INFO, "test");
}

void CLogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGLIST, __logListBox);
}

BEGIN_MESSAGE_MAP(CLogView, CFormView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CLogView diagnostics

#ifdef _DEBUG
void CLogView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CLogView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void CLogView::__onIdle()
{
	__pLoggerEngine->flush();
}

// CLogView message handlers


int CLogView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  Add your specialized creation code here
	theApp.getIdleEvent() += __pAppIdleListener;

	return 0;
}


void CLogView::OnDestroy()
{
	CFormView::OnDestroy();

	// TODO: Add your message handler code here
	theApp.getIdleEvent() -= __pAppIdleListener;
}

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
	__pLoggerImpl = std::make_shared<ListBoxLoggerImpl>(__logListBox);
	__pAppIdleListener = Lib::EventListener<>::bind(&CLogView::__onIdle, this);
}

CLogView::~CLogView()
{
}

void CLogView::emplaceLoggerImpl() noexcept
{
	Lib::Logger::emplaceImpl(__pLoggerImpl);
}

void CLogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGLIST, __logListBox);
	DDX_Control(pDX, IDC_LOG_MSG_EDIT, __logMsgEdit);
}

BEGIN_MESSAGE_MAP(CLogView, CFormView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_CLEAR_LOG_BUTTON, &CLogView::OnBnClickedClearLogButton)
	ON_LBN_SELCHANGE(IDC_LOGLIST, &CLogView::OnLbnSelchangeLoglist)
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
	__pLoggerImpl->flush();
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


void CLogView::OnBnClickedClearLogButton()
{
	__logListBox.ResetContent();
	__logMsgEdit.SetWindowText(TEXT(""));
}

void CLogView::OnLbnSelchangeLoglist()
{
	CString text;
	__logListBox.GetText(__logListBox.GetCurSel(), text);
	__logMsgEdit.SetWindowText(text);
}

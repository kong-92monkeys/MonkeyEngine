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

}

CLogView::~CLogView()
{
}

void CLogView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CLogView, CFormView)
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


// CLogView message handlers

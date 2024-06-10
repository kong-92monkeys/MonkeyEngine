#pragma once

#include "ListBoxLoggerEngine.h"

// CLogView form view

class CLogView : public CFormView
{
	DECLARE_DYNCREATE(CLogView)

protected:
	CLogView();           // protected constructor used by dynamic creation
	virtual ~CLogView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGDIALOG };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	void emplaceLoggerEngine() noexcept;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CListBox __logListBox;
	std::shared_ptr<ListBoxLoggerEngine> __pLoggerEngine;
};



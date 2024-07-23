#pragma once

import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.App.ListBoxLoggerImpl;

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

	void emplaceLoggerImpl() noexcept;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CListBox __logListBox;
	CEdit __logMsgEdit;

	std::shared_ptr<ListBoxLoggerImpl> __pLoggerImpl;
	Lib::EventListenerPtr<> __pAppIdleListener;

	void __onIdle();

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedClearLogButton();
	afx_msg void OnLbnSelchangeLoglist();
};



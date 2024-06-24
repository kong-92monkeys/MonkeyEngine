
// ChildView.h : interface of the CChildView class
//


#pragma once

import ntmonkeys.com.Frameworks.Window;

// CChildView window

class CMainView : public CWnd
{
	DECLARE_DYNCREATE(CMainView)

// Construction
public:
	CMainView();

// Attributes
public:

// Operations
public:

// Overrides
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~CMainView();

	// Generated message map functions
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);

private:
	std::unique_ptr<Frameworks::Window> __pWindow;

	int __createWindow(const HINSTANCE hInstance);
public:
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
};


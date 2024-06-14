
// ChildView.h : interface of the CChildView class
//


#pragma once

import ntmonkeys.com.Graphics.Surface;

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

private:
	std::unique_ptr<Graphics::Surface> __pSurface;

	int __createSurface(const HINSTANCE hInstance);
};


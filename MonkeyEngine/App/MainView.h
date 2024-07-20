
// ChildView.h : interface of the CChildView class
//

#pragma once

import ntmonkeys.com.Lib.Event;
import ntmonkeys.com.Engine.RenderTarget;

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
	std::unique_ptr<Engine::RenderTarget> __pRenderTarget;
	Lib::EventListenerPtr<const Engine::RenderTarget *> __pRenderTargetNeedRedrawListener;

	int __createRenderTarget(const HINSTANCE hInstance);
	void __onRenderTargetRedrawNeeded() noexcept;

public:
	afx_msg BOOL OnEraseBkgnd(CDC *pDC);
};


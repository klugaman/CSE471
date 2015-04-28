
// AudioGenerateView.h : interface of the CAudioGenerateView class
//

#pragma once

#include "audio/WaveformWnd.h"

class CAudioGenerateView : public CView
{
protected: // create from serialization only
	CAudioGenerateView();
	DECLARE_DYNCREATE(CAudioGenerateView)

// Attributes
public:
	CAudioGenerateDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

// Implementation
public:
	virtual ~CAudioGenerateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()

private:
    CWaveformWnd    m_waveformWnd;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

#ifndef _DEBUG  // debug version in AudioGenerateView.cpp
inline CAudioGenerateDoc* CAudioGenerateView::GetDocument() const
   { return reinterpret_cast<CAudioGenerateDoc*>(m_pDocument); }
#endif


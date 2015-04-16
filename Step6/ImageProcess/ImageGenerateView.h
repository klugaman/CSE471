// ImageGenerateView.h : interface of the CImageGenerateView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_ImageGenerateView_H__9DC9E63F_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)
#define AFX_ImageGenerateView_H__9DC9E63F_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CImageGenerateView : public CScrollView
{
protected: // create from serialization only
	CImageGenerateView();
	DECLARE_DYNCREATE(CImageGenerateView)

// Attributes
public:
	CImageGenerateDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageGenerateView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();

protected:
	virtual void CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType = adjustBorder);
	virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CImageGenerateView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CImageGenerateView)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnCancelMode();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:
	void Mouse(CPoint point, int p_button);
	void DrawMe(CDC *pdc);
	void DrawMeOffScreen(CDC *pdc);
	BOOL m_mouseDown;
};

#ifndef _DEBUG  // debug version in ImageGenerateView.cpp
inline CImageGenerateDoc* CImageGenerateView::GetDocument()
   { return (CImageGenerateDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ImageGenerateView_H__9DC9E63F_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)

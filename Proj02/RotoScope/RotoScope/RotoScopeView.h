/*! \file RotoScopeView.h
 *
 * Interface of the CRotoScopeView class
 * \author Charles B. Owen
 */

#pragma once

//! Rotoscope view class
/*! The view class for the rotoscope document */
class CRotoScopeView : public CScrollView
{
protected: // create from serialization only
	CRotoScopeView();
	DECLARE_DYNCREATE(CRotoScopeView)

// Attributes
public:
    //! Get a pointer to the document object
    /*! \returns Pointer to CRotoScopeDoc document object */
	CRotoScopeDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
	virtual ~CRotoScopeView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	DECLARE_MESSAGE_MAP()

private:
    afx_msg BOOL OnEraseBkgnd(CDC* pDC);
    afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // debug version in RotoScopeView.cpp
inline CRotoScopeDoc* CRotoScopeView::GetDocument() const
   { return reinterpret_cast<CRotoScopeDoc*>(m_pDocument); }
#endif


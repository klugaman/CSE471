
// Step2View.h : interface of the CStep2View class
//

#pragma once


class CStep2View : public CView
{
protected: // create from serialization only
	CStep2View();
	DECLARE_DYNCREATE(CStep2View)

// Attributes
public:
	CStep2Doc* GetDocument() const;
	enum States { Start, Sound_1, Name, Display, Caption, Sound_2, Done, Blank, Music, Image };

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
	virtual ~CStep2View();
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
	CBitmap m_splash;
	int m_splashwid;
	int m_splashhit;
	UINT_PTR m_timer;
	bool m_firstdraw;
	void CStep2View::OnFirstDraw();

	CBitmap m_smith;
	CBitmap m_chill;
	CBitmap m_matrix;
	States m_state;
	DWORD m_starttime;
	bool m_reset;

public:

	afx_msg void OnPlayPlaysmith32772();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnPlayReset();
};

#ifndef _DEBUG  // debug version in Step2View.cpp
inline CStep2Doc* CStep2View::GetDocument() const
   { return reinterpret_cast<CStep2Doc*>(m_pDocument); }
#endif


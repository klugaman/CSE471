// RotoScopeView.cpp : implementation of the CRotoScopeView class
//

#include "stdafx.h"
#include "RotoScope.h"

#include "RotoScopeDoc.h"
#include "RotoScopeView.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace std;

// CRotoScopeView

IMPLEMENT_DYNCREATE(CRotoScopeView, CScrollView)

BEGIN_MESSAGE_MAP(CRotoScopeView, CScrollView)
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_MOUSEMOVE()
END_MESSAGE_MAP()

// CRotoScopeView construction/destruction

CRotoScopeView::CRotoScopeView()
{
}

CRotoScopeView::~CRotoScopeView()
{
}

//! Function called prior to creation of window
/*! \param cs A creation structure 
 * \returns true if successful */
BOOL CRotoScopeView::PreCreateWindow(CREATESTRUCT& cs)
{
    SetScrollSizes(MM_TEXT, CSize(720, 480));

	return CScrollView::PreCreateWindow(cs);
}


//! CRotoScopeView drawing
/*! \param pDC Pointer to a device context to draw on */
void CRotoScopeView::OnDraw(CDC* pDC)
{
    CRotoScopeDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

    // Get the image to draw
    const CGrImage &image = pDoc->GetImage();

    CSize imgSize(image.GetWidth(), image.GetHeight());

    // Draw any background we need around the image
    CRect rect;
    GetClientRect(&rect);
    BYTE bgGray = 128;
    CBrush bgBrush(RGB(bgGray, bgGray, bgGray));
    //pDC->FillRect(rect, &bgBrush);

    if(rect.Width() > imgSize.cx)
    {
        // Need to draw to the right of the image
        CRect rect2(rect);
        rect2.left = image.GetWidth();
        pDC->FillRect(rect2, &bgBrush);

        if(rect.Height() > imgSize.cy)
        {
            rect2 = rect;
            rect2.top = imgSize.cy;
            rect2.right = imgSize.cx;
            pDC->FillRect(rect2, &bgBrush);
        }
    } 
    else if(rect.Height() > imgSize.cy)
    {
        CRect rect2(rect);
        rect2.top = imgSize.cy;
        pDC->FillRect(rect2, &bgBrush);
    }

    SetScrollSizes(MM_TEXT, imgSize);
	image.Draw(pDC, 0, 0);
}


// CRotoScopeView diagnostics

#ifdef _DEBUG
void CRotoScopeView::AssertValid() const
{
	CView::AssertValid();
}

void CRotoScopeView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CRotoScopeDoc* CRotoScopeView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CRotoScopeDoc)));
	return (CRotoScopeDoc*)m_pDocument;
}
#endif //_DEBUG


// CRotoScopeView message handlers

BOOL CRotoScopeView::OnEraseBkgnd(CDC* pDC)
{
    return TRUE;
}

void CRotoScopeView::OnLButtonDown(UINT nFlags, CPoint point)
{
    GetDocument()->Mouse(point.x, point.y);

    CView::OnLButtonDown(nFlags, point);
}

void CRotoScopeView::OnMouseMove(UINT nFlags, CPoint point)
{
    if(nFlags & MK_LBUTTON)
        GetDocument()->Mouse(point.x, point.y);

    CView::OnMouseMove(nFlags, point);
}

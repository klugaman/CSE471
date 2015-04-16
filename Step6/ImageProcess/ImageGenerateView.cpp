//
// Name :         ImageGenerateView.cpp
// Description :  Implementation of the view class for the 
//                ImageProcess application.
//

// Notice:  Do not add any headers before stdafx.h
#include "stdafx.h"
#include "ImageProcess.h"

#include "ImageGenerateDoc.h"
#include "ImageGenerateView.h"
#include "graphics/GrImage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace std;

/////////////////////////////////////////////////////////////////////////////
// CImageGenerateView

IMPLEMENT_DYNCREATE(CImageGenerateView, CScrollView)

BEGIN_MESSAGE_MAP(CImageGenerateView, CScrollView)
	//{{AFX_MSG_MAP(CImageGenerateView)
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_CANCELMODE()
	ON_WM_TIMER()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CImageGenerateView construction/destruction

CImageGenerateView::CImageGenerateView()
{
	m_mouseDown = FALSE;

}

CImageGenerateView::~CImageGenerateView()
{
}

BOOL CImageGenerateView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CImageGenerateView drawing

void CImageGenerateView::OnDraw(CDC* pDC)
{
   // You can call whichever one of these you want to call.  If you call
   // DrawMe(), the drawing is directly to the screen.  If you call
   // DrawMeOffScreen, we draw to a buffer, then copy that buffer to the
   // screen.
   DrawMeOffScreen(pDC);
   //DrawMe(pDC);
}

//
// Name :         CImageGenerateView::DrawMeOffScreen(CDC *pdc)
// Description :  This is called by OnDraw to draw the window off-screen.  It
//                in turn calls DrawMe() to draw the actual window contents.
//

void CImageGenerateView::DrawMeOffScreen(CDC *pdc)
{
   //
   // First, we have to allocate a bitmap that is compatible with the
   // screen that we will draw in.  We determine how big it needs to
   // be right here.
   //

   // Get an enclosing rectangle
   CRect rcBounds;
   CSize size = GetTotalSize();
   rcBounds.SetRect(0, 0, size.cx, size.cy);

   // And convert bounds to device units. 
   // This is in case we happened to be using inches or something like that.
   pdc->LPtoDP(&rcBounds) ;

   // Create a Display Context that is compatible with the screen.
   CDC dcMem;
   dcMem.CreateCompatibleDC(pdc) ;

   // Create a really compatible bitmap.
   CBitmap bitOff;
   bitOff.CreateCompatibleBitmap(pdc, rcBounds.Width(), rcBounds.Height());

   // Select the bitmap into the memory DC.
   CBitmap* pOldBitmap = dcMem.SelectObject(&bitOff) ;

   // Save the memory DC state, since DrawMe might change it.
   int iSavedDC = dcMem.SaveDC();

   // Draw our control on the memory DC.
   DrawMe(&dcMem) ;

   // Restore the DC, since DrawMe might have changed mapping modes.
   dcMem.RestoreDC(iSavedDC) ;

   // We don't know what mapping mode pdc is using.
   // BitBlt uses logical coordinates.
   // Easiest thing is to change to MM_TEXT.
   pdc->SetMapMode(MM_TEXT) ;
   pdc->SetWindowOrg(0,0) ;
   pdc->SetViewportOrg(0,0) ;

   // Block transfer the memory device context to the screen.
   pdc->BitBlt( rcBounds.left,
                rcBounds.top,
                rcBounds.Width(),
                rcBounds.Height(),
                &dcMem,
                0,
                0,
                SRCCOPY) ;

   // Clean up.
   dcMem.SelectObject(pOldBitmap) ;
}

//
// Name :         CImageGenerateView::DrawMe(CDC *pdc)
// Description :  Draw the image into a display context.  This is what was
//                in OnDraw before...
//

void CImageGenerateView::DrawMe(CDC *pdc)
{
    CImageGenerateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

    // Get pointer to the image to draw...
    CGrImage *pImage = pDoc->GetImage();
    pImage->Draw(pdc, 0, 0);
}


/////////////////////////////////////////////////////////////////////////////
// CImageGenerateView diagnostics

#ifdef _DEBUG
void CImageGenerateView::AssertValid() const
{
	CView::AssertValid();
}

void CImageGenerateView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CImageGenerateDoc* CImageGenerateView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CImageGenerateDoc)));
	return (CImageGenerateDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CImageGenerateView message handlers

void CImageGenerateView::OnInitialUpdate() 
{
	CScrollView::OnInitialUpdate();

   CImageGenerateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

   CGrImage *pImage = pDoc->GetImage();

   SetScrollSizes(MM_TEXT, CSize(pImage->GetWidth(), pImage->GetHeight()));

   GetParentFrame()->RecalcLayout();
   ResizeParentToFit(FALSE);
}

void CImageGenerateView::CalcWindowRect(LPRECT lpClientRect, UINT nAdjustType) 
{
   CImageGenerateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

   CGrImage *pImage = pDoc->GetImage();

   SetScrollSizes(MM_TEXT, CSize(pImage->GetWidth(), pImage->GetHeight()));
   
	CScrollView::CalcWindowRect(lpClientRect, nAdjustType);
}



//
// Name :         CImageGenerateView::OnLButtonDown()
// Description :  Handle a left mouse button press.  
//

void CImageGenerateView::OnLButtonDown(UINT nFlags, CPoint point) 
{
   m_mouseDown = TRUE;
   SetCapture();

   Mouse(point, 1);

}


//
// Name :         CImageGenerateView::OnMouseMove()
// Description :  Handle a mouse movement.
//

void CImageGenerateView::OnMouseMove(UINT nFlags, CPoint point) 
{
   if(m_mouseDown)
   {
      Mouse(point, 0);
   }
}


void CImageGenerateView::Mouse(CPoint point, int p_button)
{
   CClientDC dc(this);
   OnPrepareDC(&dc);
   dc.DPtoLP(&point);

   CImageGenerateDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

   CGrImage *pImage = pDoc->GetImage();

   int mx = point.x;
   if(mx >= 0 && mx < pImage->GetWidth() &&
      point.y >= 0 && point.y < pImage->GetHeight())
   {
      switch(p_button)
      {
      case 0:
         GetDocument()->MouseMove(mx, pImage->GetHeight() - point.y - 1);
         break;
      }

   }

}


void CImageGenerateView::OnLButtonUp(UINT nFlags, CPoint point) 
{
   ReleaseCapture();
   m_mouseDown = FALSE;
}

void CImageGenerateView::OnCancelMode() 
{
	CScrollView::OnCancelMode();
}

void CImageGenerateView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint) 
{
   if(lHint)
      Invalidate(FALSE);	
   else
      Invalidate(TRUE);
}

void CImageGenerateView::OnTimer(UINT nIDEvent) 
{
	// TODO: Add your message handler code here and/or call default
	
	CScrollView::OnTimer(nIDEvent);
}



BOOL CImageGenerateView::OnEraseBkgnd(CDC* pDC) 
{
    CBrush br( GetSysColor( COLOR_WINDOW ) ); 
    FillOutsideRect( pDC, &br );
    return TRUE;                   // Erased
}


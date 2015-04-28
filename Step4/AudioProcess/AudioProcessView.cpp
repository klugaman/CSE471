
// AudioProcessView.cpp : implementation of the CAudioProcessView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "AudioProcess.h"
#endif

#include "AudioProcessDoc.h"
#include "AudioProcessView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int WAVEFORMY = 150;

// CAudioProcessView

IMPLEMENT_DYNCREATE(CAudioProcessView, CView)

BEGIN_MESSAGE_MAP(CAudioProcessView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CAudioProcessView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CAudioProcessView construction/destruction

CAudioProcessView::CAudioProcessView()
{
	// TODO: add construction code here

}

CAudioProcessView::~CAudioProcessView()
{
}

BOOL CAudioProcessView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CAudioProcessView drawing

void CAudioProcessView::OnDraw(CDC* pDC)
{
	CAudioProcessDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

    CFont font;
    font.CreateFont(18, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial"));
    CFont *oldfont = pDC->SelectObject(&font);

   pDC->SetTextAlign(TA_LEFT);

   int y = 0;
   const int skip = 20;
   CString line;

   line.Format(_T("Audio document:  %s"), pDoc->GetTitle());
   pDC->TextOut(0, y, line);  y += skip;

   line.Format(_T("Channels:  %d"), pDoc->NumChannels());
   pDC->TextOut(0, y, line);  y += skip;

   line.Format(_T("Sample Rate:  %10.2f"), pDoc->SampleRate());
   pDC->TextOut(0, y, line);  y += skip;
   
   line.Format(_T("Sample Frames:  %d"), pDoc->SampleFrames());
   pDC->TextOut(0, y, line);  y += skip;
   
   line.Format(_T("Duration:  %10.2f"), pDoc->SampleFrames() / pDoc->SampleRate());
   pDC->TextOut(0, y, line);  y += skip;
   
   line.Format(_T("Amplitude:  %10.2f"), pDoc->Amplitude());
   pDC->TextOut(0, y, line);  y += skip;
   
    pDC->SelectObject(oldfont);}


// CAudioProcessView printing


void CAudioProcessView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CAudioProcessView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAudioProcessView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAudioProcessView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CAudioProcessView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAudioProcessView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CAudioProcessView diagnostics

#ifdef _DEBUG
void CAudioProcessView::AssertValid() const
{
	CView::AssertValid();
}

void CAudioProcessView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAudioProcessDoc* CAudioProcessView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAudioProcessDoc)));
	return (CAudioProcessDoc*)m_pDocument;
}
#endif //_DEBUG


// CAudioProcessView message handlers


int CAudioProcessView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    if (CView::OnCreate(lpCreateStruct) == -1)
        return -1;

    CAudioProcessDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CRect rect;
    GetClientRect(&rect);

    m_waveformWnd.Create(this, rect, pDoc->GetWaveformBuffer());
    return 0;
}


void CAudioProcessView::OnMove(int x, int y)
{
    CView::OnMove(x, y);

    CRect rect;
    GetClientRect(&rect);

    rect.top += WAVEFORMY;
    if(rect.bottom < rect.top)
        rect.bottom = rect.top;
    m_waveformWnd.MoveWindow(&rect);
}


void CAudioProcessView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    CRect rect;
    GetClientRect(&rect);

    rect.top += WAVEFORMY;
    if(rect.bottom < rect.top)
        rect.bottom = rect.top;
    m_waveformWnd.MoveWindow(&rect);
}

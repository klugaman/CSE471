
// AudioGenerateView.cpp : implementation of the CAudioGenerateView class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "AudioProcess.h"
#endif

#include "AudioGenerateDoc.h"
#include "AudioGenerateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int WAVEFORMY = 150;



// CAudioGenerateView

IMPLEMENT_DYNCREATE(CAudioGenerateView, CView)

BEGIN_MESSAGE_MAP(CAudioGenerateView, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CAudioGenerateView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_CREATE()
	ON_WM_MOVE()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CAudioGenerateView construction/destruction

CAudioGenerateView::CAudioGenerateView()
{
	// TODO: add construction code here

}

CAudioGenerateView::~CAudioGenerateView()
{
}

BOOL CAudioGenerateView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CAudioGenerateView drawing

void CAudioGenerateView::OnDraw(CDC* pDC)
{
	CAudioGenerateDoc* pDoc = GetDocument();
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

    line.Format(_T("Duration:  %10.2f"), pDoc->Duration());
    pDC->TextOut(0, y, line);  y += skip;

    line.Format(_T("Amplitude:  %10.2f"), pDoc->Amplitude());
    pDC->TextOut(0, y, line);  y += skip;

    pDC->SelectObject(oldfont);
}


// CAudioGenerateView printing


void CAudioGenerateView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CAudioGenerateView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAudioGenerateView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAudioGenerateView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CAudioGenerateView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CAudioGenerateView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CAudioGenerateView diagnostics

#ifdef _DEBUG
void CAudioGenerateView::AssertValid() const
{
	CView::AssertValid();
}

void CAudioGenerateView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAudioGenerateDoc* CAudioGenerateView::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAudioGenerateDoc)));
	return (CAudioGenerateDoc*)m_pDocument;
}
#endif //_DEBUG


// CAudioGenerateView message handlers


int CAudioGenerateView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

    CAudioGenerateDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);

    CRect rect;
    GetClientRect(&rect);

    m_waveformWnd.Create(this, rect, pDoc->GetWaveformBuffer());
    return 0;
}


void CAudioGenerateView::OnMove(int x, int y)
{
    CView::OnMove(x, y);

    CRect rect;
    GetClientRect(&rect);

    rect.top += WAVEFORMY;
    if(rect.bottom < rect.top)
        rect.bottom = rect.top;
    m_waveformWnd.MoveWindow(&rect);
}


void CAudioGenerateView::OnSize(UINT nType, int cx, int cy)
{
    CView::OnSize(nType, cx, cy);

    CRect rect;
    GetClientRect(&rect);

    rect.top += WAVEFORMY;
    if(rect.bottom < rect.top)
        rect.bottom = rect.top;
    m_waveformWnd.MoveWindow(&rect);
}

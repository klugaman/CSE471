
// Step2View.cpp : implementation of the CStep2View class
//

#include "stdafx.h"
// SHARED_HANDLERS can be defined in an ATL project implementing preview, thumbnail
// and search filter handlers and allows sharing of document code with that project.
#ifndef SHARED_HANDLERS
#include "Step2.h"
#include "mmsystem.h"
#endif

#include "Step2Doc.h"
#include "Step2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStep2View

IMPLEMENT_DYNCREATE(CStep2View, CView)

BEGIN_MESSAGE_MAP(CStep2View, CView)
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CStep2View::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()

	
	ON_COMMAND(ID_PLAY_PLAYSMITH32772, &CStep2View::OnPlayPlaysmith32772)
	ON_WM_TIMER()
	ON_COMMAND(ID_PLAY_RESET, &CStep2View::OnPlayReset)
END_MESSAGE_MAP()

// CStep2View construction/destruction

CStep2View::CStep2View()
{
	m_state = Start;      // our initial state
	// TODO: add construction code here
	m_splash.LoadBitmap(IDB_SPLASH);
	m_smith.LoadBitmap(IDB_SMITH);
	m_matrix.LoadBitmap(IDB_MATRIX);

	BITMAP map;
	m_splash.GetBitmap(&map);
	m_splashwid = map.bmWidth;
	m_splashhit = map.bmHeight;

	m_firstdraw = true;
	m_reset = true;
	m_timer = 0;
}

CStep2View::~CStep2View()
{
}

void CStep2View::OnFirstDraw()
{
	m_starttime = timeGetTime();
	OnTimer(1);
}

BOOL CStep2View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

// CStep2View drawing

void CStep2View::OnDraw(CDC* pDC)
{
	if (m_firstdraw)
	{
		m_firstdraw = false;
		m_reset = false;
		OnFirstDraw();
	}

	CStep2Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: add draw code for native data here
	CDC bmpDC;

	CPen pen;
	pen.CreatePen(PS_SOLID, 2, RGB(0, 0, 0));

	CPen *oldpen = pDC->SelectObject(&pen);


	CFont font;
	font.CreateFont(20, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, TEXT("Arial"));

	CFont *oldfont = pDC->SelectObject(&font);

	switch (m_state)
	{

	case Name:
		pDC->MoveTo(10, 10);
		pDC->LineTo(10, 100);
		pDC->LineTo(700, 100);
		pDC->LineTo(700, 10);
		pDC->LineTo(10, 10);
		pDC->SelectObject(oldpen);
		pDC->TextOut(270, 45, L"My Step by Michael Chen");
		pDC->SelectObject(oldfont);
		break;

	case Image:
		bmpDC.CreateCompatibleDC(pDC);
		bmpDC.SelectObject(&m_matrix);
		pDC->BitBlt(0, 0, 640, 480, &bmpDC, 0, 0, SRCCOPY);
		break;

	case Caption:
		bmpDC.CreateCompatibleDC(pDC);
		bmpDC.SelectObject(&m_matrix);
		pDC->BitBlt(0, 0, 640, 480, &bmpDC, 0, 0, SRCCOPY);
		pDC->TextOut(240, 500, L"My Step by Michael Chen");
		pDC->SelectObject(oldfont);
		break;

	case Music:
		bmpDC.CreateCompatibleDC(pDC);
		bmpDC.SelectObject(&m_matrix);
		pDC->BitBlt(0, 0, 640, 480, &bmpDC, 0, 0, SRCCOPY);
		pDC->TextOut(240,500, L"My Step by Michael Chen");
		pDC->SelectObject(oldfont);
		break;

	default:
		// Display nothing
		break;
	}
}


// CStep2View printing


void CStep2View::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CStep2View::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CStep2View::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CStep2View::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

void CStep2View::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CStep2View::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CStep2View diagnostics

#ifdef _DEBUG
void CStep2View::AssertValid() const
{
	CView::AssertValid();
}

void CStep2View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStep2Doc* CStep2View::GetDocument() const // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStep2Doc)));
	return (CStep2Doc*)m_pDocument;
}
#endif //_DEBUG


// CStep2View message handlers





void CStep2View::OnPlayPlaysmith32772()
{
	// TODO: Add your command handler code here
	PlaySound(MAKEINTRESOURCE(IDR_HEARTHAT), AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);

}


void CStep2View::OnTimer(UINT_PTR nIDEvent)
{
	// Kill any existing timer
	if (m_timer)
	{
		KillTimer(m_timer);
		m_timer = 0;
	}

	// Force redraw whenever the timer fires
	Invalidate();

	// This will keep track of the relative time
	// to the next state.
	DWORD nexteventtime;

	switch (m_state)
	{
	case Start:
		// If we are in the state state, just move directly
		// to the Smith state
	
		m_state = Sound_1;
		nexteventtime = 3000;
		


		break;
		

	case Sound_1:
		// The Smith state is ending, we are changing to the HearThat state
		
		m_state = Blank;
		// What we do at the end of state Smith, entering state HearThat
		PlaySound(MAKEINTRESOURCE(IDR_SILLY), AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
		nexteventtime = 8000;

		break;


	case Blank:

		m_state = Name;
		nexteventtime = 12000;
		break;


	case Name:
		// What we at the end of state HearThat, entering state Chill
		m_state = Image;
		nexteventtime = 15000;
		break;

	case Image:
		m_state = Caption;
		nexteventtime = 20000;
		break;

	case Caption:
		m_state = Music;
		PlaySound(MAKEINTRESOURCE(IDR_GOLDRUSH), AfxGetInstanceHandle(), SND_RESOURCE | SND_ASYNC);
		nexteventtime = 30000;
		break;

	case Music:
		m_state = Done;

		break;

	}

	// Only reset the timer if we are not done
	if (m_state != Done)
	{
		DWORD currenttime = timeGetTime();
		DWORD nexttime = m_starttime + nexteventtime;
		DWORD tillnext = nexttime > currenttime ? nexttime - currenttime : 0;

		m_timer = SetTimer(1, tillnext, NULL);
	}
}


void CStep2View::OnPlayReset()
{
	// TODO: Add your command handler code here

	PlaySound(NULL, 0, 0);
	m_firstdraw = true;
	m_starttime = timeGetTime();
	OnTimer(1);

	m_state = Start;

}

// Progress.cpp: implementation of the CProgress class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "audioprocess.h"
#include "Progress.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CProgress::CProgress()
{
	m_lastProgress = -1;
}

CProgress::~CProgress()
{

}

void CProgress::ProgressBegin(CDocument *p_doc)
{
   POSITION pos = p_doc->GetFirstViewPosition();
   while (pos != NULL)
   {
      CView* pView = p_doc->GetNextView(pos);
      pView->EnableWindow(FALSE);
   }   

   p_doc->BeginWaitCursor();
   m_lastProgress = 0;
   m_progressdlg.Create(IDD_PROGRESS_DLG);
   m_progressdlg.ShowWindow(SW_SHOW);
   m_progressdlg.m_progress.SetRange(0, 1000);
   m_progressdlg.m_progress.SetPos(0);
   m_progressdlg.AbortClear();
}

bool CProgress::Progress(double p)
{
	int newProgress = int(p * 1000);
	if(newProgress != m_lastProgress)
	{
		m_lastProgress = newProgress;
		m_progressdlg.m_progress.SetPos(newProgress);
	}

   return !m_progressdlg.Abort();
}

void CProgress::ProgressEnd(CDocument *p_doc)
{
   m_progressdlg.DestroyWindow();
   p_doc->EndWaitCursor();

   POSITION pos = p_doc->GetFirstViewPosition();
   while (pos != NULL)
   {
      CView* pView = p_doc->GetNextView(pos);
      pView->EnableWindow(TRUE);
   }   
}

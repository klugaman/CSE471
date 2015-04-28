// Progress.h: interface for the CProgress class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROGRESS_H__D5977EFD_A4DB_4690_8549_54DDEB864FBE__INCLUDED_)
#define AFX_PROGRESS_H__D5977EFD_A4DB_4690_8549_54DDEB864FBE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ProgressDlg.h"	// Added by ClassView

class CProgress  
{
public:
	void ProgressEnd(CDocument *p_doc);
	bool Progress(double p);
	void ProgressBegin(CDocument *p_doc);
	CProgress();
	virtual ~CProgress();

private:
	CProgressDlg m_progressdlg;

	int m_lastProgress;
};

#endif // !defined(AFX_PROGRESS_H__D5977EFD_A4DB_4690_8549_54DDEB864FBE__INCLUDED_)

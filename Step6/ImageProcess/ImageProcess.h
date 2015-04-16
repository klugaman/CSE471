// ImageProcess.h : main header file for the IMAGEPROCESS application
//

#if !defined(AFX_IMAGEPROCESS_H__9DC9E635_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)
#define AFX_IMAGEPROCESS_H__9DC9E635_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CImageProcessApp:
// See ImageProcess.cpp for the implementation of this class
//

class CImageProcessApp : public CWinApp
{
public:
	CImageProcessApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CImageProcessApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CImageProcessApp)
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
    virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IMAGEPROCESS_H__9DC9E635_7AAC_11D3_B40B_00C04F53D1ED__INCLUDED_)

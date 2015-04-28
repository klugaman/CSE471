
// AudioProcess.h : main header file for the AudioProcess application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols
#include "audio/DirSound.h"

// CAudioProcessApp:
// See AudioProcess.cpp for the implementation of this class
//

class CAudioProcessApp : public CWinAppEx
{
public:
	CAudioProcessApp();

	CDirSound *DirSound() {return &m_DirSound;}

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()

private:
    CDirSound   m_DirSound;

public:
	afx_msg void OnFileNew();
	afx_msg void OnFileOpen();
	virtual CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
};

extern CAudioProcessApp theApp;

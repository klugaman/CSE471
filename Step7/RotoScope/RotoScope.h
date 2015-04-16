// RotoScope.h : main header file for the RotoScope application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CRotoScopeApp:
// See RotoScope.cpp for the implementation of this class
//

class CRotoScopeApp : public CWinApp
{
public:
	CRotoScopeApp();


// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CRotoScopeApp theApp;
// CartmanDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ImageProcess.h"
#include "CartmanDlg.h"


// CCartmanDlg dialog

IMPLEMENT_DYNAMIC(CCartmanDlg, CDialog)
CCartmanDlg::CCartmanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCartmanDlg::IDD, pParent)
    , m_nearest(FALSE)
    , m_transparent(FALSE)
{
}

CCartmanDlg::~CCartmanDlg()
{
}

void CCartmanDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Check(pDX, IDC_NEAREST, m_nearest);
    DDX_Check(pDX, IDC_TRANSPARENT, m_transparent);
}


BEGIN_MESSAGE_MAP(CCartmanDlg, CDialog)
END_MESSAGE_MAP()


// CCartmanDlg message handlers

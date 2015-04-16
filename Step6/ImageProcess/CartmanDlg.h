#pragma once


// CCartmanDlg dialog

class CCartmanDlg : public CDialog
{
	DECLARE_DYNAMIC(CCartmanDlg)

public:
	CCartmanDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CCartmanDlg();

// Dialog Data
	enum { IDD = IDD_CARTMANDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
    BOOL m_nearest;
    BOOL m_transparent;
};

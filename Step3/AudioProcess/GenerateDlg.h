#if !defined(AFX_GENERATEDLG_H__35FCE835_64B8_11D3_B40A_00C04F53D1ED__INCLUDED_)
#define AFX_GENERATEDLG_H__35FCE835_64B8_11D3_B40A_00C04F53D1ED__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GenerateDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGenerateDlg dialog

class CGenerateDlg : public CDialog
{
// Construction
public:
	CGenerateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGenerateDlg)
	enum { IDD = IDD_GEN_AUDIO_DLG };
	double	m_SampleRate;
	int		m_Channels;
	double	m_Duration;
	double	m_Amplitude;
	double	m_freq1;
	double	m_freq2;
	double	m_vibratoamp;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGenerateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGenerateDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GENERATEDLG_H__35FCE835_64B8_11D3_B40A_00C04F53D1ED__INCLUDED_)

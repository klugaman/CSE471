// GenerateDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AudioProcess.h"
#include "GenerateDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGenerateDlg dialog


CGenerateDlg::CGenerateDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenerateDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGenerateDlg)
	m_SampleRate = 0.0;
	m_Channels = 0;
	m_Duration = 0.0;
	m_Amplitude = 0.0;
	m_freq1 = 0.0;
	m_freq2 = 0.0;
	m_vibratoamp = 0.0;
	//}}AFX_DATA_INIT
}


void CGenerateDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGenerateDlg)
	DDX_Text(pDX, IDC_SAMPLE_RATE, m_SampleRate);
	DDV_MinMaxDouble(pDX, m_SampleRate, 8000., 48000.);
	DDX_Text(pDX, IDC_CHANNELS, m_Channels);
	DDV_MinMaxInt(pDX, m_Channels, 1, 2);
	DDX_Text(pDX, IDC_DURATION, m_Duration);
	DDV_MinMaxDouble(pDX, m_Duration, 1.e-006, 10000000.);
	DDX_Text(pDX, IDC_AMPLITUDE, m_Amplitude);
	DDX_Text(pDX, IDC_FREQ1, m_freq1);
	DDX_Text(pDX, IDC_FREQ2, m_freq2);
	DDX_Text(pDX, IDC_VIBRATOAMP, m_vibratoamp);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGenerateDlg, CDialog)
	//{{AFX_MSG_MAP(CGenerateDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGenerateDlg message handlers

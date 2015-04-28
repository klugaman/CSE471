
// AudioProcess.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "AudioProcess.h"
#include "MainFrm.h"

#include "ChildFrm.h"
#include "AudioProcessDoc.h"
#include "AudioProcessView.h"
#include "AudioGenerateDoc.h"
#include "AudioGenerateView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAudioProcessApp

BEGIN_MESSAGE_MAP(CAudioProcessApp, CWinAppEx)
	ON_COMMAND(ID_APP_ABOUT, &CAudioProcessApp::OnAppAbout)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, &CWinAppEx::OnFilePrintSetup)
	ON_COMMAND(ID_FILE_NEW, &CAudioProcessApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, &CAudioProcessApp::OnFileOpen)
END_MESSAGE_MAP()


// CAudioProcessApp construction

CAudioProcessApp::CAudioProcessApp()
{
	m_bHiColorIcons = TRUE;

	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_ALL_ASPECTS;
#ifdef _MANAGED
	// If the application is built using Common Language Runtime support (/clr):
	//     1) This additional setting is needed for Restart Manager support to work properly.
	//     2) In your project, you must add a reference to System.Windows.Forms in order to build.
	System::Windows::Forms::Application::SetUnhandledExceptionMode(System::Windows::Forms::UnhandledExceptionMode::ThrowException);
#endif

	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("AudioProcess.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CAudioProcessApp object

CAudioProcessApp theApp;


// CAudioProcessApp initialization

BOOL CAudioProcessApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinAppEx::InitInstance();


	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	AfxEnableControlContainer();

	EnableTaskbarInteraction();

	// AfxInitRichEdit2() is required to use RichEdit control	
	// AfxInitRichEdit2();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	LoadStdProfileSettings(4);  // Load standard INI file options (including MRU)


	InitContextMenuManager();

	InitKeyboardManager();

	InitTooltipManager();
	CMFCToolTipInfo ttParams;
	ttParams.m_bVislManagerTheme = TRUE;
	theApp.GetTooltipManager()->SetTooltipParams(AFX_TOOLTIP_TYPE_ALL,
		RUNTIME_CLASS(CMFCToolTipCtrl), &ttParams);

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CMultiDocTemplate* pDocTemplate;

	pDocTemplate = new CMultiDocTemplate(IDR_AudioGenerateTYPE,
		RUNTIME_CLASS(CAudioGenerateDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CAudioGenerateView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(IDR_AudioProcessTYPE,
		RUNTIME_CLASS(CAudioProcessDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(CAudioProcessView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame || !pMainFrame->LoadFrame(IDR_MAINFRAME))
	{
		delete pMainFrame;
		return FALSE;
	}
	m_pMainWnd = pMainFrame;
	// call DragAcceptFiles only if there's a suffix
	//  In an MDI app, this should occur immediately after setting m_pMainWnd
	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Initialize our DirectSound interface class.  This must be
    // done before we dispatch any command line options since
    // that is when we create new windows.
    if(!m_DirSound.Initialize())
        return false;

	// Parse command line for standard shell commands, DDE, file open
	//CCommandLineInfo cmdInfo;
	//ParseCommandLine(cmdInfo);

	// Enable DDE Execute open
	//EnableShellOpen();
	//RegisterShellFileTypes(TRUE);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	// The main window has been initialized, so show and update it
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}

int CAudioProcessApp::ExitInstance()
{
	//TODO: handle additional resources you may have added
	AfxOleTerm(FALSE);

	return CWinAppEx::ExitInstance();
}

// CAudioProcessApp message handlers


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()

// App command to run the dialog
void CAudioProcessApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CAudioProcessApp customization load/save methods

void CAudioProcessApp::PreLoadState()
{
	BOOL bNameValid;
	CString strName;
	bNameValid = strName.LoadString(IDS_EDIT_MENU);
	ASSERT(bNameValid);
	GetContextMenuManager()->AddMenu(strName, IDR_POPUP_EDIT);
}

void CAudioProcessApp::LoadCustomState()
{
}

void CAudioProcessApp::SaveCustomState()
{
}

// CAudioProcessApp message handlers





void CAudioProcessApp::OnFileNew()
{
   POSITION position = GetFirstDocTemplatePosition();
   CDocTemplate* pTemplate = GetNextDocTemplate(position);
   pTemplate->OpenDocumentFile(NULL);
}


void CAudioProcessApp::OnFileOpen()
{
	static WCHAR BASED_CODE szFilter[] = L"Audio Files (*.wav;*.mp3)|*.wav; *.mp3|All Files (*.*)|*.*||";

	CFileDialog dlg(TRUE, NULL, NULL, 0, szFilter, NULL);
	if(dlg.DoModal() != IDOK)
      return;

   POSITION position = GetFirstDocTemplatePosition();
   GetNextDocTemplate(position);
   CDocTemplate* pTemplate = GetNextDocTemplate(position);
   pTemplate->OpenDocumentFile(dlg.GetPathName());
}


CDocument* CAudioProcessApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
   POSITION position = GetFirstDocTemplatePosition();
   GetNextDocTemplate(position);
   CDocTemplate* pTemplate = GetNextDocTemplate(position);
   return pTemplate->OpenDocumentFile(lpszFileName);
}

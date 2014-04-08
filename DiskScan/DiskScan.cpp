// DiskScan.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "DiskScan.h"

#include "MainFrm.h"
#include "ChildFrm.h"
#include "DiskScanDoc.h"
#include "DiskScanView.h"
#include "Fat32Doc.h"
#include "NtfsDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDiskScanApp

BEGIN_MESSAGE_MAP(CDiskScanApp, CWinApp)
	//{{AFX_MSG_MAP(CDiskScanApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
// 	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
// 	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDiskScanApp construction

CDiskScanApp::CDiskScanApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CDiskScanApp object

CDiskScanApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CDiskScanApp initialization

BOOL CDiskScanApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	//用于管理磁盘的文档管理器
	m_pDiskDocTemplate = new CMultiDocTemplate(
		IDR_DISKSCTYPE,
		RUNTIME_CLASS(CDiskDoc),
		RUNTIME_CLASS(CChildFrm), 
		RUNTIME_CLASS(CHexDataView));
	AddDocTemplate(m_pDiskDocTemplate);

	//用于管理Fat32卷的文档模板
	m_pFat32DocTemplate = new CMultiDocTemplate(
		IDR_FAT32TYPE,
		RUNTIME_CLASS(CFat32Doc),
		RUNTIME_CLASS(CChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CHexDataView));
	AddDocTemplate(m_pFat32DocTemplate);

	//用于管理Ntfs卷的文档模板
	m_pNtfsDocTemplate = new CMultiDocTemplate(
		IDR_NTFSTYPE,
		RUNTIME_CLASS(CNtfsDoc),
		RUNTIME_CLASS(CChildFrm), // custom MDI child frame
		RUNTIME_CLASS(CHexDataView));
	AddDocTemplate(m_pNtfsDocTemplate);



	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Enable drag/drop open
	m_pMainWnd->DragAcceptFiles();

	// Enable DDE Execute open
	EnableShellOpen();
	RegisterShellFileTypes(TRUE);

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	//打开时不创建文档
	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew){
		cmdInfo.m_nShellCommand = CCommandLineInfo::FileNothing;
	}

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CDiskScanApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CDiskScanApp message handlers


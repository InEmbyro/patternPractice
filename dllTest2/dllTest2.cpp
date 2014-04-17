
// dllTest2.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "afxwinappex.h"
#include "afxdialogex.h"
#include "dllTest2.h"
#include "../canEngine/canEngineApi.h"

#include "MainFrm.h"

#include "ChildFrm.h"
#include "CGridFormChildFrm.h"
#include "CGridFormThread.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CdllTest2App

BEGIN_MESSAGE_MAP(CdllTest2App, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CdllTest2App::OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, &CdllTest2App::OnFileNew)
END_MESSAGE_MAP()


// CdllTest2App construction
CdllTest2App::~CdllTest2App()
{
	delete _pGridForm;
}

CdllTest2App::CdllTest2App()
{
	// TODO: replace application ID string below with unique ID string; recommended
	// format for string is CompanyName.ProductName.SubProduct.VersionInformation
	SetAppID(_T("dllTest2.AppID.NoVersion"));

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

// The one and only CdllTest2App object

CdllTest2App theApp;


// CdllTest2App initialization

BOOL CdllTest2App::InitInstance()
{
	CWinApp::InitInstance();


	EnableTaskbarInteraction(FALSE);

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


	// To create the main window, this code creates a new frame window
	// object and then sets it as the application's main window object
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// create main MDI frame window
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// try to load shared MDI menus and accelerator table
	//TODO: add additional member variables and load calls for
	//	additional menu types your application may need
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_dllTest2TYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_dllTest2TYPE));





	// The main window has been initialized, so show and update it
	pFrame->ShowWindow(m_nCmdShow);
	pFrame->UpdateWindow();

	InitCan();
	_pGridForm = new CGridFormThread();
	if (_pGridForm) {
		if (!_pGridForm->InitThread())
			AfxMessageBox(_T("_pGridForm->InitThread"));
	}

	return TRUE;
}

int CdllTest2App::ExitInstance()
{
	//TODO: handle additional resources you may have added
	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}

// CdllTest2App message handlers

void CdllTest2App::OnFileNew() 
{
	CMainFrame* pFrame = STATIC_DOWNCAST(CMainFrame, m_pMainWnd);
	// create a new MDI child window
	pFrame->CreateNewChild(
		RUNTIME_CLASS(CGridFormChildFrm), IDR_dllTest2TYPE, m_hMDIMenu, m_hMDIAccel);
	CString name;
	name.SetString(_T("\\\\.\\mailslot\\wnc_grid_view"));

	POSITION pos;
	pos = RegisterAcquire(name);
	if (pos == NULL) {
		AfxMessageBox(_T("wnc_grid_view mailslot fail"));
		return;
	}
	_pGridForm->SetInfoHandle(InforEventAcquire(pos));
	_pGridForm->SetMailHandle(MailSlotAcquire(pos));
	if (!_pGridForm->InitThread())
		AfxMessageBox(_T("_pGridForm->InitThread"));

}

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
void CdllTest2App::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

// CdllTest2App message handlers




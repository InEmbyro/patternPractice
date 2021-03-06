
// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "dllTest2.h"

#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
//	ON_WM_KEYDOWN()
ON_MESSAGE(WM_FAKE_KEYDOWN, &CMainFrame::OnFakeKeydown)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

#if 0
	if (!m_wndStatusBar.Create(this))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT));
#endif
	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CMDIFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return TRUE;
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame message handlers


void CMainFrame::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	UINT uId = AFX_IDM_FIRST_MDICHILD;
#if 1
	for (CWnd *pWnd = GetDescendantWindow(uId); pWnd; pWnd = GetDescendantWindow(++uId)) {
		pWnd->SendMessage(WM_CLOSE);
	}
#endif
	CMDIFrameWnd::OnClose();
}


afx_msg LRESULT CMainFrame::OnFakeKeydown(WPARAM wParam, LPARAM lParam)
{
	UINT uId = AFX_IDM_FIRST_MDICHILD;
#if 1
	for (CWnd *pWnd = GetDescendantWindow(uId); pWnd; pWnd = GetDescendantWindow(++uId)) {
		pWnd->SendMessage(WM_FAKE_KEYDOWN);
	}
#endif
	return 0;
}

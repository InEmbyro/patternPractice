// dllmain.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include "targetList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE targetListDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果您使用 lpReserved，請移除這個
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("targetList.DLL 正在初始化!\n");
		
		// 擴充 DLL 的一次初始化
		if (!AfxInitExtensionModule(targetListDLL, hInstance))
			return 0;

		// 將這個 DLL 插入資源鏈結
		// 注意: 如果這個擴充 DLL 是由
		//  MFC 標準 DLL (例如 ActiveX 控制項)，
		//  而非 MFC 應用程式以隱含方式連結，
		//  請從 DllMain 移除這一行，並且將它
		//  置入從這個擴充 DLL 所匯出的單獨函式中。使用這個擴充 DLL 的
		//  標準 DLL 應該就會明確地呼叫那個
		//  函式來初始化這個擴充 DLL。否則，
		//  CDynLinkLibrary 物件將不會附加到
		//  標準 DLL 的資源鏈結，同時會造成嚴重的
		//  問題。

		new CDynLinkLibrary(targetListDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("targetList.DLL 正在終止!\n");

		// 呼叫解構函式前先終止程式庫
		AfxTermExtensionModule(targetListDLL);
	}
	return 1;   // 確定
}

extern "C" AFX_EXT_API LPVOID WINAPI InitTargetListForm()
{
	// create a new CDynLinkLibrary for this app
	#pragma warning(push)
	#pragma warning(disable:6211) //silence prefast warning because CDynLinkLibrary is cleaned up elsewhere
	new CDynLinkLibrary(targetListDLL);
	#pragma warning(pop)

	CWinApp* pApp = AfxGetApp();
	ENSURE(pApp != NULL);
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)(pApp->m_pMainWnd);
	return (LPVOID) pFrame->CreateNewChild(RUNTIME_CLASS(CTargetListForm), IDR_TARGETMENU, 0, 0);
}
// D:\workspace\myDr\targetList\targetList.cpp : 實作檔
//
// CTargetList

IMPLEMENT_DYNCREATE(CTargetList, CFormView)

CTargetList::CTargetList()
	: CFormView(CTargetList::IDD)
{

}

CTargetList::~CTargetList()
{
}

void CTargetList::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TARGETLIST_COMBO, m_combo);
	DDX_Control(pDX, IDC_TARGETLISTLIST, m_listctrl);
}

BEGIN_MESSAGE_MAP(CTargetList, CFormView)
//	ON_WM_CREATE()
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CTargetList 診斷

#ifdef _DEBUG
void CTargetList::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CTargetList::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CTargetList 訊息處理常式
// D:\workspace\myDr\targetList\targetList.cpp : 實作檔
//
// CTargetListForm

IMPLEMENT_DYNCREATE(CTargetListForm, CMDIChildWnd)

CTargetListForm::CTargetListForm()
{
	_pView = NULL;
}

CTargetListForm::~CTargetListForm()
{
}


BEGIN_MESSAGE_MAP(CTargetListForm, CMDIChildWnd)
//	ON_WM_CREATE()
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CTargetListForm 訊息處理常式

BOOL CTargetList::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


int CTargetListForm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(targetListDLL.hModule);
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1) {
		AfxSetResourceHandle(hInstOld);
		return -1;
	}

	// TODO:  在此加入特別建立的程式碼
	_pView = new CTargetList();
	if (!_pView) {
		CString error;
		error.Format(_T("CTargetListForm::Create, %d"), GetLastError());
		AfxMessageBox(error);
		return 0;
	}
	CRect rect;
	rect.SetRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy);
	_pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rect, this, AFX_IDW_PANE_FIRST, NULL);
	_pView->UpdateData(FALSE);
	_pView->m_listctrl.SetWindowPos(NULL, 0, 0, rect.Width(), rect.Height(), SWP_NOMOVE);

	AfxSetResourceHandle(hInstOld);
	return 0;
}


void CTargetList::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼
	if (m_listctrl.m_hWnd)
		m_listctrl.SetWindowPos(NULL, 0, 0, cx, cy,SWP_NOMOVE);
}

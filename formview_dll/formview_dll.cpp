// dllmain.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include "resource.h"
#include "CGridFormChildFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE formview_dll = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果您使用 lpReserved，請移除這個
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("formview_dll.DLL 正在初始化!\n");
		
		// 擴充 DLL 的一次初始化
		if (!AfxInitExtensionModule(formview_dll, hInstance))
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

		//new CDynLinkLibrary(formview_dllDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("formview_dll.DLL 正在終止!\n");

		// 呼叫解構函式前先終止程式庫
		AfxTermExtensionModule(formview_dll);
	}
	return 1;   // 確定
}

extern "C" AFX_EXT_API void WINAPI InitGridForm()
{
	// create a new CDynLinkLibrary for this app
	#pragma warning(push)
	#pragma warning(disable:6211) //silence prefast warning because CDynLinkLibrary is cleaned up elsewhere
	new CDynLinkLibrary(formview_dll);
	#pragma warning(pop)

	CWinApp* pApp = AfxGetApp();
	ENSURE(pApp != NULL);
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)(pApp->m_pMainWnd);
	pFrame->CreateNewChild(RUNTIME_CLASS(CGridFormChildFrm), IDR_GRIDMENU);

}

// CGridFormChildFrm

IMPLEMENT_DYNCREATE(CGridFormChildFrm, CMDIChildWnd)
BEGIN_MESSAGE_MAP(CGridFormChildFrm, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

CGridFormChildFrm::CGridFormChildFrm()
{

}

CGridFormChildFrm::~CGridFormChildFrm()
{
}




// CGridFormChildFrm 訊息處理常式


int CGridFormChildFrm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(formview_dll.hModule);

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1) {
		AfxSetResourceHandle(hInstOld);
		return -1;
	}

	// TODO:  在此加入特別建立的程式碼
	_pView = new GridFormChildView();
	CRect rect;
	rect.SetRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy);
	_pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rect, this, AFX_IDW_PANE_FIRST, NULL);
	
	/*	Calling UpdateDate(FALSE) will cause GridFormChildView::DoDataExchange is called.
		In the function, GridFormChildView::m_hWnd will be assigned */
	_pView->UpdateData(FALSE);

	AfxSetResourceHandle(hInstOld);
	return 0;
}

void CGridFormChildFrm::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//ShowWindow(SW_HIDE);
#if 0
	DeregisterAcquire(rawPos);
	_pView->CloseAllHnd();
	_pView->_pThread->TerminateThread();
	WaitForSingleObject(_pView->_pThread->getConfirmHnd(), 5000);

	Sleep(10);
#endif
	CMDIChildWnd::OnClose();
}


void CGridFormChildFrm::OnDestroy()
{
	CMDIChildWnd::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼
}

// GridFormChildView
IMPLEMENT_DYNCREATE(GridFormChildView, CFormView)
BEGIN_MESSAGE_MAP(GridFormChildView, CFormView)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_DRAW, &GridFormChildView::OnUserDraw)
END_MESSAGE_MAP()

GridFormChildView::GridFormChildView()
	: CFormView(GridFormChildView::IDD), _pThread(NULL)
{
	_counter = 0;
	_ListMutex = CreateMutex(NULL, FALSE, NULL);
}

GridFormChildView::~GridFormChildView()
{
	CloseHandle(_ListMutex);
//	delete _pThread;
}

void GridFormChildView::OnInitialUpdate()
{
	int idx = 0;
	if (m_GridList.GetSafeHwnd() != NULL) {
		m_GridList.InsertColumn(idx++, _T("#Msgs"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Ident"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Len"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Data Bytes [7...0]"), LVCFMT_LEFT, 200, 0);
	}
}

void GridFormChildView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRIDLIST, m_GridList);
}

BOOL GridFormChildView::Create(LPCTSTR p1, LPCTSTR p2, DWORD p3, const RECT& p4, CWnd* p5, UINT p6, CCreateContext*p7)
{
	return CFormView::Create(p1, p2, p3, p4, p5, p6, p7);
}

// GridFormChildView 診斷

#ifdef _DEBUG
void GridFormChildView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void GridFormChildView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

void GridFormChildView::CloseAllHnd()
{
	::SendMessage(m_GridList.m_hWnd, WM_CLOSE, 0, 0);
}

void GridFormChildView::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CFormView::OnClose();
}


afx_msg LRESULT GridFormChildView::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	if (GetSafeHwnd() == NULL)
		return 0;
	if (_List.IsEmpty())
		return 0;

	POSITION pos;
	PARAM_STRUCT data;
	CString str;
	int idx = 0;

	pos = _List.GetHeadPosition();
	idx = 0;
	if (_counter == (sizeof(_counter)-1))
		_counter = 0;
	while (pos) {
		data = _List.GetNext(pos);
		if (m_GridList.GetItemCount() > 10)
			m_GridList.DeleteItem(10);
		str.Format(_T("%d"), _counter++);
		m_GridList.InsertItem(idx, str);
		str.Format(_T("0x%X"), data.Ident);
		m_GridList.SetItemText(idx, 1, str);
		str.Format(_T("%d"), data.DataLength);
		m_GridList.SetItemText(idx, 2, str);
	}
	WaitForSingleObject(_ListMutex, INFINITE);
	_List.RemoveAll();
	ReleaseMutex(_ListMutex);
	return 0;
}

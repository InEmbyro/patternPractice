// dllmain.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include "resource.h"
#include "formview_dll.h"
#include "../canEngine/canEngineApi.h"
#include "CGridFormThread.h"

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

extern "C" AFX_EXT_API LPVOID WINAPI InitGridForm()
{
	// create a new CDynLinkLibrary for this app
	#pragma warning(push)
	#pragma warning(disable:6211) //silence prefast warning because CDynLinkLibrary is cleaned up elsewhere
	new CDynLinkLibrary(formview_dll);
	#pragma warning(pop)

	
	CWinApp* pApp = AfxGetApp();
	ENSURE(pApp != NULL);
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)(pApp->m_pMainWnd);
	return (LPVOID) pFrame->CreateNewChild(RUNTIME_CLASS(CGridFormChildFrm), IDR_GRIDMENU, 0, 0);
}

// CGridFormChildFrm

IMPLEMENT_DYNCREATE(CGridFormChildFrm, CMDIChildWnd)
BEGIN_MESSAGE_MAP(CGridFormChildFrm, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_WM_NCRBUTTONUP()
END_MESSAGE_MAP()

CGridFormChildFrm::CGridFormChildFrm()
	:pGridFormThread(NULL)
{
	pGridFormThread = new CGridFormThread();
}

CGridFormChildFrm::~CGridFormChildFrm()
{
	delete pGridFormThread;
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

	if (!pGridFormThread) {
		AfxMessageBox(_T("_pGridFormThread"));
		return -1;
	}
	
	CString name;
	POSITION pos;

	name = GridFormChildView::mailslot;
	pos = RegisterAcquire(name);
	if (pos == NULL) {
		AfxMessageBox(_T("GridFormChildView::mailslot"));
		return -1;
	}

	pGridFormThread->_pView = _pView;
	pGridFormThread->SetInfoHandle(InforEventAcquire(pos));
	pGridFormThread->SetMailHandle(MailSlotAcquire(pos));
	rawPos = pos;

	if (!pGridFormThread->InitThread()) {
		AfxMessageBox(_T("_pGridFormThread->InitThread"));
		DeregisterAcquire(rawPos);
		_pView->CloseAllHnd();
	}
	_pView->pFormThread = pGridFormThread;

	UpdateData(FALSE);
	AfxSetResourceHandle(hInstOld);
	return 0;
}

void CGridFormChildFrm::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	_pView->pFormThread->TerminateThread();
	WaitForSingleObject(_pView->pFormThread->getConfirmHnd(), 5000);
	DeregisterAcquire(rawPos);
	_pView->CloseAllHnd();

	CMDIChildWnd::OnClose();
}

const char* GridFormChildView::mailslot = "\\\\.\\mailslot\\wnc_grid_view";

// GridFormChildView
IMPLEMENT_DYNCREATE(GridFormChildView, CListView)
BEGIN_MESSAGE_MAP(GridFormChildView, CListView)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_DRAW, &GridFormChildView::OnUserDraw)
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOP_CLEARGRID, &GridFormChildView::OnTopCleargrid)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &GridFormChildView::OnNMCustomdraw)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &GridFormChildView::OnLvnGetdispinfo)
	//ON_WM_PAINT()
	ON_WM_SIZE()
	ON_NOTIFY(HDN_BEGINTRACKA, 0, &GridFormChildView::OnHdnBegintrack)
	ON_NOTIFY(HDN_BEGINTRACKW, 0, &GridFormChildView::OnHdnBegintrack)
END_MESSAGE_MAP()

GridFormChildView::GridFormChildView()
{
	_ListMutex = CreateMutex(NULL, FALSE, NULL);
	_MapMutex = CreateMutex(NULL, FALSE, NULL);
	_ArrayMutex = CreateMutex(NULL, FALSE, NULL);
	pFormThread = NULL;
	_Map.InitHashTable(0x600);
	m_brush.CreateSolidBrush(RGB(150, 150, 150));
}

GridFormChildView::~GridFormChildView()
{
	if (_ListMutex != INVALID_HANDLE_VALUE) {
		CloseHandle(_ListMutex);
		_ListMutex = INVALID_HANDLE_VALUE;
	}
}

void GridFormChildView::OnInitialUpdate()
{
	int idx = 0;
	CListCtrl &list = GetListCtrl();

	CString str;
	str = _T("Courier New");
	LOGFONT lf;
	_pCurFont = GetFont();
	_pCurFont->GetLogFont(&lf);

	lf.lfCharSet = ANSI_CHARSET;
	wcscpy_s(lf.lfFaceName, str);
	font_.CreateFontIndirect(&lf);
	list.SetFont(&font_);

	if (list.GetSafeHwnd() != NULL) {
		list.InsertColumn(idx++, _T("#Msgs"), LVCFMT_LEFT, 70, 0);
		list.InsertColumn(idx++, _T("Ident"), LVCFMT_LEFT, 70, 0);
		list.InsertColumn(idx++, _T("Len"), LVCFMT_LEFT, 50, 0);
		list.InsertColumn(idx++, _T("Data Bytes [7...0]"), LVCFMT_LEFT, 250, 0);
	}
	list.SetExtendedStyle(list.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);
	list.SetItemCountEx(200);
}

void GridFormChildView::DoDataExchange(CDataExchange* pDX)
{
	CListView::DoDataExchange(pDX);
}

BOOL GridFormChildView::Create(LPCTSTR p1, LPCTSTR p2, DWORD p3, const RECT& p4, CWnd* p5, UINT p6, CCreateContext*p7)
{
	return CListView::Create(p1, p2, p3, p4, p5, p6, p7);
}

// GridFormChildView 診斷

#ifdef _DEBUG
void GridFormChildView::AssertValid() const
{
	CListView::AssertValid();
}

#ifndef _WIN32_WCE
void GridFormChildView::Dump(CDumpContext& dc) const
{
	CListView::Dump(dc);
}
#endif
#endif //_DEBUG

void GridFormChildView::CloseAllHnd()
{
//	::SendMessage(m_GridList.m_hWnd, WM_CLOSE, 0, 0);
}

void GridFormChildView::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CListView::OnClose();
}

afx_msg LRESULT GridFormChildView::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	if (GetSafeHwnd() == NULL)
		return 0;
	if (_List.IsEmpty())
		return 0;

	POSITION pos;
	POSITION prePos;
	WPARAM_STRUCT pkt;
	PARAM_STRUCT data;
	CString str;
	int idx = 0;

	LV_ITEM lvi;
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.iSubItem = 0;
	lvi.pszText = LPSTR_TEXTCALLBACK;

	pos = _List.GetHeadPosition();
	idx = 0;

	WaitForSingleObject(_MapMutex, INFINITE);
	while (pos) {
		data = _List.GetAt(pos);
		if (_Map.Lookup(data.Ident, pkt)) {
			pkt.counter++;
			pkt.param = data;
			_Map.SetAt(pkt.param.Ident, pkt);
		} else {
			idx++;
			pkt.counter = 0;
			pkt.param = data;
			WaitForSingleObject(_MapMutex, INFINITE);
			_Map.SetAt(pkt.param.Ident, pkt);
		}
		prePos = pos;
		_List.GetNext(pos);
		WaitForSingleObject(_ListMutex, INFINITE);
		_List.RemoveAt(prePos);
		ReleaseMutex(_ListMutex);
	}

	unsigned int jj;
	WaitForSingleObject(_ArrayMutex, INFINITE);
	_Array.RemoveAll();
	ReleaseMutex(_ArrayMutex);
	pos = _Map.GetStartPosition();
	while (pos) {
		_Map.GetNextAssoc(pos, jj, pkt);
		WaitForSingleObject(_ArrayMutex, INFINITE);
		_Array.Add(pkt);
		ReleaseMutex(_ArrayMutex);
	}
	WPARAM_STRUCT *pD = _Array.GetData();
	qsort(pD, _Array.GetSize(), sizeof(WPARAM_STRUCT), (GENERICCOMPAREFN)Compare);
	CListCtrl &list = GetListCtrl();
	list.GetItemCount();
	list.Invalidate();
	ReleaseMutex(_MapMutex);

	return 0;
}

int __cdecl GridFormChildView::Compare(const WPARAM_STRUCT * p1, const WPARAM_STRUCT * p2)
{
	if (p1->param.Ident > p2->param.Ident)
		return 1;
	else if (p1->param.Ident < p2->param.Ident)
		return -1;
	else
		return 0;
}


BOOL GridFormChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	return FALSE;
	return CWnd::OnEraseBkgnd(pDC);
}


void GridFormChildView::OnContextMenu(CWnd* pWnd, CPoint point)
{

	// TODO: 在此加入您的訊息處理常式程式碼
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(formview_dll.hModule);

	CMenu menu;
	menu.LoadMenu(IDR_VIEW_CONTEXTMENU);

	CMenu* pContextMenu = menu.GetSubMenu(0);

	pContextMenu->TrackPopupMenu (TPM_LEFTALIGN | TPM_LEFTBUTTON |
		TPM_RIGHTBUTTON, point.x, point.y, AfxGetApp()->GetMainWnd());

	AfxSetResourceHandle(hInstOld);
}

void GridFormChildView::OnTopCleargrid()
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(formview_dll.hModule);
	// TODO: 在此加入您的命令處理常式程式碼
	WaitForSingleObject(_MapMutex, INFINITE);
	_Map.RemoveAll();
	ReleaseMutex(_MapMutex);

	WaitForSingleObject(_MapMutex, INFINITE);
	_Array.RemoveAll();
	ReleaseMutex(_ArrayMutex);

	GetListCtrl().Invalidate();
	
	AfxSetResourceHandle(hInstOld);
}


void CGridFormChildFrm::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(formview_dll.hModule);

	CMenu menu;
	menu.LoadMenu(IDR_FRM_CONTEXTMENU);

	CMenu* pContextMenu = menu.GetSubMenu(0);

	pContextMenu->TrackPopupMenu (TPM_LEFTALIGN | TPM_LEFTBUTTON |
		TPM_RIGHTBUTTON, point.x, point.y, (CWnd*)GetParentFrame());
	AfxSetResourceHandle(hInstOld);
	CMDIChildWnd::OnNcRButtonUp(nHitTest, point);
}



//void GridFormChildView::OnSize(UINT nType, int cx, int cy)
//{
//	CFormView::OnSize(nType, cx, cy);
//	CRect rect;
//
//	GetClientRect(&rect);
//	CWnd* pParentFrame = m_GridList.GetParentFrame(); 
//	m_GridList.MoveWindow(0, 0, rect.Width(), rect.Height());
//
//
//	// TODO: 在此加入您的訊息處理常式程式碼
//}


void GridFormChildView::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pNMCD->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	{
		int rowNumber = pNMCD->nmcd.dwItemSpec;
		if ((rowNumber & 0x01)) {
			COLORREF backgroundColor;
			backgroundColor = RGB(219, 220, 175);
			pNMCD->clrTextBk = backgroundColor;
		}
	}
		break;
	default:
		break;
    }
}


void GridFormChildView::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	WPARAM_STRUCT pkt;
	int Ident;
	CString str;

	memset(&pkt, 0x00, sizeof(WPARAM_STRUCT));
	if (pDispInfo->item.mask & LVIF_TEXT) {
		Ident = (unsigned long)pDispInfo->item.iItem;
		if (Ident >= _Array.GetSize()) {
			goto __error;
		}
		pkt = _Array.GetAt(Ident);
		switch (pDispInfo->item.iSubItem) {
		case 0:
			str.Format(_T("%d"), pkt.counter);
			break;
		case 1:
			str.Format(_T("%2X"), pkt.param.Ident);
			break;
		case 2:
			str.Format(_T("%d"), pkt.param.DataLength);
			break;
		case 3:
			str = "";
			for (int j = pkt.param.DataLength - 1; j >= 0; j--)
				str.AppendFormat(_T("%02X:"), pkt.param.RCV_data[j]);
			str.TrimRight(_T(":"));
			break;
		default:
			str = "";
			break;
		}
	}
__error:	
	::lstrcpy(pDispInfo->item.pszText, str);
	*pResult = 0;
}


BOOL GridFormChildView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	cs.style &= ~LVS_TYPEMASK;
	cs.style |= LVS_REPORT;
	cs.style |= LVS_OWNERDATA;

	return CListView::PreCreateWindow(cs);
}


void GridFormChildView::OnSize(UINT nType, int cx, int cy)
{
	CListView::OnSize(nType, cx, cy);
	// TODO: 在此加入您的訊息處理常式程式碼
	CPaintDC dc(this); // device context for painting

	CRect rect;
	CBitmap bitmap;
	CBitmap *pOldBmp;
	CDC	dcMem;

	//fill background color
	GetClientRect(rect);
	dcMem.CreateCompatibleDC(&dc);
	bitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pOldBmp = dcMem.SelectObject(&bitmap);
	//
	dcMem.FillRect(rect, &m_brush);
	//
	GetClientRect(rect);
	dc.BitBlt(rect.left, rect.top, rect.Width(), rect.Height(), &dcMem, rect.left, rect.top, SRCCOPY);
	dcMem.SelectObject(pOldBmp);
}

void GridFormChildView::OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	*pResult = TRUE;
}

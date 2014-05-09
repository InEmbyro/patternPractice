// dllmain.cpp : �w�q DLL ����l�Ʊ`���C
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
	// �p�G�z�ϥ� lpReserved�A�в����o��
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("formview_dll.DLL ���b��l��!\n");
		
		// �X�R DLL ���@����l��
		if (!AfxInitExtensionModule(formview_dll, hInstance))
			return 0;

		// �N�o�� DLL ���J�귽�쵲
		// �`�N: �p�G�o���X�R DLL �O��
		//  MFC �з� DLL (�Ҧp ActiveX ���)�A
		//  �ӫD MFC ���ε{���H���t�覡�s���A
		//  �бq DllMain �����o�@��A�åB�N��
		//  �m�J�q�o���X�R DLL �ҶץX����W�禡���C�ϥγo���X�R DLL ��
		//  �з� DLL ���ӴN�|���T�a�I�s����
		//  �禡�Ӫ�l�Ƴo���X�R DLL�C�_�h�A
		//  CDynLinkLibrary ����N���|���[��
		//  �з� DLL ���귽�쵲�A�P�ɷ|�y���Y����
		//  ���D�C

		//new CDynLinkLibrary(formview_dllDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("formview_dll.DLL ���b�פ�!\n");

		// �I�s�Ѻc�禡�e���פ�{���w
		AfxTermExtensionModule(formview_dll);
	}
	return 1;   // �T�w
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




// CGridFormChildFrm �T���B�z�`��


int CGridFormChildFrm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(formview_dll.hModule);

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1) {
		AfxSetResourceHandle(hInstOld);
		return -1;
	}

	// TODO:  �b���[�J�S�O�إߪ��{���X
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
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	_pView->pFormThread->TerminateThread();
	WaitForSingleObject(_pView->pFormThread->getConfirmHnd(), 5000);
	DeregisterAcquire(rawPos);
	_pView->CloseAllHnd();

	CMDIChildWnd::OnClose();
}

const char* GridFormChildView::mailslot = "\\\\.\\mailslot\\wnc_grid_view";

// GridFormChildView
IMPLEMENT_DYNCREATE(GridFormChildView, CFormView)
BEGIN_MESSAGE_MAP(GridFormChildView, CFormView)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_DRAW, &GridFormChildView::OnUserDraw)
	ON_WM_ERASEBKGND()
	ON_WM_CONTEXTMENU()
	ON_COMMAND(ID_TOP_CLEARGRID, &GridFormChildView::OnTopCleargrid)
//	ON_WM_SIZE()
END_MESSAGE_MAP()

GridFormChildView::GridFormChildView()
	: CFormView(GridFormChildView::IDD)/* , _pThread(NULL) */
{
	_ListMutex = CreateMutex(NULL, FALSE, NULL);
	_MapMutex = CreateMutex(NULL, FALSE, NULL);
	_ArrayMutex = CreateMutex(NULL, FALSE, NULL);
	pFormThread = NULL;
	_Map.InitHashTable(0x600);
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
	if (m_GridList.GetSafeHwnd() != NULL) {
		m_GridList.InsertColumn(idx++, _T("#Msgs"), LVCFMT_LEFT, 70, 0);
		m_GridList.InsertColumn(idx++, _T("Ident"), LVCFMT_LEFT, 70, 0);
		m_GridList.InsertColumn(idx++, _T("Len"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Data Bytes [7...0]"), LVCFMT_LEFT, 250, 0);
	}
	m_GridList._pMap = &_Map;
	m_GridList._pArray = &_Array;
	m_GridList.SetExtendedStyle(m_GridList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER);
	m_GridList.SetItemCount(200);
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

// GridFormChildView �E�_

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
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��

	CFormView::OnClose();
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
	m_GridList.Invalidate();
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
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	return CWnd::OnEraseBkgnd(pDC);
}


void GridFormChildView::OnContextMenu(CWnd* pWnd, CPoint point)
{

	// TODO: �b���[�J�z���T���B�z�`���{���X
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
	// TODO: �b���[�J�z���R�O�B�z�`���{���X
	WaitForSingleObject(_MapMutex, INFINITE);
	_Map.RemoveAll();
	ReleaseMutex(_MapMutex);

	WaitForSingleObject(_MapMutex, INFINITE);
	_Array.RemoveAll();
	ReleaseMutex(_ArrayMutex);

	m_GridList.Invalidate();
	
	AfxSetResourceHandle(hInstOld);
}


void CGridFormChildFrm::OnNcRButtonUp(UINT nHitTest, CPoint point)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
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
//	// TODO: �b���[�J�z���T���B�z�`���{���X
//}

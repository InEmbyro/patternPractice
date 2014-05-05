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
//	ON_WM_DESTROY()
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


//void CGridFormChildFrm::OnDestroy()
//{
//	CMDIChildWnd::OnDestroy();
//
//	// TODO: �b���[�J�z���T���B�z�`���{���X
//}


const char* GridFormChildView::mailslot = "\\\\.\\mailslot\\wnc_grid_view";

// GridFormChildView
IMPLEMENT_DYNCREATE(GridFormChildView, CFormView)
BEGIN_MESSAGE_MAP(GridFormChildView, CFormView)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_DRAW, &GridFormChildView::OnUserDraw)
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()

GridFormChildView::GridFormChildView()
	: CFormView(GridFormChildView::IDD)/* , _pThread(NULL) */
{
	_counter = 0;
	_ListMutex = CreateMutex(NULL, FALSE, NULL);
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
		m_GridList.InsertColumn(idx++, _T("#Msgs"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Ident"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Len"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Data Bytes [7...0]"), LVCFMT_LEFT, 200, 0);
	}
	m_GridList._pMap = &_Map;
	m_GridList.SetExtendedStyle(m_GridList.GetExtendedStyle()|LVS_EX_DOUBLEBUFFER); 
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
	WPARAM_STRUCT pkt;
	PARAM_STRUCT data;
	CString str;
	int idx = 0;

	LV_ITEM lvi;
	lvi.mask = LVIF_PARAM | LVIF_TEXT;
	lvi.iSubItem = 0;
	lvi.pszText = LPSTR_TEXTCALLBACK;

	if (_counter == (sizeof(_counter)-1))
		_counter = 0;

	pos = _List.GetHeadPosition();
	idx = 0;

	while (pos) {
		data = _List.GetAt(pos);
		if (_Map.Lookup(data.Ident, pkt)) {
			pkt.counter++;
			pkt.param = data;
			_Map.SetAt(pkt.param.Ident, pkt);
		} else {
			pkt.counter = 0;
			pkt.param = data;
			_Map.SetAt(pkt.param.Ident, pkt);
			lvi.iItem = idx++;
			lvi.lParam = (LPARAM)pkt.param.Ident;
			m_GridList.InsertItem(&lvi);
		}
		_List.GetNext(pos);
	}
	if (idx)
		m_GridList.SortItems(&CGridList::CompareFunc, 0);
	else
		m_GridList.Invalidate();

	WaitForSingleObject(_ListMutex, INFINITE);
	_List.RemoveAll();
	ReleaseMutex(_ListMutex);
	return 0;
}



BOOL GridFormChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	return CWnd::OnEraseBkgnd(pDC);
}

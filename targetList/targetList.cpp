// dllmain.cpp : �w�q DLL ����l�Ʊ`���C
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
	// �p�G�z�ϥ� lpReserved�A�в����o��
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("targetList.DLL ���b��l��!\n");
		
		// �X�R DLL ���@����l��
		if (!AfxInitExtensionModule(targetListDLL, hInstance))
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

		new CDynLinkLibrary(targetListDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("targetList.DLL ���b�פ�!\n");

		// �I�s�Ѻc�禡�e���פ�{���w
		AfxTermExtensionModule(targetListDLL);
	}
	return 1;   // �T�w
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
// D:\workspace\myDr\targetList\targetList.cpp : ��@��
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


// CTargetList �E�_

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


// CTargetList �T���B�z�`��
// D:\workspace\myDr\targetList\targetList.cpp : ��@��
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


// CTargetListForm �T���B�z�`��

BOOL CTargetList::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: �b���[�J�S�w���{���X�M (��) �I�s�����O

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}


int CTargetListForm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// TODO: �b���[�J�S�w���{���X�M (��) �I�s�����O
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(targetListDLL.hModule);
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1) {
		AfxSetResourceHandle(hInstOld);
		return -1;
	}

	// TODO:  �b���[�J�S�O�إߪ��{���X
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

	// TODO: �b���[�J�z���T���B�z�`���{���X
	if (m_listctrl.m_hWnd)
		m_listctrl.SetWindowPos(NULL, 0, 0, cx, cy,SWP_NOMOVE);
}

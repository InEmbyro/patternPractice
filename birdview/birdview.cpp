// birdview.cpp : �w�q DLL ����l�Ʊ`���C
//
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include "resource.h"
#include "birdview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

static AFX_EXTENSION_MODULE birdviewDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// �p�G�z�ϥ� lpReserved�A�в����o��
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("birdview.DLL ���b��l��!\n");
		
		// �X�R DLL ���@����l��
		if (!AfxInitExtensionModule(birdviewDLL, hInstance))
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

		//new CDynLinkLibrary(birdviewDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("birdview.DLL ���b�פ�!\n");

		// �I�s�Ѻc�禡�e���פ�{���w
		AfxTermExtensionModule(birdviewDLL);
	}
	return 1;   // �T�w
}

extern "C" AFX_EXT_API LPVOID WINAPI InitBirdviewForm()
{
	// create a new CDynLinkLibrary for this app
	#pragma warning(push)
	#pragma warning(disable:6211) //silence prefast warning because CDynLinkLibrary is cleaned up elsewhere
	new CDynLinkLibrary(birdviewDLL);
	#pragma warning(pop)

	CWinApp* pApp = AfxGetApp();
	ENSURE(pApp != NULL);
	CMDIFrameWnd *pFrame = (CMDIFrameWnd*)(pApp->m_pMainWnd);
	return (LPVOID) pFrame->CreateNewChild(RUNTIME_CLASS(CBirdviewFrm), IDR_MENU1, 0, 0);
}


// CBirdviewFrm
IMPLEMENT_DYNCREATE(CBirdviewFrm, CMDIChildWnd)

CBirdviewFrm::CBirdviewFrm()
{

}

CBirdviewFrm::~CBirdviewFrm()
{
}


int CBirdviewFrm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(birdviewDLL.hModule);

	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1) {
		AfxSetResourceHandle(hInstOld);
		return -1;
	}

	// TODO:  �b���[�J�S�O�إߪ��{���X
	_pView = new CBirdviewView();
	CRect rect;
	rect.SetRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy);
	_pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rect, this, AFX_IDW_PANE_FIRST, NULL);
	
	/*	Calling UpdateDate(FALSE) will cause GridFormChildView::DoDataExchange is called.
		In the function, GridFormChildView::m_hWnd will be assigned */
	_pView->UpdateData(FALSE);

	AfxSetResourceHandle(hInstOld);
	return 0;
}


BEGIN_MESSAGE_MAP(CBirdviewFrm, CMDIChildWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CBirdviewFrm �T���B�z�`��


// CBirdviewView

IMPLEMENT_DYNCREATE(CBirdviewView, CView)

CBirdviewView::CBirdviewView()
{

}

CBirdviewView::~CBirdviewView()
{
}

BEGIN_MESSAGE_MAP(CBirdviewView, CView)
END_MESSAGE_MAP()


// CBirdviewView �yø

void CBirdviewView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: �b���[�J�yø�{���X
}


// CBirdviewView �E�_

#ifdef _DEBUG
void CBirdviewView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CBirdviewView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif //_DEBUG


// CBirdviewView �T���B�z�`��

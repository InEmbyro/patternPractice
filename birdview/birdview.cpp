// birdview.cpp : �w�q DLL ����l�Ʊ`���C
//
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include "resource.h"
#include "birdview.h"
#include "CReceiveThread.h"
#include "../canEngine/canEngineApi.h"

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
	return (LPVOID) pFrame->CreateNewChild(RUNTIME_CLASS(CBirdviewFrm), IDR_MENU2, 0, 0);
}


// CBirdviewFrm
IMPLEMENT_DYNCREATE(CBirdviewFrm, CMDIChildWnd)

CBirdviewFrm::CBirdviewFrm()
	:pRcvThread(NULL)
{
	pRcvThread = new CReceiveThread();
}

CBirdviewFrm::~CBirdviewFrm()
{
	delete pRcvThread;
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

	if (!pRcvThread) {
		AfxMessageBox(_T("!pRcvThread"));
		return -1;
	}
	
	CString name;
	POSITION pos;

	name = CBirdviewView::mailslot;
	pos = RegisterAcquire(name, CBirdviewView::slotKey);
	if (pos == NULL) {
		AfxMessageBox(_T("GridFormChildView::mailslot"));
		return -1;
	}

	pRcvThread->_pView = _pView;
	pRcvThread->SetInfoHandle(InforEventAcquire(pos));
	pRcvThread->SetMailHandle(MailSlotAcquire(pos));
	pRcvThread->SetDecMailslotName(GetDecMailslotName());
	rawPos = pos;

	if (!pRcvThread->InitThread()) {
		AfxMessageBox(_T("_pGridFormThread->InitThread"));
		DeregisterAcquire(rawPos, CBirdviewView::slotKey);
	}
	_pView->pRcvThread = pRcvThread;

	UpdateData(FALSE);

	AfxSetResourceHandle(hInstOld);
	return 0;
}


BEGIN_MESSAGE_MAP(CBirdviewFrm, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CBirdviewFrm �T���B�z�`��

unsigned long rcvIdent[] = {
	0x401, 0x402, 
	0x610, 0x611, 0x612, 0x613, 0x614, 0x615, 0x616, 0x617, 0x618, 0x619, 0x620,
	0x00};

// CBirdviewView

IMPLEMENT_DYNCREATE(CBirdviewView, CView)
const char* CBirdviewView::mailslot = "\\\\.\\mailslot\\wnc_bird_view";
const unsigned int CBirdviewView::slotKey = 0x01;

CBirdviewView::CBirdviewView()
	:pRcvThread(NULL)
{
	_ListMutex = CreateMutex(NULL, FALSE, NULL);
	_ReceiveMap.InitHashTable(100);

	int idx = 0;
	unsigned long pkt;
	do {
		_ReceiveMap.SetAt(rcvIdent[idx++], pkt);
	} while (rcvIdent[idx] != 0x00);
}

CBirdviewView::~CBirdviewView()
{
}

BEGIN_MESSAGE_MAP(CBirdviewView, CView)
	ON_MESSAGE(WM_USER_DRAW, &CBirdviewView::OnUserDraw)
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


afx_msg LRESULT CBirdviewView::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	PARAM_STRUCT data;

	if (GetSafeHwnd() == NULL)
		return 0;
	if (_List.IsEmpty())
		return 0;

	POSITION pos;
	pos = _List.GetHeadPosition();
	while (pos) {
		data = _List.GetHead();
		WaitForSingleObject(_ListMutex, INFINITE);
		_List.RemoveHead();
		ReleaseMutex(_ListMutex);
		pos = _List.GetHeadPosition();
	}
	return 0;
}


void CBirdviewFrm::OnClose()
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	_pView->pRcvThread->TerminateThread();
	WaitForSingleObject(_pView->pRcvThread->getConfirmHnd(), 5000);
	DeregisterAcquire(rawPos, CBirdviewView::slotKey);

	CMDIChildWnd::OnClose();
}

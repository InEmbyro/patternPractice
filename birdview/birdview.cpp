// birdview.cpp : 定義 DLL 的初始化常式。
//
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include <math.h>
#include "resource.h"
#include "birdview.h"
#include "CReceiveThread.h"
#include "../canEngine/canEngineApi.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define CAR_WIDTH	20	//2 meter
#define CAR_LENGTH	45	//4.5m
#define Y_OFFSET	10

static AFX_EXTENSION_MODULE birdviewDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果您使用 lpReserved，請移除這個
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("birdview.DLL 正在初始化!\n");
		
		// 擴充 DLL 的一次初始化
		if (!AfxInitExtensionModule(birdviewDLL, hInstance))
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

		//new CDynLinkLibrary(birdviewDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("birdview.DLL 正在終止!\n");

		// 呼叫解構函式前先終止程式庫
		AfxTermExtensionModule(birdviewDLL);
	}
	return 1;   // 確定
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

	// TODO:  在此加入特別建立的程式碼
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
	rawPos = pos;

	if (!pRcvThread->InitThread()) {
		AfxMessageBox(_T("pRcvThread->InitThread"));
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


// CBirdviewFrm 訊息處理常式

unsigned long rcvIdent[] = {
	0x401, 0x402, 
	0x610, 0x611, 0x612, 0x613, 0x614, 0x615, 0x616, 0x617, 0x618, 0x619, 0x620,
	0x00};

// CBirdviewView

IMPLEMENT_DYNCREATE(CBirdviewView, CView)
const char* CBirdviewView::mailslot = "\\\\.\\mailslot\\wnc_bird_view";
const unsigned int CBirdviewView::slotKey = 0x01;

CBirdviewView::CBirdviewView()
	:pRcvThread(NULL), count(0)
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
//ON_WM_PAINT()
END_MESSAGE_MAP()

// CBirdviewView 診斷

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


// CBirdviewView 訊息處理常式


afx_msg LRESULT CBirdviewView::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	PARAM_STRUCT data;
	CPoint pnt;
	float range;
	int temp;
	float theta;

	if (GetSafeHwnd() == NULL)
		return 0;
	if (_List.IsEmpty())
		return 0;

	/* Create a compatible dc & bmp to avoid flickering */
	CClientDC dc(this);	/* acquire device context */
	CRect rect;
	GetClientRect(&rect);

	/* Prepare Pen for drawing object */
	CPen pen, *pOldPen;
	pen.CreatePen(PS_SOLID, 3, RGB(0, 200, 0));
	CBrush brush, *pOldBrush;
	brush.CreateSolidBrush(RGB(0, 200, 0));

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	CBitmap bmp;
	CBitmap *pOldBmp = NULL;
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pOldBmp = dcMem.SelectObject(&bmp);
	DrawCarLine(&dcMem);

	POSITION pos;
	pos = _List.GetHeadPosition();
	/* Assign pen, brush etc.. */
	pOldPen = dcMem.SelectObject(&pen);
	pOldBrush = dcMem.SelectObject(&brush);

	SetOrigin(&dcMem, TRUE, 0, Y_OFFSET + CAR_LENGTH);
	while (pos) {
		data = _List.GetHead();
		WaitForSingleObject(_ListMutex, INFINITE);
		/* Only try raw first */
		if (data.Ident == 0x401 && data.DataLength == 8) {
			temp = data.RCV_data[4] & 0x01;
			temp = (temp << 8) + data.RCV_data[3];
			temp = (temp << 7) + ((data.RCV_data[2] & 0xFE) >> 1);
			range = temp * 0.01;
			temp = data.RCV_data[2] & 0x01;
			temp = (temp << 8) + data.RCV_data[1];
			temp = (temp << 2) + ((data.RCV_data[0] & 0xC0) >> 6);
			theta = (temp - 1024) * 0.16;
#define PI	3.1416
			pnt.x = range * sin(theta * PI / 180) * 10;
			pnt.y = range * cos(theta * PI / 180) * 10;
#define OBJECT_SIZE	5
			dcMem.Ellipse(pnt.x, pnt.y, pnt.x + OBJECT_SIZE, pnt.y + OBJECT_SIZE);
		}
		_List.RemoveHead();
		ReleaseMutex(_ListMutex);
		pos = _List.GetHeadPosition();
	}
	SetOrigin(&dcMem, FALSE);
	dcMem.SelectObject(pOldPen);
	dcMem.SelectObject(pOldBrush);
	brush.DeleteObject();
	pen.DeleteObject();

	//
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBmp);
	bmp.DeleteObject();
	dcMem.DeleteDC();

	return 0;
}

void CBirdviewView::SetOrigin(CDC *pDc, BOOL action, int x_off, int y_off)
{
	if (action == TRUE) {
		CRect rect;
		GetClientRect(&rect);
		pDc->SetViewportOrg((rect.Width() / 2) + x_off, y_off);
	} else {
		pDc->SetViewportOrg(0, 0);
	}
}

void CBirdviewView::DrawCarLine(CDC *pDc)
{
	/* Set bk mode to TRANSAPRENT and draw bk color */
	CRect rect;
	GetClientRect(&rect);
	pDc->SetBkMode(TRANSPARENT);
	pDc->FillSolidRect(rect, RGB(0, 0, 0));

	/* Set the origin point to the center of the client area */
	SetOrigin(pDc, TRUE, 0, Y_OFFSET);

	/* draw horizatal & vertical line for reference */
	CPen pen;
	pen.CreatePen(PS_SOLID, 1, RGB(192, 192, 192));
	CPen *pOldPen = pDc->SelectObject(&pen);

	pDc->MoveTo(-rect.Width() / 2, 0);
	pDc->LineTo(rect.Width() / 2, 0);
	pDc->MoveTo(0, -Y_OFFSET);
	pDc->LineTo(0, rect.Height() - Y_OFFSET);
	pDc->SelectObject(pOldPen);
	pen.DeleteObject();

	/* Draw Car */
	pDc->FillSolidRect(-(CAR_WIDTH / 2), 0, CAR_WIDTH, CAR_LENGTH, RGB(222, 0, 0));
	SetOrigin(pDc, FALSE);
	return;
}

void CBirdviewFrm::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	_pView->pRcvThread->TerminateThread();
	WaitForSingleObject(_pView->pRcvThread->getConfirmHnd(), 5000);
	DeregisterAcquire(rawPos, CBirdviewView::slotKey);

	CMDIChildWnd::OnClose();
}


void CBirdviewView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: 在此加入您的訊息處理常式程式碼
	// 不要呼叫圖片訊息的 CView::OnPaint()
}


void CBirdviewView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
}

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

#define PI	3.1416
#define RAD_CONVER	(PI / 180)
#define SCALE	20
#define OBJECT_SIZE	10

#define CAR_WIDTH	(2 * SCALE)		//2 meter
#define CAR_LENGTH	(4.5 * SCALE)	//4.5m
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
	/*0x400, 0x401, 0x402, 0x00,*/
	0x410, 0x411, 0x00,
	/* 0x601, 0x610, 0x611, 0x612, 0x613, 0x614, 0x615, 0x616, 0x617, 0x618, 0x619, 0x620, 0x00*/};

// CBirdviewView

IMPLEMENT_DYNCREATE(CBirdviewView, CView)
const char* CBirdviewView::mailslot = "\\\\.\\mailslot\\wnc_bird_view";
const unsigned int CBirdviewView::slotKey = 0x01;

CBirdviewView::CBirdviewView()
	:pRcvThread(NULL)
{
	_ListMutex = CreateMutex(NULL, FALSE, NULL);

	int idx = 0;
	unsigned long pkt = 0;
	CList <PARAM_STRUCT, PARAM_STRUCT&> *p;
	HANDLE h;
	ARRAY_IDX array_idx;

	array_idx.rowIdx = 0;
	_FilterMap.InitHashTable(sizeof(rcvIdent)/sizeof(rcvIdent[0]));
	for (idx = 0; idx < (sizeof(rcvIdent)/sizeof(rcvIdent[0])); idx++) {
		if (rcvIdent[idx] == 0x00) {
			p = new CList <PARAM_STRUCT, PARAM_STRUCT&>;
			if (p == NULL) {
				AfxMessageBox(_T("p = NULL"));
				return;
			}
			_ListStoreArray.Add(p);
			_ListArray.Add(NULL);
			h = CreateMutex(NULL, FALSE, NULL);
			if (h == NULL) {
				_ListStoreArray.RemoveAt(_ListArray.GetSize() - 1);
				delete p;
				AfxMessageBox(_T("Create _ListStoreArray is failure"));
				continue;
			} else {
				_ListStoreArrayMutex.Add(h);
				h = CreateMutex(NULL, FALSE, NULL);
				if (h)
					_ListArrayMutex.Add(h);
				else {
					AfxMessageBox(_T("Create _ListArrayMutex is failure"));
					_ListArrayMutex.RemoveAt(_ListArrayMutex.GetSize() - 1);
					_ListStoreArray.RemoveAt(_ListArray.GetSize() - 1);
					delete p;
					continue;
				}
			}
			pkt++;
			array_idx.rowIdx++;
			continue;
		}
		array_idx.canId = rcvIdent[idx];
		_FilterMap.SetAt((unsigned long)rcvIdent[idx], array_idx);
	}
}

CBirdviewView::~CBirdviewView()
{
	CList <PARAM_STRUCT, PARAM_STRUCT&> *p;

	for (int idx = 0; idx < _ListArray.GetSize(); idx++) {
		p = _ListArray.GetAt(idx);
		if (p)
			delete p;
		p = _ListStoreArray.GetAt(idx);
		if (p)
			delete p;
		CloseHandle(_ListArrayMutex.GetAt(idx));
		CloseHandle(_ListStoreArrayMutex.GetAt(idx));
	}
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
	RAW_OBJECT_STRUCT raw;
	CPoint pnt;
	CString str;
	double temp;
	int angle;
	unsigned long fakeKey;
	CList <PARAM_STRUCT, PARAM_STRUCT&>* p;

	if (GetSafeHwnd() == NULL)
		return 0;

	/* Create a compatible dc & bmp to avoid flickering */
	CClientDC dc(this);	/* acquire device context */
	CRect rect;
	GetClientRect(&rect);

	CDC dcMem;
	dcMem.CreateCompatibleDC(&dc);

	/* Prepare Pen for drawing object */
	CPen pen, *pOldPen;
	CBrush	*pOldBrush = NULL;
	CBrush	*pBrush = CBrush::FromHandle((HBRUSH)GetStockObject(NULL_BRUSH));
	CFont font;
	CFont *pOldFont;
	font.CreatePointFont(90, _T("Arial"));
	pOldFont = dcMem.SelectObject(&font);
	dcMem.SetTextColor(RGB(255, 255, 255));

	CBitmap bmp;
	CBitmap *pOldBmp = NULL;
	bmp.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());
	pOldBmp = dcMem.SelectObject(&bmp);
	pOldBrush = dcMem.SelectObject(pBrush);
	DrawCarLine(&dcMem);

	POSITION pos;
	//pos = _List.GetHeadPosition();
	/* Assign pen, brush etc.. */
	pOldPen = dcMem.SelectObject(&pen);

	SetOrigin(&dcMem, TRUE, 0, Y_OFFSET + CAR_LENGTH);

	int arrIdx = 0;
	for (arrIdx = 0; arrIdx < _ListArray.GetSize(); arrIdx++) {
		WaitForSingleObject(_ListArrayMutex.GetAt(arrIdx), INFINITE);
		p = _ListArray.GetAt(arrIdx);
		pos = p->GetHeadPosition();
		while (pos) {
			data = p->GetAt(pos);
			p->RemoveHead();
			if (data.Ident == 0x412 || data.Ident == 0x410) {
				pos = p->GetHeadPosition();
				continue;
			}
			ParseRawObject(&data, &raw);
			if (data.Ident == 0x401) {
				angle = 57;
				if (pen.m_hObject)
					pen.DeleteObject();
				pen.CreatePen(PS_SOLID, 3, RGB(0, 200, 0));
				pOldPen = dcMem.SelectObject(&pen);
			} else {
				angle = -57;
				if (pen.m_hObject)
					pen.DeleteObject();
				pen.CreatePen(PS_SOLID, 3, RGB(200, 0, 0));
				pOldPen = dcMem.SelectObject(&pen);
			}
			temp = (raw.x_range * cos(angle * RAD_CONVER)) - (raw.y_range * sin(angle * RAD_CONVER));
			pnt.x = (temp * SCALE);
			temp = (raw.x_range * sin(angle * RAD_CONVER)) + (raw.y_range * cos(angle * RAD_CONVER));
			pnt.y = (temp *SCALE);
			rect.left = pnt.x;
			rect.right = pnt.x + OBJECT_SIZE;
			rect.top = pnt.y;
			rect.bottom = pnt.y + OBJECT_SIZE;
			dcMem.Ellipse(rect);
			dcMem.SelectObject(pOldPen);
			rect.OffsetRect(OBJECT_SIZE, -OBJECT_SIZE);
			str.Format(_T("%d"), raw.TargetNum);
			CSize sz = dcMem.GetTextExtent(str);
			rect.right = rect.left + sz.cx;
			rect.bottom = rect.top + sz.cy;
			dcMem.DrawText(str, rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
			pos = p->GetHeadPosition();
		}
		ReleaseMutex(_ListArrayMutex.GetAt(arrIdx));
	}

	SetOrigin(&dcMem, FALSE);
	dcMem.SelectObject(pOldPen);
	dcMem.SelectObject(pOldBrush);
	if (pen.m_hObject)
		pen.DeleteObject();

	//
	GetClientRect(&rect);
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &dcMem, 0, 0, SRCCOPY);
	dcMem.SelectObject(pOldBmp);
	if (bmp.m_hObject)
		bmp.DeleteObject();
	dcMem.DeleteDC();

	return 0;
}

void CBirdviewView::ParseRawObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
{
	if (pSrc == NULL || pRaw == NULL)
		return;

	int temp;

	pRaw->TargetNum = pSrc->RCV_data[0] & 0x3F;

	temp = pSrc->RCV_data[2] & 0x01;
	temp = (temp << 8) + pSrc->RCV_data[1];
	temp = (temp << 2) + ((pSrc->RCV_data[0] & 0xC0) >> 6);
	pRaw->angle = (temp - 1024) * 0.16;

	temp = pSrc->RCV_data[4] & 0x01;
	temp = (temp << 8) + pSrc->RCV_data[3];
	temp = (temp << 7) + ((pSrc->RCV_data[2] & 0xFE) >> 1);
	pRaw->range = temp * 0.01;

	temp = pSrc->RCV_data[5] & 0x03;
	temp = (temp << 7) + ((pSrc->RCV_data[4] & 0xFE) >> 1);
	pRaw->AbsLevel_db = temp * 0.32;

	pRaw->type = (pSrc->RCV_data[5] & 0x7C) >> 2;

	temp = pSrc->RCV_data[7];
	temp = (temp << 6) + ((pSrc->RCV_data[4] & 0xFC) >> 2);
	pRaw->relatedSpeed = (temp - 8192) * 0.02;

	pRaw->x_range = pRaw->range * sin(pRaw->angle * RAD_CONVER);
	pRaw->y_range = pRaw->range * cos(pRaw->angle * RAD_CONVER);
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

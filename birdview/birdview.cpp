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

#define PI	3.1415926
#define RAD_CONVER	(PI / 180)
#define SCALE	10
#define OBJECT_SIZE		10

#define CAR_WIDTH	(2 * SCALE)		//2 meter
#define CAR_LENGTH	(4.5 * SCALE)	//4.5m
#define Y_OFFSET	10
#define SENSOR_ROTATE	(57.0f)
#define DASHED_STEP		(3.0f)
#define FAN_LEN			(3.5f * 1.5)

static AFX_EXTENSION_MODULE birdviewDLL = { NULL, NULL };
float CBirdviewView::halfCarWidth = 1.0f;
float CBirdviewView::halfCarLen = 2.0f;
float CBirdviewView::halfCarHeight = 1.0f;
float CBirdviewView::halfRoadWidth = (3.5f / 2);
float CBirdviewView::m_Speed = 0;

unsigned char threeto8[8] =
{
	0, 0111>>1, 0222>>1, 0333>>1, 0444>>1, 0555>>1, 0666>>1, 0377
};


unsigned char twoto8[4] =
{
	0, 0x55, 0xaa, 0xff
};

unsigned char oneto8[2] =
{
	0, 255
};

static int defaultOverride[13] =
{
	0, 3, 24, 27, 64, 67, 88, 173, 181, 236, 247, 164, 91
};

static PALETTEENTRY defaultPalEntry[20] =
{
	{ 0,   0,   0,    0 },
	{ 0x80,0,   0,    0 },
	{ 0,   0x80,0,    0 },
	{ 0x80,0x80,0,    0 },
	{ 0,   0,   0x80, 0 },
	{ 0x80,0,   0x80, 0 },
	{ 0,   0x80,0x80, 0 },
	{ 0xC0,0xC0,0xC0, 0 },

	{ 192, 220, 192,  0 },
	{ 166, 202, 240,  0 },
	{ 255, 251, 240,  0 },
	{ 160, 160, 164,  0 },

	{ 0x80,0x80,0x80, 0 },
	{ 0xFF,0,   0,    0 },
	{ 0,   0xFF,0,    0 },
	{ 0xFF,0xFF,0,    0 },
	{ 0,   0,   0xFF, 0 },
	{ 0xFF,0,   0xFF, 0 },
	{ 0,   0xFF,0xFF, 0 },
	{ 0xFF,0xFF,0xFF, 0 }
};


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
	_pView->Init();
	//_pView->SetTimer(0, 100, NULL);
	
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
		AfxSetResourceHandle(hInstOld);
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
//	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CBirdviewFrm 訊息處理常式

unsigned long rcvIdent[] = {
	0x400, 0x401, 0x402, 0x00,
	0x410, 0x411, 0x412, 0x00,
	0x601, 0x610, 0x611, 0x612, 0x613, 0x614, 0x615, 0x616, 0x617, 0x618, 0x619, 0x620, 0x00};

// CBirdviewView

IMPLEMENT_DYNCREATE(CBirdviewView, CView)
const char* CBirdviewView::mailslot = "\\\\.\\mailslot\\wnc_bird_view";
const unsigned int CBirdviewView::slotKey = 0x01;

CBirdviewView::CBirdviewView()
	:pRcvThread(NULL), m_RoadLineStartZ(0), m_RoadLineStartZStep(0), m_fYoffset(10.28)
{
	m_pDC = NULL;
	m_pOldPalette = NULL;

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
ON_WM_ERASEBKGND()
ON_WM_TIMER()
ON_WM_DESTROY()
ON_WM_SIZE()
ON_MESSAGE(WM_UPDATE_SPEED_DRAWING, &CBirdviewView::OnUpdateSpeedDrawing)
ON_WM_KEYDOWN()
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


void CBirdviewView::ParseTrackingObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
{
	if (pSrc == NULL || pRaw == NULL)
		return;

	int temp;

	pRaw->angle = 0;
	pRaw->range = 0;
	pRaw->AbsLevel_db = 0;
	pRaw->type = 0;

	pRaw->TargetNum = (pSrc->RCV_data[7] & 0xFC) >> 2;
	
	temp = pSrc->RCV_data[1] & 0x1F;
	temp = (temp << 8) + pSrc->RCV_data[0];
	pRaw->x_range = (temp - 7500) * 0.016;

	temp = pSrc->RCV_data[3] & 0x01;
	temp = (temp << 8) + pSrc->RCV_data[2];
	temp = (temp << 3) + ((pSrc->RCV_data[1] & 0xE0) >> 5);
	pRaw->y_range = (temp - 2048) * 0.016;
}

void CBirdviewView::DrawTrackingObject3D(PARAM_STRUCT* pD)
{
	RAW_OBJECT_STRUCT raw;
	float tempX;
	float tempZ;

	ParseTrackingObject(pD, &raw);
	tempZ = -raw.x_range;
	tempX = -raw.y_range;

	glColor3f(0.0f, 1.0f, 0.0f);

	//front
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, 0.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX + halfCarWidth, 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX + halfCarWidth, 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	//back
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX - halfCarWidth, 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 0.0f, tempZ - halfCarLen);
	glEnd();
	glPopMatrix();

	//right
	glPushMatrix();
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX + halfCarWidth, 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX + halfCarWidth, 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	//left
	glPushMatrix();
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX - halfCarWidth, 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX - halfCarWidth, 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	//top
	glPushMatrix();
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 1.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.8f, 0.8f, 0.8f);
	glRasterPos3f(tempX - halfCarWidth, 1.0f, tempZ + halfCarLen);
	sprintf(quote, "%d", raw.TargetNum);
	glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote);
	glPopMatrix();

	//bottom
	glPushMatrix();
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, 0.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

}

void CBirdviewView::DrawTrackingObject(PARAM_STRUCT *pD, CDC *pDc)
{
	RAW_OBJECT_STRUCT raw;
	CPen pen;
	CPen *pOldPen;
	CPoint pnt;
	CRect rect;
	CString str;
	CSize sz;

	if (pen.m_hObject)
		pen.DeleteObject();
	ParseTrackingObject(pD, &raw);
	pen.CreatePen(PS_SOLID, 3, RGB(0, 200, 0));
	pOldPen = pDc->SelectObject(&pen);

	pnt.y = raw.x_range * SCALE;
	pnt.y = -pnt.y;
	pnt.x = raw.y_range * SCALE;
	pnt.x = -pnt.x;

	rect.left = pnt.x - (CAR_WIDTH / 2);
	rect.right = pnt.x + (CAR_WIDTH / 2);
	rect.top = pnt.y;
	rect.bottom = pnt.y + (CAR_LENGTH);
	pDc->FillSolidRect(rect, RGB(0, 200, 0));
	pDc->SelectObject(pOldPen);
	rect.OffsetRect((CAR_WIDTH / 2), -(CAR_WIDTH / 2));
	str.Format(_T("%d"), raw.TargetNum);
	sz = pDc->GetTextExtent(str);
	rect.right = rect.left + sz.cx;
	rect.bottom = rect.top + sz.cy;
	pDc->DrawText(str, rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}
// CBirdviewView 訊息處理常式

void CBirdviewView::DrawRawObject3D(RAW_OBJECT_STRUCT *pRaw, unsigned char sensorNo)
{
	int angle;
	float temp;
	float z;
	float x;
	float y;

	if (sensorNo == 0)
		angle = SENSOR_ROTATE;
	else
		angle = -SENSOR_ROTATE;

	temp = (pRaw->x_range * cos(angle * RAD_CONVER)) - (pRaw->y_range * sin(angle * RAD_CONVER));
	x = temp;
	temp = (pRaw->x_range * sin(angle * RAD_CONVER)) + (pRaw->y_range * cos(angle * RAD_CONVER));
	temp += halfCarLen;
	y = temp;	//it is Z
	z = pRaw->z_range;

	glPushMatrix();
	switch (sensorNo) {
	case 0:
		if (z != 0.0f)
			glColor3f(1.0f, 0.0f, 1.0f);
		else
			glColor3f(1.0f, 0.0f, 0.0f);
		break;
	case 1:
		if (z != 0.0f)
			glColor3f(1.0f, 1.0f, 1.0f);
		else
			glColor3f(1.0f, 1.0f, 0.0f);
		break;
	default:
		break;
	}

	glTranslatef(0.0f, 0.0f, halfCarLen);
	//front
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(x - 0.5f,	z + 1.0f,	y + 0.5f);
	glVertex3f(x - 0.5f,	z + 0.0f,	y + 0.5f);
	glVertex3f(x + 0.5f,	z + 0.0f,	y + 0.5f);
	glVertex3f(x + 0.5f,	z + 1.0f,	y + 0.5f);
	glEnd();

	//back
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(x - 0.5f,	z + 1.0f,	y - 0.5f);
	glVertex3f(x - 0.5f,	z + 0.0f,	y - 0.5f);
	glVertex3f(x + 0.5f,	z + 0.0f,	y - 0.5f);
	glVertex3f(x + 0.5f,	z + 1.0f,	y - 0.5f);
	glEnd();

	//right
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(x + 0.5f,	z + 1.0f,	y + 0.5f);
	glVertex3f(x + 0.5f,	z + 0.0f,	y + 0.5f);
	glVertex3f(x + 0.5f,	z + 0.0f,	y - 0.5f);
	glVertex3f(x + 0.5f,	z + 1.0f,	y - 0.5f);
	glEnd();

	//left
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(x - 0.5f,	z + 1.0f,	y + 0.5f);
	glVertex3f(x - 0.5f,	z + 0.0f,	y + 0.5f);
	glVertex3f(x - 0.5f,	z + 0.0f,	y - 0.5f);
	glVertex3f(x - 0.5f,	z + 1.0f,	y - 0.5f);
	glEnd();

	//top
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(x + 0.5f,	z + 1.0f,	y + 0.5f);
	glVertex3f(x + 0.5f,	z + 1.0f,	y - 0.5f);
	glVertex3f(x - 0.5f,	z + 1.0f,	y - 0.5f);
	glVertex3f(x - 0.5f,	z + 1.0f,	y + 0.5f);
	glEnd();

	glPushMatrix();
	glColor3f(0.8f, 0.8f, 0.8f);
	glRasterPos3f(x, z + 1.0f, y);
	sprintf(quote, "%d", pRaw->TargetNum);
	glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote);
	glPopMatrix();
	
	//bottom
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(x + 0.5f,	z + 0.0f,	y + 0.5f);
	glVertex3f(x + 0.5f,	z + 0.0f,	y - 0.5f);
	glVertex3f(x - 0.5f,	z + 0.0f,	y - 0.5f);
	glVertex3f(x - 0.5f,	z + 0.0f,	y + 0.5f);
	glEnd();

	//
	glPopMatrix();
}

void CBirdviewView::DrawRawObject3D(PARAM_STRUCT *pD)
{
	int angle;
	float temp;
	RAW_OBJECT_STRUCT raw;
	CPoint pnt;

	ParseRawObject(pD, &raw);
	if (pD->Ident == 0x401) {
		angle = SENSOR_ROTATE;
	} else if (pD->Ident == 0x411) {
		angle = -SENSOR_ROTATE;
	}
	temp = (raw.x_range * cos(angle * RAD_CONVER)) - (raw.y_range * sin(angle * RAD_CONVER));
	pnt.x = temp;
	temp = (raw.x_range * sin(angle * RAD_CONVER)) + (raw.y_range * cos(angle * RAD_CONVER));
	temp += halfCarLen;
	pnt.y = temp;	//it is Z

	glPushMatrix();
	if (pD->Ident == 0x401) {
		glColor3f(1.0f, 0.0f, 0.0f);
	} else {
		glColor3f(1.0f, 1.0f, 0.0f);
	}
	glTranslatef(0.0f, 0.0f, halfCarLen);
	//front
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(pnt.x - 0.5f,	1.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x - 0.5f,	0.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x + 0.5f,	0.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x + 0.5f,	1.0f,	pnt.y + 0.5f);
	glEnd();

	//back
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(pnt.x - 0.5f,	1.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x - 0.5f,	0.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x + 0.5f,	0.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x + 0.5f,	1.0f,	pnt.y - 0.5f);
	glEnd();

	//right
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(pnt.x + 0.5f,	1.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x + 0.5f,	0.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x + 0.5f,	0.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x + 0.5f,	1.0f,	pnt.y - 0.5f);
	glEnd();

	//left
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(pnt.x - 0.5f,	1.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x - 0.5f,	0.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x - 0.5f,	0.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x - 0.5f,	1.0f,	pnt.y - 0.5f);
	glEnd();

	//top
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(pnt.x + 0.5f,	1.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x + 0.5f,	1.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x - 0.5f,	1.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x - 0.5f,	1.0f,	pnt.y + 0.5f);
	glEnd();

	glPushMatrix();
	glColor3f(0.8f, 0.8f, 0.8f);
	glRasterPos3f(pnt.x, 1.0f, pnt.y);
	sprintf(quote, "%d", raw.TargetNum);
	glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote);
	glPopMatrix();
	
	//bottom
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(pnt.x + 0.5f,	0.0f,	pnt.y + 0.5f);
	glVertex3f(pnt.x + 0.5f,	0.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x - 0.5f,	0.0f,	pnt.y - 0.5f);
	glVertex3f(pnt.x - 0.5f,	0.0f,	pnt.y + 0.5f);
	glEnd();


	//
	glPopMatrix();
}

void CBirdviewView::DrawRawObject(PARAM_STRUCT *pD, CDC *pDc)
{
	RAW_OBJECT_STRUCT raw;
	int angle;
	CPen pen;
	CPen *pOldPen;
	CPoint pnt;
	double temp;
	CRect rect;
	CString str;
	CSize sz;

	if (pen.m_hObject)
		pen.DeleteObject();
	ParseRawObject(pD, &raw);
	if (pD->Ident == 0x401) {
		angle = 57;
		pen.CreatePen(PS_SOLID, 3, RGB(200, 0, 0));
	} else if (pD->Ident == 0x411) {
		angle = -57;
		pen.CreatePen(PS_SOLID, 3, RGB(200, 200, 0));
	}
	pOldPen = pDc->SelectObject(&pen);

	temp = (raw.x_range * cos(angle * RAD_CONVER)) - (raw.y_range * sin(angle * RAD_CONVER));
	pnt.x = (temp * SCALE);
	temp = (raw.x_range * sin(angle * RAD_CONVER)) + (raw.y_range * cos(angle * RAD_CONVER));
	pnt.y = (temp *SCALE);

	rect.left = pnt.x;
	rect.right = pnt.x + OBJECT_SIZE;
	rect.top = pnt.y;
	rect.bottom = pnt.y + OBJECT_SIZE;
	pDc->Ellipse(rect);
	pDc->SelectObject(pOldPen);
	rect.OffsetRect(OBJECT_SIZE, -OBJECT_SIZE);
	str.Format(_T("%d"), raw.TargetNum);
	sz = pDc->GetTextExtent(str);
	rect.right = rect.left + sz.cx;
	rect.bottom = rect.top + sz.cy;
	pDc->DrawText(str, rect, DT_SINGLELINE | DT_LEFT | DT_VCENTER);
}

void CBirdviewView::DrawFarTarget()
{
	CPoint pnt;

	pnt.x = halfCarWidth * 3;
	pnt.y = -200;

	glPushMatrix();
	glColor3f(0.0f, 1.0f, 0.0f);
	//front
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(pnt.x - halfCarWidth,	2.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	0.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	0.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	2.0f,	pnt.y + halfCarLen);
	glEnd();

	//back
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(pnt.x - halfCarWidth,	2.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	0.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	0.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	2.0f,	pnt.y - halfCarLen);
	glEnd();

	//right
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(pnt.x + halfCarWidth,	2.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	0.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	0.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	2.0f,	pnt.y - halfCarLen);
	glEnd();

	//left
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(pnt.x - halfCarWidth,	2.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	0.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	0.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	2.0f,	pnt.y - halfCarLen);
	glEnd();

	//top
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(pnt.x + halfCarWidth,	2.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	2.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	2.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	2.0f,	pnt.y + halfCarLen);
	glEnd();

	//bottom
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(pnt.x + halfCarWidth,	0.0f,	pnt.y + halfCarLen);
	glVertex3f(pnt.x + halfCarWidth,	0.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	0.0f,	pnt.y - halfCarLen);
	glVertex3f(pnt.x - halfCarWidth,	0.0f,	pnt.y + halfCarLen);
	glEnd();
	//

	glPopMatrix();
}

void CBirdviewView::DrawText()
{
	float dis = 0.0f;

	glPushMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);

	for (dis = -20.0; dis <= 20.0f; dis += 5.0f) {
		glRasterPos3f(halfRoadWidth * 7, 0.0f, dis);
		sprintf(quote, "%.1fm", -dis);
		glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote); 
	}

	for (dis = -20.0; dis >= -100.0f; dis -= 10.0f) {
		glRasterPos3f(halfRoadWidth * 7, 0.0f, dis);
		sprintf(quote, "%.1fm", -dis);
		glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote); 
		glRasterPos3f(halfRoadWidth * 7, 0.0f, -dis);
		sprintf(quote, "%.1fm", dis);
		glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote); 
	}

	for (dis = -100.0f; dis >= -500.0f; dis -= 50.0f) {
		glRasterPos3f(halfRoadWidth * 7, 0.0f, dis);
		sprintf(quote, "%.1fm", -dis);
		glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote); 

		glRasterPos3f(halfRoadWidth * 7, 0.0f, -dis);
		sprintf(quote, "%.1fm", dis);
		glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote); 
	}

#if 0
	glRasterPos3f(-30.0f, 3.0f, 0.0f);
	sprintf(quote, "%.2fkm/hr,", m_Speed);
	glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote);		
	sprintf(quote, "%.2f,", m_fFarPlane);
	glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote);		
	sprintf(quote, "%.2f,", m_fRadius);
	glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote);		
#endif

	glPopMatrix();

}

/*	pRaw points to raw data which has been parsed by ParseRawObject.
	The function find the related raw object based on Target Number.

	Return Value:
		If the source data and raw are pairs, TRUE will be returned,
		otherwise, FALSE will be returned.
*/

afx_msg LRESULT CBirdviewView::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	PARAM_STRUCT data;
	CPoint pnt;
	CString str;
	CList <PARAM_STRUCT, PARAM_STRUCT&>* p;
	POSITION pos;
	POSITION posOld;
	RAW_OBJECT_STRUCT raw;
	PARAM_STRUCT data2nd;
	POSITION pos2nd;
	POSITION pos2ndOld;
	RAW_OBJECT_STRUCT raw2nd;

	if (GetSafeHwnd() == NULL)
		return 0;

	DrawScene();
	DrawFarTarget();
	DrawText();


	/*	The drawing case should consider the array index of rcvIdent[].
		The order is mandatory. Arbitrarily change will impact other portion
	*/
	int arrIdx = 0;
	for (arrIdx = 0; arrIdx < _ListArray.GetSize(); arrIdx++) {
		WaitForSingleObject(_ListArrayMutex.GetAt(arrIdx), INFINITE);
		p = _ListArray.GetAt(arrIdx);
		if (!p) {
			ReleaseMutex(_ListArrayMutex.GetAt(arrIdx));
			continue;
		}
		switch (arrIdx) {
		case 0:
		case 1:
			pos = p->GetHeadPosition();
			while (pos) {
				posOld = pos;
				data = p->GetNext(pos);
				if (data.Ident == 0x402 || data.Ident == 0x412) {
					continue;
				}
				ParseRawObject(&data, &raw);
				pos2nd = pos;
				while (pos2nd) {
					data2nd = p->GetAt(pos2nd);
					if (data2nd.Ident == 0x401 || data2nd.Ident == 0x411) {
						p->GetNext(pos2nd);
						continue;
					}
					if (ParseRawObject2nd(&data2nd, &raw)) {
						DrawRawObject3D(&raw, arrIdx);
						if (pos == pos2nd)
							p->GetNext(pos);
						p->RemoveAt(pos2nd);
						break;
					} else {
						p->GetNext(pos2nd);
						continue;
					}
				}
			}
			break;
		case 2:
			pos = p->GetHeadPosition();
			while (pos) {
				posOld = pos;
				data = p->GetNext(pos);
				if (data.DataLength != 8 || data.Ident < 0x610 || data.Ident > 0x620) {
					continue;
				}
				ParseTrackingObject(&data, &raw);
				pos2nd = pos;
				while (pos2nd) {
					data2nd = p->GetAt(pos2nd);
					if (data2nd.DataLength != 7 || data2nd.Ident < 0x610 || data2nd.Ident > 0x620) {
						p->GetNext(pos2nd);
						continue;
					}
					if (ParseTrackingObject2nd(&data2nd, &raw)) {
						DrawTrackingObject3D(&raw);
						if (pos == pos2nd)
							p->GetNext(pos);
						p->RemoveAt(pos2nd);
						break;
					} else {
						p->GetNext(pos2nd);
						continue;
					}
				}
			}
			break;
		default:
			p->RemoveAll();
		}
		ReleaseMutex(_ListArrayMutex.GetAt(arrIdx));
	}

	glFinish();
	SwapBuffers(wglGetCurrentDC());
	return 0;
}

BOOL CBirdviewView::ParseTrackingObject2nd(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
{
	int temp;

	temp = (pSrc->RCV_data[7] & 0xFC) >> 2;

	if (pRaw->TargetNum != temp)
		return FALSE;

	temp = pSrc->RCV_data[3] & 0x01;
	temp = (temp << 8) + pSrc->RCV_data[2];
	temp = (temp << 3) + ((pSrc->RCV_data[1] & 0xE0) >> 5);
	pRaw->z_range = (temp - 2048) * 0.016;

	return TRUE;
}


void CBirdviewView::DrawTrackingObject3D(RAW_OBJECT_STRUCT *pRaw)
{
	float tempX;
	float tempZ;
	float tempY;

	tempZ = -pRaw->x_range;
	tempX = -pRaw->y_range;
	tempY = pRaw->z_range;

	glColor3f(0.0f, 1.0f, 0.0f);

	//front
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, tempY + 0.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, tempY + 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	//back
	glPushMatrix();
	glNormal3f(0.0f, 0.0f, -1.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, tempY + 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX - halfCarWidth, tempY + 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 0.0f, tempZ - halfCarLen);
	glEnd();
	glPopMatrix();

	//right
	glPushMatrix();
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX + halfCarWidth, tempY + 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	//left
	glPushMatrix();
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, tempY + 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, tempY + 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX - halfCarWidth, tempY + 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX - halfCarWidth, tempY + 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	//top
	glPushMatrix();
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, tempY + 1.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, tempY + 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 1.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 1.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

	glPushMatrix();
	glColor3f(0.8f, 0.8f, 0.8f);
	glRasterPos3f(tempX - halfCarWidth, tempY + 1.0f, tempZ + halfCarLen);
	sprintf(quote, "%d", pRaw->TargetNum);
	glCallLists(strlen(quote), GL_UNSIGNED_BYTE, quote);
	glPopMatrix();

	//bottom
	glPushMatrix();
	glNormal3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex3f(tempX - halfCarWidth, tempY + 0.0f, tempZ + halfCarLen);
	glVertex3f(tempX - halfCarWidth, tempY + 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 0.0f, tempZ - halfCarLen);
	glVertex3f(tempX + halfCarWidth, tempY + 0.0f, tempZ + halfCarLen);
	glEnd();
	glPopMatrix();

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
	pRaw->z_range = 0.0f;
}

BOOL CBirdviewView::ParseRawObject2nd(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
{
	int temp;

	temp = pSrc->RCV_data[0] & 0x3F;
	if (temp != pRaw->TargetNum)
		return FALSE;

	temp = pSrc->RCV_data[6] & 0x07;
	temp = (temp << 8) + pSrc->RCV_data[5];
	pRaw->theta = (temp - 1024) * 0.16;
	pRaw->theta += 5;

	pRaw->z_range = pRaw->range * sin(pRaw->theta * RAD_CONVER);

	return TRUE;
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


unsigned char CBirdviewView::ComponentFromIndex(int i, UINT nbits, UINT shift)
{
	unsigned char val;

	val = (unsigned char) (i >> shift);
	switch (nbits)
	{

	case 1:
		val &= 0x1;
		return oneto8[val];
	case 2:
		val &= 0x3;
		return twoto8[val];
	case 3:
		val &= 0x7;
		return threeto8[val];

	default:
		return 0;
	}
}

void CBirdviewView::Init()
{
	PIXELFORMATDESCRIPTOR pfd;
	int         n;

	m_pDC = new CClientDC(this);

	ASSERT(m_pDC != NULL);

	if (!bSetupPixelFormat())
		return;

	n = ::GetPixelFormat(m_pDC->GetSafeHdc());
	::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	CreateRGBPalette();

	m_hrc = wglCreateContext(m_pDC->GetSafeHdc());
	wglMakeCurrent(m_pDC->GetSafeHdc(), m_hrc);

/*
HFONT font = CreateFont(12, 0, 0, 0,
                           FW_NORMAL, FALSE, FALSE, FALSE,
                           ANSI_CHARSET, 0,
                            0,0,0, _T("Times New Roman"));
	SelectObject(m_pDC->GetSafeHdc(), font);
*/
	wglUseFontBitmaps(m_pDC->GetSafeHdc(), 0, 256, 1000); 
	// move bottom left, southwest of the red triangle  
	//glRasterPos2f(30.0F, 300.0F); 
	// set up for a string-drawing display list call  
	glListBase(1000); 

	GetClientRect(&m_oldRect);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING); //Enable lighting
	glEnable(GL_LIGHT0); //Enable light #0
	glEnable(GL_LIGHT1); //Enable light #1
	glEnable(GL_NORMALIZE); //Automatically normalize normals

	if (m_oldRect.bottom)
		m_fAspect = (GLfloat)m_oldRect.Width()/m_oldRect.Height();
	else    // don't divide by zero, not that we should ever run into that...
		m_fAspect = 1.0f;
	m_fNearPlane = 30.0f;
	m_fFarPlane = 300;
	m_fMaxObjSize = m_fFarPlane / 2;
	m_fMaxObjSizeOld = m_fMaxObjSize;
	m_fRadius = m_fNearPlane + m_fMaxObjSize / 2.0f;
	m_fFov = 30.0f;

	m_fOldRadius = m_fRadius;
	m_fOldFarPlane = m_fFarPlane;
	m_fOldFov = m_fFov;

#if 0
	glViewport(0, 0, (GLsizei)m_oldRect.Width(), (GLsizei)m_oldRect.Height());
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.5, 1.5, -1.5, 1.5, -10, 10);
#else
	glViewport(0, 0, m_oldRect.Width(), m_oldRect.Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_fFov, m_fAspect, m_fNearPlane, m_fFarPlane);
	glMatrixMode(GL_MODELVIEW);
#endif
}

BOOL CBirdviewView::bSetupPixelFormat()
{
	static PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),  // size of this pfd
		1,                              // version number
		PFD_DRAW_TO_WINDOW |            // support window
		  PFD_SUPPORT_OPENGL |          // support OpenGL
		  PFD_DOUBLEBUFFER,             // double buffered
		PFD_TYPE_RGBA,                  // RGBA type
		24,                             // 24-bit color depth
		0, 0, 0, 0, 0, 0,               // color bits ignored
		0,                              // no alpha buffer
		0,                              // shift bit ignored
		0,                              // no accumulation buffer
		0, 0, 0, 0,                     // accum bits ignored
		32,                             // 32-bit z-buffer
		0,                              // no stencil buffer
		0,                              // no auxiliary buffer
		PFD_MAIN_PLANE,                 // main layer
		0,                              // reserved
		0, 0, 0                         // layer masks ignored
	};
	int pixelformat;

	if ( (pixelformat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0 )
	{
		MessageBox(_T("ChoosePixelFormat failed"));
		return FALSE;
	}

	if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelformat, &pfd) == FALSE)
	{
		MessageBox(_T("SetPixelFormat failed"));
		return FALSE;
	}

	return TRUE;
}

#pragma warning(disable : 4244)
void CBirdviewView::CreateRGBPalette()
{
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE *pPal;
	int n, i;

	n = ::GetPixelFormat(m_pDC->GetSafeHdc());
	::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);

	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		n = 1 << pfd.cColorBits;
		pPal = (PLOGPALETTE) new char[sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY)];

		ASSERT(pPal != NULL);

		pPal->palVersion = 0x300;
		pPal->palNumEntries = n;
		for (i=0; i<n; i++)
		{
			pPal->palPalEntry[i].peRed =
					ComponentFromIndex(i, pfd.cRedBits, pfd.cRedShift);
			pPal->palPalEntry[i].peGreen =
					ComponentFromIndex(i, pfd.cGreenBits, pfd.cGreenShift);
			pPal->palPalEntry[i].peBlue =
					ComponentFromIndex(i, pfd.cBlueBits, pfd.cBlueShift);
			pPal->palPalEntry[i].peFlags = 0;
		}

		/* fix up the palette to include the default GDI palette */
		if ((pfd.cColorBits == 8)                           &&
			(pfd.cRedBits   == 3) && (pfd.cRedShift   == 0) &&
			(pfd.cGreenBits == 3) && (pfd.cGreenShift == 3) &&
			(pfd.cBlueBits  == 2) && (pfd.cBlueShift  == 6)
		   )
		{
			for (i = 1 ; i <= 12 ; i++)
				pPal->palPalEntry[defaultOverride[i]] = defaultPalEntry[i];
		}

		m_cPalette.CreatePalette(pPal);
		delete [] pPal;

		m_pOldPalette = m_pDC->SelectPalette(&m_cPalette, FALSE);
		m_pDC->RealizePalette();
	}
}
#pragma warning(default : 4244)
void CBirdviewView::OnDraw(CDC* /*pDC*/)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
}


BOOL CBirdviewView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	return TRUE;
	return CView::OnEraseBkgnd(pDC);
}



void CBirdviewView::OnTimer(UINT_PTR nIDEvent)
{

	SendMessage(WM_USER_DRAW, 0, 0);
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CView::OnTimer(nIDEvent);

	// Eat spurious WM_TIMER messages
	MSG msg;
	while(::PeekMessage(&msg, m_hWnd, WM_TIMER, WM_TIMER, PM_REMOVE));
}

#pragma warning(default : 4244)

void CBirdviewView::DrawScene()
{
	static GLfloat  wAngleY = 45.0f;
	static GLfloat  wAngleX = 1.0f;
	static GLfloat  wAngleZ = 5.0f;

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glLoadIdentity();
	glTranslatef(0.0f, -10.0f, -m_fRadius);

#if 1
	//glRotatef(20.0f, 0.0f, 1.0f, 0.0f);
	glScalef(2.0f, 2.0f, 2.0f);
	gluLookAt(0.0f, 2.0f, halfCarLen * 4, 0.0f, 0.0f, -(200.0f), 0.0f, 1.0f, 0.0f);
	//gluLookAt(0.0f, 0.0f, -halfCarLen * 2, 0.0f, 0.0f, 100.0f, 0.0f, 1.0f, 0.0f);
	//
	GLfloat ambientColor[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.2, 0.2, 0.2)
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambientColor);
	
	//Add positioned light
	GLfloat lightColor0[] = {0.2f, 0.2f, 0.2f, 1.0f}; //Color (0.5, 0.5, 0.5)
	GLfloat lightPos0[] = {0.0f, 0.0f, -8.0f, 1.0f}; //Positioned at (4, 0, 8)
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightColor0);
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);
	
	//Add directed light
	GLfloat lightColor1[] = {0.5f, 0.5f, 0.5f, 1.0f}; //Color (0.5, 0.2, 0.2)
	//Coming from the direction (-1, 0.5, 0.5)
	GLfloat lightPos1[] = {-5.0f, 8.0f, -8.0f, 0.0f};
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightColor1);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);
	
	glPushMatrix();
	glColor3f(1.0f, 0.0f, 0.0f);

	//front
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-halfCarWidth, 2.0f, halfCarLen);
	glVertex3f(-halfCarWidth, 0.0f, halfCarLen);
	glVertex3f(halfCarWidth, 0.0f, halfCarLen);
	glVertex3f(halfCarWidth, 2.0f, halfCarLen);
	glEnd();

	//back
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 0.0f, -1.0f);
	glVertex3f(-halfCarWidth, 2.0f, -halfCarLen);
	glVertex3f(-halfCarWidth, 0.0f, -halfCarLen);
	glVertex3f(halfCarWidth, 0.0f, -halfCarLen);
	glVertex3f(halfCarWidth, 2.0f, -halfCarLen);
	glEnd();

	//right
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(halfCarWidth, 2.0f, halfCarLen);
	glVertex3f(halfCarWidth, 2.0f, -halfCarLen);
	glVertex3f(halfCarWidth, 0.0f, -halfCarLen);
	glVertex3f(halfCarWidth, 0.0f, halfCarLen);
	glEnd();

	//left
	glBegin(GL_QUADS);
	glNormal3f(-1.0f, 0.0f, 0.0f);
	glVertex3f(-halfCarWidth, 2.0f, halfCarLen);
	glVertex3f(-halfCarWidth, 2.0f, -halfCarLen);
	glVertex3f(-halfCarWidth, 0.0f, -halfCarLen);
	glVertex3f(-halfCarWidth, 0.0f, halfCarLen);
	glEnd();

	//top
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-halfCarWidth, 2.0f, halfCarLen);
	glVertex3f(halfCarWidth, 2.0f, halfCarLen);
	glVertex3f(halfCarWidth, 2.0f, -halfCarLen);
	glVertex3f(-halfCarWidth, 2.0f, -halfCarLen);
	glEnd();

	//bottom
	glBegin(GL_QUADS);
	glNormal3f(0.0f, 1.0f, 0.0f);
	glVertex3f(-halfCarWidth,	0.0f, halfCarLen);
	glVertex3f(halfCarWidth,	0.0f, halfCarLen);
	glVertex3f(halfCarWidth,	0.0f, -halfCarLen);
	glVertex3f(-halfCarWidth,	0.0f, -halfCarLen);
	glEnd();
	glPopMatrix();

#if 1
	float tempX = 0;
	float tempZ = 0;

	//drawing left fan
	glPushMatrix();
	glTranslatef(-halfCarWidth, 0.0f, halfCarLen);
	glRotatef(-SENSOR_ROTATE, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINE_LOOP);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (float idx = -60; idx <= 60; idx += 0.01) {
		tempX = FAN_LEN * sin(idx * 3.1415926 / 180);
		tempZ = FAN_LEN * cos(idx * 3.1415926 / 180);
		glVertex3f(tempX, 0.0f, tempZ);
	}
	glEnd();
	glPopMatrix();

	//drawing right fan
	glPushMatrix();
	glTranslatef(halfCarWidth, 0.0f, halfCarLen);
	glRotatef(SENSOR_ROTATE, 0.0f, 1.0f, 0.0f);

	glBegin(GL_LINE_LOOP);
	glColor3f(0.5f, 0.5f, 0.5f);
	glVertex3f(0.0f, 0.0f, 0.0f);
	for (float idx = -60; idx <= 60; idx += 0.01) {
		tempX = FAN_LEN * sin(idx * 3.1415926 / 180);
		tempZ = FAN_LEN * cos(idx * 3.1415926 / 180);
		glVertex3f(tempX, 0.0f, tempZ);
	}
	glEnd();
	glPopMatrix();

	float temp;
	temp = -1000.0f;

	m_RoadLineStartZ += m_RoadLineStartZStep;

	glPushMatrix();
	//glTranslatef(halfRoadWidth, 0.0f, 0.0f);
	glBegin(GL_LINES);

	if (m_RoadLineStartZ > DASHED_STEP * 2)
		m_RoadLineStartZ -= (DASHED_STEP * 2);

	float temp1 = temp;
	for (int idx = 1; idx <= 7; idx += 2) {
		glColor3f(0.5f, 0.5f, 0.5f);
		glVertex3f(halfRoadWidth * idx, 0.0f, temp);
		glVertex3f(halfRoadWidth * idx, 0.0f, temp + m_RoadLineStartZ);
		glVertex3f(halfRoadWidth * -idx, 0.0f, temp);
		glVertex3f(halfRoadWidth * -idx, 0.0f, temp + m_RoadLineStartZ);
		temp += DASHED_STEP;

		glColor3f(0.0f, 0.0f, 0.0f);
		glVertex3f(halfRoadWidth * idx, 0.0f, temp);
		glVertex3f(halfRoadWidth * idx, 0.0f, temp + DASHED_STEP);
		glVertex3f(halfRoadWidth * -idx, 0.0f, temp);
		glVertex3f(halfRoadWidth * -idx, 0.0f, temp + DASHED_STEP);
		temp = temp1;
	}

	temp = temp1 + m_RoadLineStartZ + DASHED_STEP;
	temp1 = temp;
	for (int idx = 1; idx <= 7; idx += 2) {
		while (!(temp > ((m_fFarPlane - m_fNearPlane) / 2))) {
			glColor3f(0.5f, 0.5f, 0.5f);
			glVertex3f(halfRoadWidth * idx, 0.0f, temp);
			glVertex3f(halfRoadWidth * idx, 0.0f, temp + DASHED_STEP);
			glVertex3f(halfRoadWidth * -idx, 0.0f, temp);
			glVertex3f(halfRoadWidth * -idx, 0.0f, temp + DASHED_STEP);
			temp += DASHED_STEP;

			glColor3f(0.0f, 0.0f, 0.0f);
			glVertex3f(halfRoadWidth * idx, 0.0f, temp);
			glVertex3f(halfRoadWidth * idx, 0.0f, temp + DASHED_STEP);
			glVertex3f(halfRoadWidth * -idx, 0.0f, temp);
			glVertex3f(halfRoadWidth * -idx, 0.0f, temp + DASHED_STEP);
			temp += DASHED_STEP;
		}
		temp = temp1;
	}
	glEnd();
	glPopMatrix();
#endif

#endif

}


void CBirdviewView::OnDestroy()
{
	HGLRC   hrc;

	KillTimer(0);

	hrc = ::wglGetCurrentContext();

	::wglMakeCurrent(NULL,  NULL);

	if (hrc)
		::wglDeleteContext(hrc);

	if (m_pOldPalette)
		m_pDC->SelectPalette(m_pOldPalette, FALSE);

	if (m_pDC)
		delete m_pDC;

	CView::OnDestroy();

	// TODO: 在此加入您的訊息處理常式程式碼
}


void CBirdviewView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼
	CRect rect;
	GetClientRect(&rect);
	glViewport(0, 0, rect.Width(), rect.Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(m_fFov, (double)rect.Width()/ (double)rect.Height(), m_fNearPlane, m_fFarPlane);
	glMatrixMode(GL_MODELVIEW);

}


afx_msg LRESULT CBirdviewView::OnUpdateSpeedDrawing(WPARAM wParam, LPARAM lParam)
{
	m_RoadLineStartZStep = (float)wParam / 6000.0f;
	m_fRadius = m_fOldRadius + (float)wParam / 100.0f;
	m_fFarPlane = m_fOldFarPlane + (float)wParam / 10.0f;

	CRect rect;
	GetClientRect(&rect);

	glViewport(0, 0, rect.Width(), rect.Height());
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	m_fAspect = (float)rect.Width()/(float)rect.Height();//+ (float)wParam / 100.0f;
	gluPerspective(m_fFov, m_fAspect, m_fNearPlane, m_fFarPlane);
	glMatrixMode(GL_MODELVIEW);
	//m_RoadLineStartZStep /= 10;
	return 0;
}


void CBirdviewView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	switch (nChar) {
	case VK_UP:
		if (m_Speed <= 322.0f)
			m_Speed += 10.0f;
		break;
	case VK_DOWN:
		if (m_Speed > 0.0f)
			m_Speed -= 10.0f;
		else 
			m_Speed = 0;
		break;
	default:
		break;
	}
	SendMessage(WM_UPDATE_SPEED_DRAWING, m_Speed*100, 0);
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}

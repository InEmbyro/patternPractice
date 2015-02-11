// dllmain.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include <afxwin.h>
#include <afxdllx.h>
#include <iostream>
#include <vector>
#include "../canEngine/canEngineApi.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "targetList.h"
#include "CReceiveThread.h"

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

unsigned long recvId[] = {
	0x601, 0x610, 0x611, 0x612, 0x613, 0x614, 0x615, 0x616, 0x617, 0x618, 0x619, 0x620, 0x00,
	0x400, 0x401, 0x402, 0x00,
	0x410, 0x411, 0x412, 0x00
};

const char *comboStr[] = {
	"Tracking 01", 
	"Raw 01",
	"Raw 02",
	NULL};

std::vector<CString> comboList;


const char* CTargetList::mailslot = "\\\\.\\mailslot\\wnc_targetlist_view";
static AFX_EXTENSION_MODULE targetListDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	// 如果您使用 lpReserved，請移除這個
	UNREFERENCED_PARAMETER(lpReserved);

	if (dwReason == DLL_PROCESS_ATTACH)
	{
		TRACE0("targetList.DLL 正在初始化!\n");
		
		// 擴充 DLL 的一次初始化
		if (!AfxInitExtensionModule(targetListDLL, hInstance))
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

		new CDynLinkLibrary(targetListDLL);

	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		TRACE0("targetList.DLL 正在終止!\n");

		// 呼叫解構函式前先終止程式庫
		AfxTermExtensionModule(targetListDLL);
	}
	return 1;   // 確定
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
// D:\workspace\myDr\targetList\targetList.cpp : 實作檔
//
// CTargetList

IMPLEMENT_DYNCREATE(CTargetList, CFormView)

CTargetList::CTargetList()
	: CFormView(CTargetList::IDD)
{
	CString sz;
	const char **ptr = &comboStr[0];
	while (*ptr) {
		sz = *ptr;
		comboList.push_back(sz);
		ptr++;
	}
	_filterMap.InitHashTable(100);
#if 1
	_MapShowMutex = CreateMutex(NULL, FALSE, NULL);
	_MapStoreMutex = CreateMutex(NULL, FALSE, NULL);
	_MapShow = new CMap <unsigned char, unsigned char, RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT>;
	if (_MapShow) {
		_MapStore = new CMap <unsigned char, unsigned char, RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT>;
		if (_MapStore) {
			_MapShow->InitHashTable(50);
			_MapStore->InitHashTable(50);
		} else {
			AfxMessageBox(_T("!_MapStore"));
			delete _MapShow;
		}
	} else {
		AfxMessageBox(_T("!_MapShow"));
	}
#else
	_listMutex = CreateMutex(NULL, FALSE, NULL);
	_listStoreMutex = CreateMutex(NULL, FALSE, NULL);

	_list = new CList <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&>;
	if (_list) {
		_listStore = new CList <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&>;
		if (!_listStore) {
			AfxMessageBox(_T("!_listStore"));
		}
	} else {
		AfxMessageBox(_T("!_list"));
	}
#endif
}

CTargetList::~CTargetList()
{
#if 1
	CloseHandle(_MapShowMutex);
	CloseHandle(_MapStoreMutex);
	if (_MapShow)
		delete _MapShow;
	if (_MapStore)
		delete _MapStore;

#else
	CloseHandle(_listMutex);
	CloseHandle(_listStoreMutex);
	if (_list)
		delete _list;
	if (_listStore)
		delete _listStore;
#endif
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
	ON_MESSAGE(WM_USER_DRAW, &CTargetList::OnUserDraw)
	ON_CBN_SELCHANGE(IDC_TARGETLIST_COMBO, &CTargetList::OnCbnSelchangeTargetlistCombo)
//	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()


// CTargetList 診斷

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


// CTargetList 訊息處理常式
// D:\workspace\myDr\targetList\targetList.cpp : 實作檔
//
// CTargetListForm

IMPLEMENT_DYNCREATE(CTargetListForm, CMDIChildWnd)

CTargetListForm::CTargetListForm()
{
	pRcvThread = new CReceiveThread();
	_pView = NULL;

}

CTargetListForm::~CTargetListForm()
{
	delete pRcvThread;
}


BEGIN_MESSAGE_MAP(CTargetListForm, CMDIChildWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CTargetListForm 訊息處理常式

BOOL CTargetList::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	return CFormView::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);
}
void CTargetList::ParseRawObject2nd(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
{
	int temp;

	temp = pSrc->RCV_data[0] & 0x3F;
	pRaw->TargetNum = temp;

	temp = pSrc->RCV_data[6] & 0x07;
	temp = (temp << 8) + pSrc->RCV_data[5];
	pRaw->theta = (temp - 1024) * 0.16;
}
void CTargetList::ParseRawObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
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
	temp = (temp << 6) + ((pSrc->RCV_data[6] & 0xFC) >> 2);
	pRaw->relatedSpeed = (temp - 8192) * 0.02;

	pRaw->x_range = pRaw->range * sin(pRaw->angle * RAD_CONVER);
	pRaw->y_range = pRaw->range * cos(pRaw->angle * RAD_CONVER);
}


void CTargetList::ParseTrackingObject2nd(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
{
	int temp;

	pRaw->TargetNum = (pSrc->RCV_data[0] & 0x3F);

	temp = pSrc->RCV_data[3] & 0x01;
	temp = (temp << 8) + pSrc->RCV_data[2];
	temp = (temp << 3) + ((pSrc->RCV_data[1] & 0xE0) >> 5);
	pRaw->z_point = (temp - 2048) * 0.016;

	temp = pSrc->RCV_data[5] & 0x7F;
	temp = (temp << 4) + ((pSrc->RCV_data[4] & 0xF0) >> 4);
	pRaw->z_speed = (temp - 1024) * 0.1;
}

void CTargetList::ParseTrackingObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw)
{
	if (pSrc == NULL || pRaw == NULL)
		return;

	int temp;

	pRaw->TargetNum = (pSrc->RCV_data[7] & 0xFC) >> 2;
	
	temp = pSrc->RCV_data[1] & 0x1F;
	temp = (temp << 8) + pSrc->RCV_data[0];
	pRaw->x_point = (temp - 2500) * 0.016;

	temp = pSrc->RCV_data[3] & 0x01;
	temp = (temp << 8) + pSrc->RCV_data[2];
	temp = (temp << 3) + ((pSrc->RCV_data[1] & 0xE0) >> 5);
	pRaw->y_point = (temp - 2048) * 0.016;

	temp = pSrc->RCV_data[4] & 0x0F;
	temp = (temp << 7) + ((pSrc->RCV_data[3] & 0xFE) >> 1);
	pRaw->x_speed = (temp - 1024) * 0.1;

	temp = pSrc->RCV_data[5] & 0x7F;
	temp = (temp << 4) + ((pSrc->RCV_data[4] & 0xF0) >> 4);
	pRaw->y_speed = (temp - 1024) * 0.1;

	temp = pSrc->RCV_data[6] & 0x01;
	temp = (temp << 1) + ((pSrc->RCV_data[5] & 0x80) >> 7);
	pRaw->lane = temp;

	temp = pSrc->RCV_data[6] & 0x1E;
	pRaw->len = (temp >> 1);

	temp = pSrc->RCV_data[7] & 0x03;
	temp = (temp << 3) + ((pSrc->RCV_data[6] & 0xE0) >> 5);
	pRaw->size = temp * 0.064;
}

int CTargetListForm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	HINSTANCE hInstOld = AfxGetResourceHandle();
	AfxSetResourceHandle(targetListDLL.hModule);
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1) {
		AfxSetResourceHandle(hInstOld);
		return -1;
	}

	// TODO:  在此加入特別建立的程式碼
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

	_pView->m_combo.ResetContent();
	vector<CString>::iterator ite;
	for (ite = comboList.begin(); ite < comboList.end(); ite++) {
		_pView->m_combo.AddString(*ite);
	}

	/*	The default selected item of combon list is TargetList.
		So, only the CAN ID of tracking ID is added in _filterMap	*/
	_pView->m_combo.SetCurSel(0);
	for (int idx = 0; idx < sizeof(recvId)/sizeof(recvId[0]); idx++) {
		if (recvId[idx] == 0x00)
			break;
		_pView->_filterMap.SetAt(recvId[idx], recvId[idx]);
	}

	if (!pRcvThread) {
		AfxMessageBox(_T("!pRcvThread"));
		return -1;
	}

	CString name;
	POSITION pos;
	name = CTargetList::mailslot;
	pos = RegisterAcquire01(name);
	if (pos == NULL) {
		AfxMessageBox(_T("CTargetListForm::mailslot"));
		AfxSetResourceHandle(hInstOld);
		return -1;
	}
	
	pRcvThread->_pView = (LPVOID)_pView;
	pRcvThread->SetInfoHandle(InforEventAcquire(pos));
	pRcvThread->SetMailHandle(MailSlotAcquire(pos));
	rawPos = pos;

	if (!pRcvThread->InitThread()) {
		AfxMessageBox(_T("pRcvThread->InitThread"));
		DeregisterAcquire01(rawPos);
	}
	_pView->pRcvThread = pRcvThread;

	UpdateData(FALSE);

	AfxSetResourceHandle(hInstOld);
	return 0;
}


void CTargetList::OnSize(UINT nType, int cx, int cy)
{
	CFormView::OnSize(nType, cx, cy);

	// TODO: 在此加入您的訊息處理常式程式碼
	if (m_listctrl.m_hWnd)
		m_listctrl.SetWindowPos(NULL, 0, 0, cx, cy,SWP_NOMOVE);
}

int __cdecl CTargetList::Compare1(const RAW_OBJECT_STRUCT * p1, const RAW_OBJECT_STRUCT * p2)
{
	if (p1->range > p2->range)
		return 1;
	else if (p1->range < p2->range)
		return -1;
	else
		return 0;

}

int __cdecl CTargetList::Compare0(const RAW_OBJECT_STRUCT * p1, const RAW_OBJECT_STRUCT * p2)
{
	if (p1->x_point > p2->x_point)
		return 1;
	else if (p1->x_point < p2->x_point)
		return -1;
	else
		return 0;
}

afx_msg LRESULT CTargetList::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	int row = 0;
	int idx = 0;
	CString sz;
	RAW_OBJECT_STRUCT data;
	POSITION pos;
	
	_listArray.RemoveAll();
#if 1
	WaitForSingleObject(_MapShowMutex, INFINITE);
	idx = 0;
	while (_MapShow->Lookup(idx++, data)) {
		_listArray.Add(data);
	}
	ReleaseMutex(_MapShowMutex);
#else
	WaitForSingleObject(_listMutex, INFINITE);
	while ((pos = _list->FindIndex(row++))) {
		data = _list->GetAt(pos);
		_listArray.Add(data);
	}
	ReleaseMutex(_listMutex);
#endif
	RAW_OBJECT_STRUCT *pD = _listArray.GetData();
	if (m_combo.GetCurSel() == 0)
		qsort(pD, _listArray.GetSize(), sizeof(RAW_OBJECT_STRUCT), (GENERICCOMPAREFN1)Compare0);
	else
		qsort(pD, _listArray.GetSize(), sizeof(RAW_OBJECT_STRUCT), (GENERICCOMPAREFN1)Compare1);
	row = 0;

	switch (m_combo.GetCurSel()) {
	case 0:
		m_listctrl.DeleteAllItems();
		for (row = 0; row < _listArray.GetCount(); row++) {
			data = _listArray.GetAt(row);
			idx = 1;
			sz.Format(_T("%d"), data.TargetNum);
			m_listctrl.InsertItem(row, sz);
			sz.Format(_T("%.3f"), data.x_point);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.3f"), data.y_point);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.3f"), data.z_point);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.3f"), data.x_speed);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.3f"), data.y_speed);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.3f"), data.z_speed);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%d"), data.lane);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%d"), data.len);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%d"), data.size);
			m_listctrl.SetItemText(row, idx++, sz);
		}
		break;
	case 1:
	case 2:
		m_listctrl.DeleteAllItems();
		for (row = 0; row < _listArray.GetCount(); row++) {
			data = _listArray.GetAt(row);
			idx = 1;
			sz.Format(_T("%d"), data.TargetNum);
			m_listctrl.InsertItem(row, sz);
			sz.Format(_T("%.2f"), data.angle);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.2f"), data.theta);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.2f"), data.range);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.2f"), data.AbsLevel_db);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%d"), data.type);
			m_listctrl.SetItemText(row, idx++, sz);
			sz.Format(_T("%.2f"), data.relatedSpeed);
			m_listctrl.SetItemText(row, idx++, sz);
		}
		break;
	default:
		break;
	}

	return 0;
}

void CTargetListForm::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	_pView->pRcvThread->TerminateThread();
	WaitForSingleObject(_pView->pRcvThread->getConfirmHnd(), 5000);
	DeregisterAcquire01(rawPos);
	CMDIChildWnd::OnClose();
}

int CTargetList::GetComboIdx()
{
	return m_combo.GetCurSel();
}

void CTargetList::OnCbnSelchangeTargetlistCombo()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	int selIdx = m_combo.GetCurSel();
	
	int idx = 0;
	while (selIdx != 0) {
		if (recvId[idx++] == 0)
			selIdx--;
	}

	_filterMap.RemoveAll();
	for (; idx <(sizeof(recvId)/sizeof(recvId[0])); idx++) {
		if (recvId[idx] == 0x0)
			break;
		_filterMap.SetAt(recvId[idx], recvId[idx]);
	}

	switch (m_combo.GetCurSel()) {
	case 0:
		SetTrackingHeader();
		break;
	case 1:
	case 2:
		SetRawHeader();
		break;
	}

#if 1
	WaitForSingleObject(_MapShowMutex, INFINITE);
	_MapShow->RemoveAll();
	ReleaseMutex(_MapShowMutex);

	WaitForSingleObject(_MapStoreMutex, INFINITE);
	_MapStore->RemoveAll();
	ReleaseMutex(_MapStoreMutex);
#else
	WaitForSingleObject(_listMutex, INFINITE);
	_list->RemoveAll();
	if (_list)
		delete _list;
	_list = new CList <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&>;
	ReleaseMutex(_listMutex);
	WaitForSingleObject(_listStoreMutex, INFINITE);
	_listStore->RemoveAll();
	if (_listStore)
		delete _listStore;
	_listStore = new CList <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&>;
	ReleaseMutex(_listStoreMutex);
#endif
}


void CTargetList::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	SetTrackingHeader();
}

void CTargetList::SetTrackingHeader()
{
	int idx = m_listctrl.GetHeaderCtrl()->GetItemCount();
	while (idx--)
		m_listctrl.GetHeaderCtrl()->DeleteItem(0);

	idx = 0;
	m_listctrl.InsertColumn(idx++, _T("No."),			LVCFMT_LEFT, 50, 0);
	m_listctrl.InsertColumn(idx++, _T("x_point[m]"),	LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("y_point[m]"),	LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("z_point[m]"),	LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Speed_x[m/s]"),	LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Speed_y[m/s]"),	LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Speed_z[m/s]"),	LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Lane"),			LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Length"),		LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Size"),			LVCFMT_LEFT, 90, 0);
}

void CTargetList::SetRawHeader()
{
	int idx = m_listctrl.GetHeaderCtrl()->GetItemCount();
	while (idx--)
		m_listctrl.GetHeaderCtrl()->DeleteItem(0);

	idx = 0;
	m_listctrl.InsertColumn(idx++, _T("No."),				LVCFMT_LEFT, 50, 0);
	m_listctrl.InsertColumn(idx++, _T("Azimuth[deg]"),		LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Elevation[deg]"),		LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Range[m]"),			LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Amplitude"),			LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Class"),				LVCFMT_LEFT, 90, 0);
	m_listctrl.InsertColumn(idx++, _T("Speed_radial[m/s]"), LVCFMT_LEFT, 90, 0);
}

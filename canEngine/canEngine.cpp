// canEngine.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include "canEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: 如果這個 DLL 是動態地對 MFC DLL 連結，
//		那麼從這個 DLL 匯出的任何會呼叫
//		MFC 內部的函式，都必須在函式一開頭加上 AFX_MANAGE_STATE
//		巨集。
//
//		例如:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// 此處為正常函式主體
//		}
//
//		這個巨集一定要出現在每一個
//		函式中，才能夠呼叫 MFC 的內部。這意味著
//		它必須是函式內的第一個陳述式
//		，甚至必須在任何物件變數宣告前面
//		，因為它們的建構函式可能會產生對 MFC
//		DLL 內部的呼叫。
//
//		請參閱 MFC 技術提示 33 和 58 中的
//		詳細資料。
//

// CcanEngineApp



BEGIN_MESSAGE_MAP(CcanEngineApp, CWinApp)
END_MESSAGE_MAP()


// CcanEngineApp 建構

CcanEngineApp::CcanEngineApp()
{
	// TODO: 在此加入建構程式碼，
	// 將所有重要的初始設定加入 InitInstance 中
}

CcanEngineApp::~CcanEngineApp()
{

}


// 僅有的一個 CcanEngineApp 物件

CcanEngineApp theApp;

// CcanEngineApp 初始設定
BOOL CcanEngineApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

CCanInfo::CCanInfo()
	:m_pBuf(NULL)
{

	_pThreadHandle = NULL;
	for (int i = 0; i < 2; i++)
		_ThreadEvent[i] = INVALID_HANDLE_VALUE;

	CCanRaw *_p;
	int i = 3;
	while (i--) {
		_p = new CCanRaw();
		if (_p) {
			_p->_refCount = 0;
			_pRawList.AddHead(_p);
		}
	}
}

CCanInfo::~CCanInfo()
{
	if (m_pBuf)
		free(m_pBuf);
	
	/*	_pThreadHandle pointers to a address, which is created by AfxBeginThread.
		Theoretically, the pointers will be released when the thread exists. But, 
		the thread is brutally terminated by UNKNOW timing.
	*/
	delete _pThreadHandle;

	POSITION pos;
	MY_L2CONF l2con;
	while ((pos = m_ListL2Config.GetHeadPosition())) {
		l2con = m_ListL2Config.GetAt(pos);
		if (l2con.HasInit)
			INIL2_close_channel(l2con.chHnd);
		m_ListL2Config.RemoveHead();
	}

	while ((pos = _pRawList.GetHeadPosition()))
		delete _pRawList.RemoveHead();

}

BOOL CCanInfo::StartThread(MY_L2CONF l2con)
{
	/* We need to ensure there is at least one available CAN channel */
	if (m_ListL2Config.IsEmpty()) {
		LOG_ERROR("No available CAN channel; cannot start thread");
		return FALSE;
	}

	PrepareForIntEvent(l2con);

	return TRUE;
}
void CCanInfo::GetDeviceType(int u32DeviceType)
{
	switch(u32DeviceType) {
	case D_CANCARD2:
		m_CurTypeName.SetString(_T("CANcard2"));
		break;
	case D_CANACPCI:
		m_CurTypeName.SetString(_T("CAN-AC PCI"));
		break;
	case D_CANACPCIDN:
		m_CurTypeName.SetString(_T("CAN-AC PCI/DN"));
		break;
	case D_CANAC104:
		m_CurTypeName.SetString(_T("CAN-AC PC/104"));
		break;
	case D_CANUSB:
		m_CurTypeName.SetString(_T("CANusb"));
		break;
	case D_EDICCARD2:
		m_CurTypeName.SetString(_T("EDICcard2"));
		break;
	case D_CANPROXPCIE:
		m_CurTypeName.SetString(_T("CANpro PCI Express"));
		break;
	case D_CANPROX104:
		m_CurTypeName.SetString(_T("CANpro PC/104plus"));
		break;
	case D_CANECO104:
		m_CurTypeName.SetString(_T("CAN-ECOx-104"));
		break;
	case D_CANFANUPC8:
		m_CurTypeName.SetString(_T("PC8 onboard CAN"));
		break;
	case D_CANPROUSB:
		m_CurTypeName.SetString(_T("CANpro USB"));
		break;
	default:
		m_CurTypeName.SetString(_T("default"));
		break;
	}
}

int CCanInfo::PrepareForIntEvent(MY_L2CONF l2con)
{
	DWORD rtnCode;

	if ((_ThreadEvent[0] = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		LOG_ERROR("_ThreadEvent[0] == NULL");
		goto _error;
	}
	if ((_ThreadEvent[1] = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		LOG_ERROR("_ThreadEvent[1] == NULL");
		goto _error;
	}

	ResetEvent(_ThreadEvent[0]);
	ResetEvent(_ThreadEvent[1]);
	
	l2con.l2conf.hEvent = _ThreadEvent[0];

	if (CANL2_initialize_fifo_mode(l2con.chHnd, &l2con.l2conf)) {
		LOG_ERROR("CANL2_initialize_fifo_mode fail");
		goto _error;
	}

	_curHandle = l2con.chHnd;
	if ((_pThreadHandle = AfxBeginThread(receiveThread, this)) == NULL) {
		LOG_ERROR("_pThreadHandle = NULL");
		goto _error;
	}

	return 0;

_error:
	for (int i = 0; i < 2; i++) {
		if (_ThreadEvent[i] != INVALID_HANDLE_VALUE)
			CloseHandle(_ThreadEvent[i]);
	}

	if (GetExitCodeThread(_pThreadHandle, &rtnCode)) {
		if (rtnCode == STILL_ACTIVE) {
			::TerminateThread(_pThreadHandle, 0);
			_pThreadHandle = NULL;
		}
	}


	return -1;
}

void CCanInfo::setRefCount()
{
	CCanRaw *_p;
	POSITION pos;

	if (_pRawList.IsEmpty())
		return;

	pos = _pRawList.GetHeadPosition();
	do {
		_p = _pRawList.GetNext(pos);
		_p->_refCount = _notedEvt.GetCount();
	} while (pos);

}

POSITION CCanInfo::RegEvent(HANDLE eV)
{
	POSITION pos;

	pos = _notedEvt.AddTail(eV);
	setRefCount();

	return pos;
}

void CCanInfo::SetEvent()
{
	POSITION pos;
	HANDLE hnd;

	if (_notedEvt.IsEmpty())
		return;

	pos = _notedEvt.GetHeadPosition();
	do {
		hnd = _notedEvt.GetNext(pos);
		::SetEvent(hnd);		
	} while(pos);
}
UINT CCanInfo::receiveThread(LPVOID pa)
{
	CCanInfo *pThis = (CCanInfo*) pa;
	PARAM_STRUCT param;
	DWORD  ret;
	int frc = CANL2_RA_NO_DATA;
	unsigned long diff_time = 0, old_time = 0;
	CCanRaw *_pR = NULL;

	POSITION pos = pThis->_pRawList.GetHeadPosition();
	if (!pos) {
		LOG_ERROR("RAW MEMEORY POOL IS NULL");
		return 1;
	}
	while(1) {
		ret = WaitForMultipleObjects(2, &pThis->_ThreadEvent[0], FALSE, INFINITE);
		switch (ret - WAIT_OBJECT_0 ) {
		case 0:
			if (!pos) {
				pos = pThis->_pRawList.GetHeadPosition();
			}
			_pR = pThis->_pRawList.GetNext(pos);
			do {
				switch (frc = CANL2_read_ac(pThis->_curHandle, &param)) {
					case CANL2_RA_DATAFRAME:
						_pR->_list.AddTail(param);
						break;
					default:
						break;
				}
			} while (frc > 0);
#ifdef _DEBUG
			pThis->SetEvent();
			Sleep(1);
#endif //_DEBUG
			break;
		case 1:
			LOG_ERROR("interruptThread exit");
			goto __exit;
			break;
		}
	}

__exit:
	return 1;
}


// canEngine.cpp : 定義 DLL 的初始化常式。
//

#include "stdafx.h"
#include "canEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//const char* CCanInfo::_dereferenceSlotName = "\\\\.\\mailslot\\wnc_canEngine_decref";
const int CCanInfo::MSG_LEN_MAX = 20;
const int CCanInfo::SHARED_MEM_BLOCK_SIZE = 256;

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
	
	for (int i = 0; i < 2; i++) {
		_ThreadEvent[i] = INVALID_HANDLE_VALUE;
		m_ptrView[i] = NULL;
		m_curOffset[i] = 0;
		m_rxMemHnd[i] = NULL;
	}

	CString szError;

	m_szName.Format(_T("SharedMem01"));
	m_iSize.QuadPart = sizeof(PARAM_STRUCT) * SHARED_MEM_BLOCK_SIZE;
	m_rxMemHnd[1] = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, m_iSize.HighPart, m_iSize.LowPart, m_szName);
	if (!m_rxMemHnd[1]) {
		szError.Format(_T("m_rxMemHnd = NULL, %d"), GetLastError());
	} else {
		m_ptrView[1] = (LPBYTE)MapViewOfFile(m_rxMemHnd[1], FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
		if (!m_ptrView[1]) {
			szError.Format(_T("m_ptrView[1] = NULL, %d"), GetLastError());
		}
	}
	_noteSlotMutex = CreateMutex(NULL, FALSE, NULL);
}
//
//const char* CCanInfo::GetDecMailslotName()
//{
//	return CCanInfo::_dereferenceSlotName;
//}

//HANDLE CCanInfo::DecrefGet()
//{
//	return _decMailslot;
//}

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

	//CloseHandle(_decMailslot);
}

//BOOL CCanInfo::StartDecThread()
//{
//	if ((_pDecrefHandle = AfxBeginThread(decrefThread, this)) == NULL) {
//		LOG_ERROR("_pDecrefHandle = NULL");
//		return FALSE;
//	}
//
//	return TRUE;
//}

BOOL CCanInfo::StartThread(MY_L2CONF l2con)
{
	/* We need to ensure there is at least one available CAN channel */
	if (m_ListL2Config.IsEmpty()) {
		LOG_ERROR("No available CAN channel; cannot start thread");
		return FALSE;
	}

	PrepareForIntEvent(l2con);

	run = TRUE;
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

void CCanInfo::TerminatedThread()
{
	run = FALSE;
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

HANDLE CCanInfo::MailslotHndGet(POSITION pos)
{
	SLOT_INFO _sl;

	WaitForSingleObject(_noteSlotMutex, INFINITE);
	for (int idx = 0; idx < _noteSlot.GetCount(); idx++) {
		if (pos == _noteSlot.FindIndex(idx)) {
			_sl = _noteSlot.GetAt(pos);
			ReleaseMutex(_noteSlotMutex);
			return _sl.slotHnd;
		}
	}
	ReleaseMutex(_noteSlotMutex);
	return INVALID_HANDLE_VALUE;
}

POSITION CCanInfo::SlotReg(CString eV, unsigned int slotKey)
{
	POSITION pos = NULL;
	SLOT_INFO _sl;

	_sl.eventHnd = INVALID_HANDLE_VALUE;
	_sl.slotHnd = INVALID_HANDLE_VALUE;
	_sl.writeHnd = INVALID_HANDLE_VALUE;

	_sl.slotHnd = CreateMailslot(eV, sizeof(SLOT_DATA), MAILSLOT_WAIT_FOREVER,(LPSECURITY_ATTRIBUTES) NULL);
	if (_sl.slotHnd == INVALID_HANDLE_VALUE)
		goto _error;

	_sl.writeHnd = CreateFile(eV, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (_sl.writeHnd == INVALID_HANDLE_VALUE)
		goto _error;
	
	_sl.eventHnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (_sl.eventHnd == INVALID_HANDLE_VALUE)
		goto _error;
	
	///* all RawList must be assign a idxKey to identify slot */
	//CCanRaw *pRaw = _pRawList.GetHead();
	//unsigned int fakeKey;
	//if (pRaw->idxKey.Lookup(slotKey, fakeKey)) {
	//	/* An new register slotKey should not be found in idxKey */
	//	goto _error;
	//}
	WaitForSingleObject(_noteSlotMutex, INFINITE);
	pos = _noteSlot.AddTail(_sl);
	ReleaseMutex(_noteSlotMutex);

	//POSITION rawPos;
	//rawPos = _pRawList.GetHeadPosition();
	//while (rawPos) {
	//	pRaw = _pRawList.GetNext(rawPos);
	//	WaitForSingleObject(pRaw->_refCountMutex, INFINITE);
	//	pRaw->idxKey.SetAt(slotKey, slotKey);
	//	ReleaseMutex(pRaw->_refCountMutex);
	//}
	return pos;

_error:
	if (_sl.eventHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.eventHnd);
	if (_sl.slotHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.slotHnd);
	if (_sl.writeHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.writeHnd);
	
	return NULL;
}

POSITION CCanInfo::SlotReg(CString eV)
{
	POSITION pos = NULL;
	SLOT_INFO _sl;

	_sl.eventHnd = INVALID_HANDLE_VALUE;
	_sl.slotHnd = INVALID_HANDLE_VALUE;
	_sl.writeHnd = INVALID_HANDLE_VALUE;

	_sl.slotHnd = CreateMailslot(eV, sizeof(SLOT_DATA), MAILSLOT_WAIT_FOREVER,(LPSECURITY_ATTRIBUTES) NULL);
	if (_sl.slotHnd == INVALID_HANDLE_VALUE)
		goto _error;

	_sl.writeHnd = CreateFile(eV, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (_sl.writeHnd == INVALID_HANDLE_VALUE)
		goto _error;
	
	_sl.eventHnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (_sl.eventHnd == INVALID_HANDLE_VALUE)
		goto _error;
	WaitForSingleObject(_noteSlotMutex, INFINITE);
	pos = _noteSlot.AddTail(_sl);
	ReleaseMutex(_noteSlotMutex);

	return pos;

_error:
	if (_sl.eventHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.eventHnd);
	if (_sl.slotHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.slotHnd);
	if (_sl.writeHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.writeHnd);
	
	return NULL;
}

HANDLE CCanInfo::InforEventGet(POSITION pos)
{
	SLOT_INFO _sl;

	for (int idx = 0; idx < _noteSlot.GetCount(); idx++) {
		if (pos == _noteSlot.FindIndex(idx)) {
			_sl = _noteSlot.GetAt(pos);
			return _sl.eventHnd;
		}
	}
	
	return INVALID_HANDLE_VALUE;


}

BOOL CCanInfo::FindNextPool(CCanRaw **_p, POSITION *pPos)
{
	CCanRaw *p;
	POSITION pos;

	pos = _curRawListPos;

	do {
		p = _pRawList.GetNext(pos);
 		if (p->GetRefCount() == 0) {
			WaitForSingleObject(p->_listMutex, INFINITE);
			p->_list.RemoveAll();
			ReleaseMutex(p->_listMutex);
			break;
		}
		if (pos == NULL) {
			pos = _pRawList.GetHeadPosition();
		}
		if (pos == _curRawListPos) {
			*pPos = NULL;
			return FALSE;
		}
	} while (1);

	if (pos == NULL) {
		_curRawListPos =_pRawList.GetHeadPosition();
		*pPos = _curRawListPos;
	} else {
		_curRawListPos = pos;
		_pRawList.GetPrev(pos);
		*pPos = pos;
	}

	*_p = p;
	return TRUE;
}

CList <PARAM_STRUCT, PARAM_STRUCT&>* CCanInfo::ReadRawList(POSITION pos)
{
	CCanRaw *pR;

	pR = _pRawList.GetAt(pos);
	return &(pR->_list);
}

void CCanInfo::DecRefCount(POSITION _pos)
{
	CCanRaw *pR;

	pR = _pRawList.GetAt(_pos);
	pR->DecRefCount();
}
void CCanInfo::SlotInfo()
{
	if (_noteSlot.IsEmpty()) {
		/* No any thread registed, need to unmap m_ptrView[0] itself */
		if (m_ptrView[0]) {
			UnmapViewOfFile(m_ptrView[0]);
			m_ptrView[0] = NULL;
			CloseHandle(m_rxMemHnd[0]);
		}
		return;
	}

	POSITION pos;
	DWORD cbWriteCount;
	pos = _noteSlot.GetHeadPosition();
	SLOT_INFO _sl;

	SLOT_DATA slotData;
//	SLOT_DATA *pSlot;
	slotData.len = static_cast<int>(m_curOffset[0]);

	WaitForSingleObject(_noteSlotMutex, INFINITE);
	while (pos) {
		_sl = _noteSlot.GetNext(pos);		
		if (_sl.writeHnd == INVALID_HANDLE_VALUE) {
			LOG_ERROR("SlotInfo error");
		} else {
			slotData.ptr = (LPBYTE)MapViewOfFile(m_rxMemHnd[0], FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
			WriteFile(_sl.writeHnd, &slotData, sizeof(slotData), &cbWriteCount, NULL);
			SetEvent(_sl.eventHnd);
		}
	}
	ReleaseMutex(_noteSlotMutex);
	if (m_ptrView[0]) {
		UnmapViewOfFile(m_ptrView[0]);
		m_ptrView[0] = NULL;
		CloseHandle(m_rxMemHnd[0]);
	}
}

void CCanInfo::SlotInfo(POSITION _pos)
{
	if (_noteSlot.IsEmpty())
		return;

	POSITION pos;
	DWORD cbWriteCount;
	pos = _noteSlot.GetHeadPosition();
	SLOT_INFO _sl;
	POSITION *_pPos;
	CCanRaw *pRaw;

	pRaw = _pRawList.GetAt(_pos);
	WaitForSingleObject(_noteSlotMutex, INFINITE);
	pRaw->SetRefCount(_noteSlot.GetCount());
	while (pos) {
		_sl = _noteSlot.GetNext(pos);		
		if (_sl.writeHnd == INVALID_HANDLE_VALUE) {
			LOG_ERROR("SlotInfo error");
		} else {
			_pPos = &_pos;
			WriteFile(_sl.writeHnd, _pPos, sizeof(_pPos), &cbWriteCount, NULL);
			SetEvent(_sl.eventHnd);
		}
	}
	ReleaseMutex(_noteSlotMutex);
}
void CCanInfo::SlotDereg(POSITION pos)
{
	SlotDereg(pos, 0);
}

void CCanInfo::SlotDereg(POSITION pos, unsigned int slotKey)
{
	SLOT_INFO _sl;
	_sl = _noteSlot.GetAt(pos);
	CloseHandle(_sl.eventHnd);
	CloseHandle(_sl.slotHnd);
	CloseHandle(_sl.writeHnd);
	WaitForSingleObject(_noteSlotMutex, INFINITE);
	_noteSlot.RemoveAt(pos);
	ReleaseMutex(_noteSlotMutex);

#if 0
	POSITION rawPos;
	CCanRaw *pRaw;
	rawPos = _pRawList.GetHeadPosition();
	unsigned int fakeKey;
	while (rawPos) {
		pRaw = _pRawList.GetNext(rawPos);
		if (pRaw->idxKey.Lookup(slotKey, fakeKey)) {
			WaitForSingleObject(pRaw->_refCountMutex, INFINITE);
			pRaw->idxKey.RemoveKey(slotKey);
			if (pRaw->_refCount != 0)
				pRaw->_refCount--;
			ReleaseMutex(pRaw->_refCountMutex);
		}
	}
	pRaw = NULL;
#endif
}

//UINT CCanInfo::decrefThread(LPVOID pa)
//{
//	CCanInfo *_this = (CCanInfo*) pa;
//	HANDLE decMailslot = INVALID_HANDLE_VALUE;
//	POSITION pos;
//	DWORD	cbRead;
//	
//	while(_this->run) {
//		ReadFile(_this->_decMailslot, &pos, sizeof(pos), &cbRead, NULL);
//		_this->DecRefCount(pos);
//	}
//	return 0;
//}

BOOL CCanInfo::swapMem()
{
	CString szError;
	
	if ((m_curOffset[1] + sizeof(PARAM_STRUCT)) <= (sizeof(PARAM_STRUCT) * SHARED_MEM_BLOCK_SIZE)) {
		return FALSE;
	}

	FlushViewOfFile(m_ptrView[1], sizeof(PARAM_STRUCT) * SHARED_MEM_BLOCK_SIZE);
	m_rxMemHnd[0] = m_rxMemHnd[1];

	if (m_ptrView[0]) {
		UnmapViewOfFile(m_ptrView[0]);
		m_ptrView[0] = NULL;
	}
	m_ptrView[0] = m_ptrView[1];

	m_rxMemHnd[1] = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, m_iSize.HighPart, m_iSize.LowPart, m_szName);
	if (!m_rxMemHnd[1]) {
		szError.Format(_T("m_rxMemHnd = NULL, %d"), GetLastError());
		AfxMessageBox(szError);
	} else {
		m_ptrView[1] = (LPBYTE)MapViewOfFile(m_rxMemHnd[1], FILE_MAP_READ|FILE_MAP_WRITE, 0, 0, 0);
		if (!m_ptrView[1]) {
			szError.Format(_T("m_ptrView = NULL, %d"), GetLastError());
			AfxMessageBox(szError);
		}
	}
	m_curOffset[0] = m_curOffset[1];
	m_curOffset[1] = 0;

	/*	Arrive here, m_ptrView[0] points to the data collected, 
		even if the new handle was not created successfully */
	return TRUE;
}

UINT CCanInfo::receiveThread(LPVOID pa)
{
	CCanInfo *pThis = (CCanInfo*) pa;
	PARAM_STRUCT param;
	DWORD  ret;
	int frc = CANL2_RA_NO_DATA;
	unsigned long diff_time = 0, old_time = 0;

	while(pThis->run) {
		ret = WaitForMultipleObjects(2, &pThis->_ThreadEvent[0], FALSE, INFINITE);
		switch (ret - WAIT_OBJECT_0 ) {
		case 0:
			do {
				switch (frc = CANL2_read_ac(pThis->_curHandle, &param)) {
					case CANL2_RA_DATAFRAME:
						/* If true, need to inform all of threads */
						if (pThis->swapMem()) {
							pThis->SlotInfo();
						}
						CopyMemory(pThis->m_ptrView[1] + pThis->m_curOffset[1], &param, sizeof(PARAM_STRUCT));
						pThis->m_curOffset[1] += sizeof(PARAM_STRUCT);
						break;
					default:
						break;
				}
				if (!pThis->run)
					break;
			} while (frc > 0);
			//pThis->SlotInfo();
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


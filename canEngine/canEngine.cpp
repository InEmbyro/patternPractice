// canEngine.cpp : �w�q DLL ����l�Ʊ`���C
//

#include "stdafx.h"
#include "canEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const int CCanInfo::MSG_LEN_MAX = 20;
//
//TODO: �p�G�o�� DLL �O�ʺA�a�� MFC DLL �s���A
//		����q�o�� DLL �ץX������|�I�s
//		MFC �������禡�A�������b�禡�@�}�Y�[�W AFX_MANAGE_STATE
//		�����C
//
//		�Ҧp:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// ���B�����`�禡�D��
//		}
//
//		�o�ӥ����@�w�n�X�{�b�C�@��
//		�禡���A�~����I�s MFC �������C�o�N����
//		�������O�禡�����Ĥ@�ӳ��z��
//		�A�Ʀܥ����b���󪫥��ܼƫŧi�e��
//		�A�]�����̪��غc�禡�i��|���͹� MFC
//		DLL �������I�s�C
//
//		�аѾ\ MFC �޳N���� 33 �M 58 ����
//		�ԲӸ�ơC
//

// CcanEngineApp



BEGIN_MESSAGE_MAP(CcanEngineApp, CWinApp)
END_MESSAGE_MAP()


// CcanEngineApp �غc

CcanEngineApp::CcanEngineApp()
{
	// TODO: �b���[�J�غc�{���X�A
	// �N�Ҧ����n����l�]�w�[�J InitInstance ��
}

CcanEngineApp::~CcanEngineApp()
{

}


// �Ȧ����@�� CcanEngineApp ����

CcanEngineApp theApp;

// CcanEngineApp ��l�]�w
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
			_pRawList.AddHead(_p);
		}
	}
	if (!_pRawList.IsEmpty())
		_curRawListPos = _pRawList.GetHeadPosition();
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

HANDLE CCanInfo::MailslotHndGet(POSITION pos)
{
	SLOT_INFO _sl;

	for (int idx = 0; idx < _noteSlot.GetCount(); idx++) {
		if (pos == _noteSlot.FindIndex(idx)) {
			_sl = _noteSlot.GetAt(pos);
			return _sl.slotHnd;
		}
	}
	
	return INVALID_HANDLE_VALUE;
}

POSITION CCanInfo::SlotReg(CString eV)
{
	POSITION pos = NULL;
	SLOT_INFO _sl;

	_sl.eventHnd = INVALID_HANDLE_VALUE;
	_sl.slotHnd = INVALID_HANDLE_VALUE;
	_sl.writeHnd = INVALID_HANDLE_VALUE;

	_sl.slotHnd = CreateMailslot(eV, MSG_LEN_MAX, MAILSLOT_WAIT_FOREVER,(LPSECURITY_ATTRIBUTES) NULL);
	if (_sl.slotHnd == INVALID_HANDLE_VALUE)
		goto _error;

	_sl.writeHnd = CreateFile(eV, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (_sl.writeHnd == INVALID_HANDLE_VALUE)
		goto _error;
	
	_sl.eventHnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (_sl.eventHnd == INVALID_HANDLE_VALUE)
		goto _error;
	
	pos = _noteSlot.AddTail(_sl);
	
	return pos;

_error:
	if (_sl.eventHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.eventHnd);
	if (_sl.slotHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.slotHnd);
	if (_sl.writeHnd != INVALID_HANDLE_VALUE)
		CloseHandle(_sl.writeHnd);
	
	return pos;
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

BOOL CCanInfo::FindNextPool(CCanRaw **_p)
{
	CCanRaw *p;
	POSITION pos;

	pos = _curRawListPos;

	do {
		p = _pRawList.GetNext(pos);
		if (p->GetRefCount() == 0) {
			p->_list.RemoveAll();
			break;
		}
		if (pos == NULL) {
			pos = _pRawList.GetHeadPosition();
		}
		if (pos == _curRawListPos)
			return FALSE;
	} while (1);

	if (pos == NULL)
		_curRawListPos =_pRawList.GetHeadPosition();
	else
		_curRawListPos = pos;

	*_p = p;
	return TRUE;
}

void CCanInfo::SlotInfo(CCanRaw* _p)
{
	if (_noteSlot.IsEmpty())
		return;

	POSITION pos;
	DWORD cbWriteCount;
	pos = _noteSlot.GetHeadPosition();
	SLOT_INFO _sl;

	while (pos) {
		_sl = _noteSlot.GetNext(pos);		
		if (_sl.writeHnd == INVALID_HANDLE_VALUE) {
			LOG_ERROR("SlotInfo error");
		} else {
			WriteFile(_sl.writeHnd, _p, sizeof(_p), &cbWriteCount, NULL);
			SetEvent(_sl.eventHnd);
		}
	}
}

UINT CCanInfo::receiveThread(LPVOID pa)
{
	CCanInfo *pThis = (CCanInfo*) pa;
	PARAM_STRUCT param;
	DWORD  ret;
	int frc = CANL2_RA_NO_DATA;
	unsigned long diff_time = 0, old_time = 0;
	CCanRaw *_pR = NULL;

	while(1) {
		ret = WaitForMultipleObjects(2, &pThis->_ThreadEvent[0], FALSE, INFINITE);
		switch (ret - WAIT_OBJECT_0 ) {
		case 0:
			if (pThis->FindNextPool(&_pR) == FALSE) {
				LOG_ERROR("RAW MEMEORY POOL IS NULL");
				break;
			}	
			_pR->SetRefCount(pThis->_noteSlot.GetCount());
			do {
				switch (frc = CANL2_read_ac(pThis->_curHandle, &param)) {
					case CANL2_RA_DATAFRAME:
						_pR->_list.AddTail(param);
						break;
					default:
						break;
				}
			} while (frc > 0);
			pThis->SlotInfo(_pR);

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


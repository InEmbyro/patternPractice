
#include "stdafx.h"
#include "CReceiveThread.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "../canEngine/canEngine_def.h"
#include "../canEngine/canEngineApi.h"
#include "targetList.h"

CReceiveThread::CReceiveThread()
	:_pThread(NULL), run(FALSE), _mailslotHnd(INVALID_HANDLE_VALUE), _pView(NULL)
{
	_infoHnd[0] = INVALID_HANDLE_VALUE;
	_infoHnd[1] = INVALID_HANDLE_VALUE;
}

CReceiveThread::~CReceiveThread()
{
	if (_pThread) {
		delete _pThread;
		_pThread = NULL;
	}
}

BOOL CReceiveThread::InitThread()
{
	_pThread = AfxBeginThread(update_thread, this);
	if (_pThread == NULL) {
		AfxMessageBox(_T("m_pThreadHandle == NULL"));
		return FALSE;
	}
	run = TRUE;
	_infoHnd[1] = CreateEvent(NULL, FALSE, FALSE, NULL);
	_confirmHnd = CreateEvent(NULL, FALSE, FALSE, NULL);
	return TRUE;
}

void CReceiveThread::SetInfoHandle(HANDLE hnd)
{
	_infoHnd[0] = hnd;
}

void CReceiveThread::SetMailHandle(HANDLE hnd)
{
	_mailslotHnd = hnd;
}

HANDLE CReceiveThread::getConfirmHnd()
{
	return _confirmHnd;
}

void CReceiveThread::SetEventTerminateHnd()
{
	SetEvent(_infoHnd[1]);
}
void CReceiveThread::TerminateThread()
{
	SetRunFalse();
	SetEventTerminateHnd();
}

void CReceiveThread::SetRunFalse()
{
	run = FALSE;
}

CWinThread* CReceiveThread::GetThread()
{
	return _pThread;
}

void CReceiveThread::SetDecMailslotName(const char* name)
{
	decmailslotName = name;
}

void CReceiveThread::InforDec(POSITION _pos)
{
	DWORD cbWriteCount;

	WriteFile(_decmailslotHnd, &_pos, sizeof(_pos), &cbWriteCount, NULL);
}

UINT CReceiveThread::update_thread(LPVOID _p)
{
	CReceiveThread* _this = (CReceiveThread*) _p;
	DWORD ret;
	DWORD cbRead;
	CTargetList *pView = NULL;
	CList <PARAM_STRUCT, PARAM_STRUCT&>* _pList;
	CList <PARAM_STRUCT, PARAM_STRUCT&>* _pViewList;
	int idx = 0;

	BOOL fResult; 
	DWORD cbMessage;
	DWORD cMessage;
	SLOT_DATA slotData;
	int dataLen;
	unsigned long fakeKey;
	PARAM_STRUCT data;
	RAW_OBJECT_STRUCT rawData;


	pView = (CTargetList*)_this->_pView;
	while (_this->run) {
		ret = WaitForMultipleObjects(2, _this->_infoHnd, FALSE, INFINITE);
		switch (ret - WAIT_OBJECT_0) {
		case 0:
			fResult = GetMailslotInfo(_this->_mailslotHnd, // mailslot handle 
				(LPDWORD) NULL,               // no maximum message size 
				&cbMessage,                   // size of next message 
				&cMessage,                    // number of messages 
				(LPDWORD) NULL);              // no read time-out 

			if (fResult) {
				if (cbMessage == MAILSLOT_NO_MESSAGE)
					continue;
			}
__next_read:
			fResult = ReadFile(_this->_mailslotHnd, &slotData, sizeof(SLOT_DATA), &cbRead, NULL);
			if (fResult) {
				dataLen = 0;
				while (dataLen != slotData.len) {
					CopyMemory(&data, slotData.ptr + dataLen, sizeof(PARAM_STRUCT));
					dataLen += sizeof(PARAM_STRUCT);
					if (pView->_filterMap.Lookup(data.Ident, fakeKey)) {
						if (data.Ident > 0x44F) {
							pView->ParseTrackingObject(&data, &rawData);
						} else {
							pView->ParseRawObject(&data, &rawData);
						}
						WaitForSingleObject(pView->_listStoreMutex, INFINITE);
						pView->_listStore->AddTail(rawData);
						if (rawData.TargetNum == 0) {
							WaitForSingleObject(pView->_listMutex, INFINITE);
							delete pView->_list;
							pView->_list = pView->_listStore;
							pView->_listStore = new CList <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&>;
							ReleaseMutex(pView->_listMutex);
						}
						ReleaseMutex(pView->_listStoreMutex);
					}
				}
				UnmapViewOfFile(slotData.ptr);
				if (!_this->run) {
					pView->SendMessage(WM_USER_DRAW, 0, 0);
				}
			}
			if (--cMessage)
				goto __next_read;
			pView->SendMessage(WM_USER_DRAW, 0, 0);
			break;
		default:
		case 1:
			_this->run = FALSE;
			break;
		}
	}

_exit:
	SetEvent(_this->_confirmHnd);
	/*	When the thread exits, the related pointer (_pThread) will be released by framework.
		the member of _pThread will be meaningless and should be set to NULL to avoid unpected deleting */
	_this->_pThread = NULL;
	return 1;
}


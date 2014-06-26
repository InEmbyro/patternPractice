
#include "stdafx.h"
#include "CGridFormThread.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "../canEngine/canEngine_def.h"
#include "../canEngine/canEngineApi.h"
#include "formview_dll.h"

CGridFormThread::CGridFormThread()
	:_pThread(NULL), run(FALSE), _mailslotHnd(INVALID_HANDLE_VALUE), _pView(NULL)
{
	_infoHnd[0] = INVALID_HANDLE_VALUE;
	_infoHnd[1] = INVALID_HANDLE_VALUE;
}

CGridFormThread::~CGridFormThread()
{
	if (_pThread) {
		delete _pThread;
		_pThread = NULL;
	}
}

BOOL CGridFormThread::InitThread()
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

void CGridFormThread::SetInfoHandle(HANDLE hnd)
{
	_infoHnd[0] = hnd;
}

void CGridFormThread::SetMailHandle(HANDLE hnd)
{
	_mailslotHnd = hnd;
}

HANDLE CGridFormThread::getConfirmHnd()
{
	return _confirmHnd;
}

void CGridFormThread::SetEventTerminateHnd()
{
	SetEvent(_infoHnd[1]);
}
void CGridFormThread::TerminateThread()
{
	SetRunFalse();
	SetEventTerminateHnd();
}

void CGridFormThread::SetRunFalse()
{
	run = FALSE;
}

CWinThread* CGridFormThread::GetThread()
{
	return _pThread;
}


void CGridFormThread::SetDecMailslotName(const char* name)
{
	decmailslotName = name;
}

void CGridFormThread::InforDec(POSITION _pos)
{
	DWORD cbWriteCount;

	WriteFile(_decmailslotHnd, &_pos, sizeof(_pos), &cbWriteCount, NULL);

}

UINT CGridFormThread::update_thread(LPVOID _p)
{
	CGridFormThread* _this = (CGridFormThread*) _p;
	DWORD ret;
	DWORD cbRead;
	GridFormChildView *pView = NULL;

	BOOL fResult; 
	DWORD cbMessage;
	DWORD cMessage;
	SLOT_DATA slotData;
	int dataLen;
	PARAM_STRUCT data;

	pView = (GridFormChildView*)_this->_pView;
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
				WaitForSingleObject(pView->_ListMutex, INFINITE);
				while (dataLen != slotData.len) {
					CopyMemory(&data, slotData.ptr + dataLen, sizeof(PARAM_STRUCT));
					dataLen += sizeof(PARAM_STRUCT);
					pView->_List.AddTail(data);
					if (!_this->run) {
						UnmapViewOfFile(slotData.ptr);
						pView->SendMessage(WM_USER_DRAW, 0, 0);
						goto _exit;
					}
				}
				ReleaseMutex(pView->_ListMutex);
				UnmapViewOfFile(slotData.ptr);
				pView->PostMessage(WM_USER_DRAW, 0, 0);
			}
			if (--cMessage)
				goto __next_read;
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

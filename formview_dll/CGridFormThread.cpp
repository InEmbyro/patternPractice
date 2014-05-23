
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
	LPVOID _pVoid;
	POSITION *_pPos;
	POSITION dPos;
	PARAM_STRUCT data;
	GridFormChildView *pView = NULL;
	CList <PARAM_STRUCT, PARAM_STRUCT&>* _pList;
	_this->_decmailslotHnd = CreateFile(_this->decmailslotName, GENERIC_WRITE, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, 
		FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);

	pView = (GridFormChildView*)_this->_pView;
	while (_this->run) {
		ret = WaitForMultipleObjects(2, _this->_infoHnd, FALSE, INFINITE);
		switch (ret - WAIT_OBJECT_0) {
		case 0:
			ReadFile(_this->_mailslotHnd, &_pVoid, sizeof(POSITION), &cbRead, NULL);
			_pPos = (POSITION*)_pVoid;
			_pList = (CList <PARAM_STRUCT, PARAM_STRUCT&>*)ReadRawList(_pPos);
			dPos = _pList->GetHeadPosition();
			WaitForSingleObject(pView->_ListMutex, INFINITE);
			while (dPos) {
				data = _pList->GetNext(dPos);
				pView->_List.AddTail(data);
				if (!_this->run) {
					_this->InforDec(*_pPos);
					ReleaseMutex(pView->_ListMutex);
					pView->SendMessage(WM_USER_DRAW, 0, 0);
					goto _exit;
				}
			}
			_this->InforDec(*_pPos);
			ReleaseMutex(pView->_ListMutex);
			pView->PostMessage(WM_USER_DRAW, 0, 0);
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


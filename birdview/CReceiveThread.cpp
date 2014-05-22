
#include "stdafx.h"
#include "CReceiveThread.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "../canEngine/canEngine_def.h"
#include "../canEngine/canEngineApi.h"
#include "birdview.h"

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

UINT CReceiveThread::update_thread(LPVOID _p)
{
	CReceiveThread* _this = (CReceiveThread*) _p;
	DWORD ret;
	DWORD cbRead;
	LPVOID _pVoid;
	POSITION *_pPos;
	POSITION dPos;
	PARAM_STRUCT data;
	CBirdviewView *pView = NULL;
	CList <PARAM_STRUCT, PARAM_STRUCT&>* _pList;
	CString str;
	unsigned long fakeKey;

	pView = (CBirdviewView*)_this->_pView;
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
				if (pView->_ReceiveMap.Lookup(data.Ident, fakeKey))
					pView->_List.AddTail(data);
				if (!_this->run) {
					DecRefCount(_pPos);
					ReleaseMutex(pView->_ListMutex);
					pView->SendMessage(WM_USER_DRAW, 0, 0);
					goto _exit;
				}
			}
			ReleaseMutex(pView->_ListMutex);
			pView->PostMessage(WM_USER_DRAW, 0, 0);
			DecRefCount(_pPos);
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


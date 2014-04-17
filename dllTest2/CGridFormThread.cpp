
#include "stdafx.h"
#include "CGridFormThread.h"
#include "../canEngine/canEngine_def.h"
#include "../canEngine/CCanRaw.h"

CGridFormThread::CGridFormThread()
	:_pThread(NULL), run(FALSE), _mailslotHnd(INVALID_HANDLE_VALUE)
{
	_infoHnd[0] = INVALID_HANDLE_VALUE;
	_infoHnd[1] = INVALID_HANDLE_VALUE;
}

CGridFormThread::~CGridFormThread()
{
	SetEvent(_infoHnd[1]);
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

UINT CGridFormThread::update_thread(LPVOID _p)
{
	CGridFormThread* _this = (CGridFormThread*) _p;
	DWORD ret;
	DWORD cbRead;
	LPVOID _pRaw;
	CCanRaw *pRaw;

	while (_this->run) {
		ret = WaitForMultipleObjects(2, _this->_infoHnd, FALSE, INFINITE);
		switch (ret - WAIT_OBJECT_0) {
		case 0:
			ReadFile(_this->_mailslotHnd, &_pRaw, sizeof(CCanRaw), &cbRead, NULL);
			pRaw = (CCanRaw*)_pRaw;
			break;
		case 1:
			_this->run = FALSE;
			break;
		}
	}

	return 1;
}


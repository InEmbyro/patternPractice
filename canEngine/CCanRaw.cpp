
#include "stdafx.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "CCanRaw.h"
#include "canEngine_def.h"

CCanRaw::CCanRaw()
	:_refCountMutex(INVALID_HANDLE_VALUE), _listMutex(INVALID_HANDLE_VALUE)
{
	_refCountMutex = CreateMutex(NULL, FALSE, NULL);
	_listMutex = CreateMutex(NULL, FALSE, NULL);

	SetRefCount(0);
	idxKey.InitHashTable(100);
}
CCanRaw::~CCanRaw()
{
#if 1
	if (_refCountMutex != INVALID_HANDLE_VALUE)
		CloseHandle(_refCountMutex);
	if (_listMutex != INVALID_HANDLE_VALUE)
		CloseHandle(_listMutex);
#endif
}

CCanRaw& CCanRaw::operator=(const CCanRaw& other) 
{
	return *this;
};

void CCanRaw::DecRefCount()
{
	DWORD rtn;

	if (_refCountMutex == INVALID_HANDLE_VALUE)
		return;

	rtn = WaitForSingleObject(_refCountMutex, INFINITE);
	switch(rtn - WAIT_OBJECT_0) {
	case 0:
		if (_refCount == 0)
			_refCount = 0;
		else
			_refCount--;
		break;
	case WAIT_TIMEOUT:
		LOG_ERROR("Reference Count MUTEX Fail");
		break;
	}
	ReleaseMutex(_refCountMutex);
}

unsigned int CCanRaw::GetRefCount()
{
	return _refCount;
}

void CCanRaw::ListRemoveAll()
{
	WaitForSingleObject(_listMutex, INFINITE);
	_list.RemoveAll();
	ReleaseMutex(_listMutex);
}

void CCanRaw::ListAddTail(PARAM_STRUCT *param)
{
	PARAM_STRUCT data;

	WaitForSingleObject(_listMutex, INFINITE);
	data = *param;
	_list.AddTail(data);
	ReleaseMutex(_listMutex);
}

BOOL CCanRaw::SetRefCount(unsigned int refCount)
{
	DWORD rtn;

	if (_refCountMutex == INVALID_HANDLE_VALUE)
		return FALSE;

	rtn = WaitForSingleObject(_refCountMutex, INFINITE);
	switch(rtn - WAIT_OBJECT_0) {
	case 0:
		_refCount = refCount;
		break;
	case WAIT_TIMEOUT:
		LOG_ERROR("Reference Count MUTEX Fail");
		break;
	}
	ReleaseMutex(_refCountMutex);

	return TRUE;
}

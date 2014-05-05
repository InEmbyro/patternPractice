
#include "stdafx.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "CCanRaw.h"
#include "canEngine_def.h"

CCanRaw::CCanRaw()
	:_refCountMutex(INVALID_HANDLE_VALUE)
{
	_refCountMutex = CreateMutex(NULL, FALSE, NULL);
	SetRefCount(0);
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
		ReleaseMutex(_refCountMutex);
		break;
	case WAIT_TIMEOUT:
		LOG_ERROR("Reference Count MUTEX Fail");
		break;
	}
}

unsigned int CCanRaw::GetRefCount()
{
	return _refCount;
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
		ReleaseMutex(_refCountMutex);
		break;
	case WAIT_TIMEOUT:
		LOG_ERROR("Reference Count MUTEX Fail");
		break;
	}

	return TRUE;
}
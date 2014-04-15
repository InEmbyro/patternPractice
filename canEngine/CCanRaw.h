#pragma once

#include <afxmt.h>
#include <afxtempl.h>
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"


class CCanRaw {

	unsigned int _refCount;

public:
	CCanRaw();
	HANDLE _refCountMutex;

	CList <PARAM_STRUCT, PARAM_STRUCT&> _list;
	BOOL SetRefCount(unsigned int refCount);
	unsigned int GetRefCount(void);

	CCanRaw& operator=(const CCanRaw& other);
};

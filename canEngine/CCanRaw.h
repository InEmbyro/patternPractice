#pragma once

#include <afxmt.h>
#include <afxtempl.h>
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"


class CCanRaw {

private:

public:
	CCanRaw();
	~CCanRaw();
	HANDLE _refCountMutex;
	HANDLE _listMutex;
	unsigned int _refCount;

	CList <PARAM_STRUCT, PARAM_STRUCT&> _list;
	CMap  <unsigned int, unsigned int, unsigned int, unsigned int&> idxKey;
	BOOL SetRefCount(unsigned int refCount);
	void DecRefCount(void);
	unsigned int GetRefCount(void);

	CCanRaw& operator=(const CCanRaw& other);
	void ListAddTail(PARAM_STRUCT*);
	void ListRemoveAll(void);
};

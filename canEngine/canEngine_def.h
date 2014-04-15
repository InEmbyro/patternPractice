#pragma once

#include <afxmt.h>
#include <afxtempl.h>
#include "../Softing/CANL2.H"

class CCanRaw {

public:
	CList <PARAM_STRUCT, PARAM_STRUCT&> _list;
	unsigned int _refCount;

	CCanRaw& operator=(const CCanRaw& other) {
		this->_refCount = other._refCount;
		return *this;
	};
};

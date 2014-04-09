#include "stdafx.h"
#include "canEngineApi.h"

DLLEXPORT void WINAPI StringTest(CString* name)
{
	if (name)
		name->Insert(0, _T("Hello "));
}

DLLEXPORT BOOL WINAPI InitCan()
{
	return TRUE;
}

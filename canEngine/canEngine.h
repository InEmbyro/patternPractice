// canEngine.h : canEngine DLL ���D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#ifdef _EXPORTING
   #define DllImport	__declspec(dllexport)
#else
   #define DllImport	__declspec(dllimport)
#endif

#ifndef _EXPORTING
#include "resource.h"		// �D�n�Ÿ�
#endif //_EXPORTING

// CcanEngineApp
// �o�����O����@�аѾ\ canEngine.cpp
//

DllImport int InitCan(void);
DllImport void StringTest(CString*);

class CcanEngineApp : public CWinApp
{
public:
	CcanEngineApp();

// �мg
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

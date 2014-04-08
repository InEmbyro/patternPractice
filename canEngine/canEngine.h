// canEngine.h : canEngine DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#ifdef _EXPORTING
   #define DllImport	__declspec(dllexport)
#else
   #define DllImport	__declspec(dllimport)
#endif

#ifndef _EXPORTING
#include "resource.h"		// 主要符號
#endif //_EXPORTING

// CcanEngineApp
// 這個類別的實作請參閱 canEngine.cpp
//

DllImport int InitCan(void);
DllImport void StringTest(CString*);

class CcanEngineApp : public CWinApp
{
public:
	CcanEngineApp();

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

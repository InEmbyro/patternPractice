
// dllTest.h : dllTest 應用程式的主標頭檔
//
#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "resource.h"       // 主要符號


// CdllTestApp:
// 請參閱實作此類別的 dllTest.cpp
//

class CdllTestApp : public CWinAppEx
{
public:
	CdllTestApp();


// 覆寫
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 程式碼實作

public:
	UINT  m_nAppLook;
	BOOL  m_bHiColorIcons;
	HANDLE m_Event;

	virtual void PreLoadState();
	virtual void LoadCustomState();
	virtual void SaveCustomState();

	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
	afx_msg void OnTerminate();
};

extern CdllTestApp theApp;

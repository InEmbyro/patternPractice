
// dllTest.h : dllTest ���ε{�����D���Y��
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "resource.h"       // �D�n�Ÿ�


// CdllTestApp:
// �аѾ\��@�����O�� dllTest.cpp
//

class CdllTestApp : public CWinAppEx
{
public:
	CdllTestApp();


// �мg
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// �{���X��@

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

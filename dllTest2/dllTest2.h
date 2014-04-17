
// dllTest2.h : main header file for the dllTest2 application
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CdllTest2App:
// See dllTest2.cpp for the implementation of this class
//

class CdllTest2App : public CWinApp
{
public:
	CdllTest2App();
	~CdllTest2App();


// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// Implementation
protected:
	HMENU  m_hMDIMenu;
	HACCEL m_hMDIAccel;

public:
	afx_msg void OnAppAbout();
	afx_msg void OnFileNew();
	DECLARE_MESSAGE_MAP()
};

extern CdllTest2App theApp;

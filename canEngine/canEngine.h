// canEngine.h : canEngine DLL ���D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

// CcanEngineApp
// �o�����O����@�аѾ\ canEngine.cpp
//
class CcanEngineApp : public CWinApp
{
public:
	CcanEngineApp();

// �мg
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

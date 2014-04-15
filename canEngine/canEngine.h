// canEngine.h : canEngine DLL ���D�n���Y��
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�� PCH �]�t���ɮ׫e���]�t 'stdafx.h'"
#endif

#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "CCanRaw.h"
#include "canEngine_def.h"



// CcanEngineApp
// �o�����O����@�аѾ\ canEngine.cpp
//

typedef struct {
	L2CONFIG l2conf;
	CAN_HANDLE chHnd;
	CString chName;
	BOOL HasInit;
} MY_L2CONF;

class CCanInfo
{
	static UINT receiveThread(LPVOID);

	void	SetEvent(void);
	int				PrepareForIntEvent(MY_L2CONF);
	HANDLE			_ThreadEvent[2];
	CWinThread*		_pThreadHandle;
	CAN_HANDLE		_curHandle;
	CList <HANDLE, HANDLE&> _notedEvt;
	BOOL		FindNextPool(CCanRaw **_p);
	POSITION	_curRawListPos;

public:
	CCanInfo();
	~CCanInfo();
	void		GetDeviceType(int u32DeviceType);
	POSITION	RegEvent(HANDLE eV);
	BOOL		StartThread(MY_L2CONF);

	int m_CanChNo;

	/* Pointer */
	LPVOID m_pBuf;
	CList <MY_L2CONF, MY_L2CONF> m_ListL2Config;
	CString m_CurTypeName;
	CList <CCanRaw*, CCanRaw*> _pRawList;
};

class CcanEngineApp : public CWinApp
{
public:
	CcanEngineApp();
	~CcanEngineApp();

// �мg
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

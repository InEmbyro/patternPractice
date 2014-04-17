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
	void		SlotInfo(CCanRaw*);
	BOOL		FindNextPool(CCanRaw **_p);
	int			PrepareForIntEvent(MY_L2CONF);

	HANDLE			_ThreadEvent[2];
	CWinThread*		_pThreadHandle;
	CAN_HANDLE		_curHandle;
	CList <SLOT_INFO, SLOT_INFO&> _noteSlot;
	POSITION	_curRawListPos;

public:
	CCanInfo();
	~CCanInfo();

	static const int MSG_LEN_MAX;
	void		GetDeviceType(int u32DeviceType);
	POSITION	SlotReg(CString slotName);
	BOOL		StartThread(MY_L2CONF);
	HANDLE		MailslotHndGet(POSITION);
	HANDLE		InforEventGet(POSITION);

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

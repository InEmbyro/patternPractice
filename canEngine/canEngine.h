// canEngine.h : canEngine DLL 的主要標頭檔
//

#pragma once

#ifndef __AFXWIN_H__
	#error "對 PCH 包含此檔案前先包含 'stdafx.h'"
#endif

#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "CCanRaw.h"
#include "canEngine_def.h"

// CcanEngineApp
// 這個類別的實作請參閱 canEngine.cpp
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
	static UINT decrefThread(LPVOID);
	static const int SHARED_MEM_BLOCK_SIZE;

	void		SlotInfo(POSITION);
	void		SlotInfo();
	BOOL		FindNextPool(CCanRaw **_p, POSITION *pPos);
	int			PrepareForIntEvent(MY_L2CONF);

	HANDLE			_ThreadEvent[2];
	CWinThread*		_pThreadHandle;
	CWinThread*		_pDecrefHandle;
	CAN_HANDLE		_curHandle;
	CList <SLOT_INFO, SLOT_INFO&> _noteSlot;
	POSITION	_curRawListPos;
	BOOL		run;

	// For share memory
	BOOL	swapMem();
	HANDLE		m_rxMemHnd[2];
	LPBYTE		m_ptrView[2];
	ULONGLONG	m_curOffset[2];
	CString		m_szName;
	ULARGE_INTEGER m_iSize;

public:
	//static const char* _dereferenceSlotName;
	HANDLE	_noteSlotMutex;
	CCanInfo();
	~CCanInfo();

	static const int MSG_LEN_MAX;
	void		GetDeviceType(int u32DeviceType);
	POSITION	SlotReg(CString slotName, unsigned int slotKey);
	POSITION	SlotReg(CString slotName);

	void		SlotDereg(POSITION, unsigned int slotKey);
	void		SlotDereg(POSITION);

	BOOL		StartThread(MY_L2CONF);
	//BOOL		StartDecThread(void);
	HANDLE		MailslotHndGet(POSITION);
	HANDLE		InforEventGet(POSITION);
	//HANDLE		DecrefGet(void);
	void		DecRefCount(POSITION);
	void		TerminatedThread();
	CList <PARAM_STRUCT, PARAM_STRUCT&>	*ReadRawList(POSITION);
	//const char*	GetDecMailslotName(void);

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

// 覆寫
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

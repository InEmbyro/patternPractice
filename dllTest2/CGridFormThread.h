#pragma once

#include "stdafx.h"

class CGridFormThread {

public:
	CGridFormThread();
	~CGridFormThread();

	BOOL	InitThread(void);
	void	SetInfoHandle(HANDLE);
	void	TerminateThread(void);
	void	SetMailHandle(HANDLE);
	LPVOID	_pView;	//pointer related FormView
	HANDLE	getConfirmHnd(void);
	CWinThread* GetThread(void);

private:
	void	SetEventTerminateHnd(void);
	void	SetRunFalse(void);
	static UINT update_thread(LPVOID);
	CWinThread* _pThread;
	BOOL	run;
	HANDLE	_mailslotHnd;
	HANDLE	_infoHnd[2];	//0 for receive, 1 for being terminated
	HANDLE	_confirmHnd;

};

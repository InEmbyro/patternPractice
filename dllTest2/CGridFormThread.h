#pragma once

#include "stdafx.h"

class CGridFormThread {

public:
	CGridFormThread();
	~CGridFormThread();

	BOOL	InitThread(void);
	void	SetInfoHandle(HANDLE);
	void	SetMailHandle(HANDLE);
	LPVOID	_pView;	//pointer related FormView

private:
	static UINT update_thread(LPVOID);
	CWinThread* _pThread;
	BOOL	run;
	HANDLE	_mailslotHnd;
	HANDLE	_infoHnd[2];	//0 for receive, 1 for being terminated

};

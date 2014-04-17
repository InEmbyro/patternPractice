#pragma once

class CCanRaw;

#if 0
#define LOG_ERROR(A)	AfxMessageBox(_T(A))
#else
#define LOG_ERROR(A)
#endif

typedef struct {
	HANDLE slotHnd;
	HANDLE writeHnd;
	HANDLE eventHnd;
}SLOT_INFO;

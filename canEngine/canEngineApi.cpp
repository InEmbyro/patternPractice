#include "stdafx.h"
#include "canEngine.h"
#include "canEngineApi.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"

CCanInfo canInfo;

DLLEXPORT void WINAPI StringTest(CString* name)
{
	if (name)
		name->Insert(0, _T("Hello "));
}
DLLEXPORT int WINAPI GetCanNo()
{
	return canInfo.CanChNo;
}

DLLEXPORT BOOL WINAPI InitCan()
{
	int ret;
	unsigned long u32NeededBufferSize;
	unsigned long u32ProvidedBufferSize;
	unsigned long u32NumOfChannels;
	PCHDSNAPSHOT pBuffer = NULL;

	ret = CANL2_get_all_CAN_channels(0, &u32NeededBufferSize, &u32NumOfChannels, NULL);
	if (ret)
		return FALSE;

	canInfo.CanChNo = u32NumOfChannels;
	if (!u32NumOfChannels) {
		return FALSE;
	}

	pBuffer = (PCHDSNAPSHOT) malloc(u32NeededBufferSize);
	if (!pBuffer)
		return FALSE;

	u32ProvidedBufferSize = u32NeededBufferSize;
	ret = CANL2_get_all_CAN_channels(u32ProvidedBufferSize, &u32NeededBufferSize, &u32NumOfChannels, pBuffer);
	if (ret) {
		free(pBuffer);
		return FALSE;
	}

	free(pBuffer);
	return TRUE;
}

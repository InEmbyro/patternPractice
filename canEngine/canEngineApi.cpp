#include "stdafx.h"
#include "canEngine.h"
#include "canEngineApi.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"

CCanInfo canInfo;

DLLEXPORT HANDLE WINAPI InforEventAcquire(POSITION pos)
{
	return canInfo.InforEventGet(pos);
}
DLLEXPORT HANDLE WINAPI MailSlotAcquire(POSITION pos)
{
	return canInfo.MailslotHndGet(pos);
}

DLLEXPORT POSITION WINAPI RegisterAcquire(CString slotName)
{
	return canInfo.SlotReg(slotName);
}

DLLEXPORT HANDLE WINAPI GetTerminalHnd()
{
	return INVALID_HANDLE_VALUE;
}

DLLEXPORT void WINAPI StringTest(CString* name)
{
	if (name)
		name->Insert(0, _T("Hello "));
}
DLLEXPORT int WINAPI GetCanNo()
{
	return canInfo.m_CanChNo;
}

DLLEXPORT BOOL WINAPI InitCan()
{
	int ret;
	unsigned long u32NeededBufferSize;
	unsigned long u32ProvidedBufferSize;
	unsigned long u32NumOfChannels;

	if (canInfo.m_pBuf != NULL) {
		return FALSE;
	}

	ret = CANL2_get_all_CAN_channels(0, &u32NeededBufferSize, &u32NumOfChannels, NULL);
	if (ret)
		return FALSE;

	canInfo.m_CanChNo = u32NumOfChannels;
	if (!u32NumOfChannels) {
		return FALSE;
	}

	canInfo.m_pBuf = (PCHDSNAPSHOT) malloc(u32NeededBufferSize);
	if (!canInfo.m_pBuf)
		return FALSE;

	u32ProvidedBufferSize = u32NeededBufferSize;
	ret = CANL2_get_all_CAN_channels(u32ProvidedBufferSize, &u32NeededBufferSize, &u32NumOfChannels, (PCHDSNAPSHOT)canInfo.m_pBuf);
	if (ret) {
		return FALSE;
	}

	if (u32NumOfChannels == 0) {
		return TRUE;
	}

	MY_L2CONF l2Con;
	for (unsigned short int idx = 0; idx < u32NumOfChannels; idx++) {
		l2Con.l2conf.bEnableAck = GET_FROM_SCIM;
		l2Con.l2conf.bEnableErrorframe = GET_FROM_SCIM;
		l2Con.l2conf.s32AccCodeStd = GET_FROM_SCIM;
		l2Con.l2conf.s32AccCodeXtd = GET_FROM_SCIM;
		l2Con.l2conf.s32AccMaskStd = GET_FROM_SCIM;
		l2Con.l2conf.s32AccMaskXtd = GET_FROM_SCIM;
		l2Con.l2conf.s32OutputCtrl = GET_FROM_SCIM;
		l2Con.l2conf.s32Prescaler = GET_FROM_SCIM;
		l2Con.l2conf.s32Sam = GET_FROM_SCIM;
		l2Con.l2conf.s32Sjw = GET_FROM_SCIM;
		l2Con.l2conf.s32Tseg1 = GET_FROM_SCIM;
		l2Con.l2conf.s32Tseg2 = GET_FROM_SCIM;

		CHDSNAPSHOT *pCh = (CHDSNAPSHOT*)canInfo.m_pBuf + idx;
		l2Con.chName = pCh->ChannelName;
		l2Con.HasInit = FALSE;
		if (INIL2_initialize_channel(&l2Con.chHnd, pCh->ChannelName) == 0) {
			l2Con.HasInit = TRUE;
		}
		canInfo.m_ListL2Config.AddTail(l2Con);
	}

#if 1
	/*	Currently, we only use the first CAN channel */
	if (!canInfo.m_ListL2Config.IsEmpty()) {
		l2Con = canInfo.m_ListL2Config.GetHead();
		if (l2Con.HasInit) {
			if (canInfo.StartThread(l2Con) == FALSE) {
				LOG_ERROR("StartThread == FALSE");
			}
		} else {
			LOG_ERROR("CANL2_initialize_fifo_mode l2Con.HasInit = FALSE");
		}
	}
#else
	POSITION pos = canInfo.m_ListL2Config.GetHeadPosition();
	while (pos) {
		l2Con = canInfo.m_ListL2Config.GetAt(pos);
		if (l2Con.HasInit == TRUE) {
			if (CANL2_initialize_fifo_mode(l2Con.chHnd, &l2Con.l2conf)) {
				LOG_ERROR("CANL2_initialize_fifo_mode fail");
			}
		}

		canInfo.m_ListL2Config.GetNext(pos);
	}
#endif


	return TRUE;
}

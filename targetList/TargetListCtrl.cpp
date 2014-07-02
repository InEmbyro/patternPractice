// TargetListCtrl.cpp : 實作檔
//

#include "stdafx.h"
#include "TargetListCtrl.h"


// CTargetListCtrl

IMPLEMENT_DYNAMIC(CTargetListCtrl, CListCtrl)

CTargetListCtrl::CTargetListCtrl()
{

}

CTargetListCtrl::~CTargetListCtrl()
{
}


BEGIN_MESSAGE_MAP(CTargetListCtrl, CListCtrl)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CTargetListCtrl::OnNMCustomdraw)
END_MESSAGE_MAP()



// CTargetListCtrl 訊息處理常式




void CTargetListCtrl::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVCUSTOMDRAW *pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pNMCD->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	{
		int rowNumber = pNMCD->nmcd.dwItemSpec;
		COLORREF backgroundColor;
		if ((rowNumber & 0x01)) {
			backgroundColor = RGB(100, 100, 100);
		} else {
			backgroundColor = RGB(0, 0, 0);
		}
		pNMCD->clrText = RGB(255, 255, 255);
		pNMCD->clrTextBk = backgroundColor;
	}
		break;
	default:
		break;
    }}

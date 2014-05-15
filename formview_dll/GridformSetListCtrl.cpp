// GridformSetListCtrl.cpp : 實作檔
//

#include "stdafx.h"
#include "formview_dll.h"
#include "GridformSetListCtrl.h"


// CGridformSetListCtrl

IMPLEMENT_DYNAMIC(CGridformSetListCtrl, CListCtrl)

CGridformSetListCtrl::CGridformSetListCtrl()
{

}

CGridformSetListCtrl::~CGridformSetListCtrl()
{
}


BEGIN_MESSAGE_MAP(CGridformSetListCtrl, CListCtrl)
	ON_NOTIFY(HDN_BEGINTRACKA, 0, &CGridformSetListCtrl::OnHdnBegintrack)
	ON_NOTIFY(HDN_BEGINTRACKW, 0, &CGridformSetListCtrl::OnHdnBegintrack)
END_MESSAGE_MAP()



// CGridformSetListCtrl 訊息處理常式




void CGridformSetListCtrl::OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	*pResult = TRUE;
}

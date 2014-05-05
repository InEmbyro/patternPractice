// GridList.cpp : 實作檔
//

#include "stdafx.h"
#include "formview_dll.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "GridList.h"


// CGridList

IMPLEMENT_DYNAMIC(CGridList, CLinkCtrl)
CMap<unsigned int, unsigned int, WPARAM_STRUCT, WPARAM_STRUCT&> *CGridList::_pMap;

CGridList::CGridList()
{

}

CGridList::~CGridList()
{
}


BEGIN_MESSAGE_MAP(CGridList, CLinkCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CGridList::OnLvnGetdispinfo)
END_MESSAGE_MAP()



// CGridList 訊息處理常式


int CALLBACK CGridList::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	unsigned long p1 = (unsigned long)lParam1;
	unsigned long p2 = (unsigned long)lParam2;
	int rtn = 0;

	if (p1 > p2)
		rtn = 1;
	else if (p1 < p2)
		rtn = -1;

	return rtn;
}



void CGridList::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	WPARAM_STRUCT pkt;
	unsigned long Ident;
	CString str;

	if (pDispInfo->item.mask & LVIF_TEXT) {
		Ident = (unsigned long)pDispInfo->item.lParam;
		_pMap->Lookup(Ident, pkt);
		switch (pDispInfo->item.iSubItem) {
		case 0:
			str.Format(_T("%d"), pkt.counter);
			break;
#if 1
		case 1:
			str.Format(_T("%X"), pkt.param.Ident);
			break;
		case 2:
			str.Format(_T("%d"), pkt.param.DataLength);
			break;
		case 3:
			str = "";
			for (int j = pkt.param.DataLength - 1; j >= 0; j--)
				str.AppendFormat(_T("%02X:"), pkt.param.RCV_data[j]);
			str.TrimRight(_T(":"));
			break;
#endif
		default:
			str = "";
			break;
		}
	}
	
	::lstrcpy(pDispInfo->item.pszText, str);
	*pResult = 0;
}

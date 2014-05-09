// GridList.cpp : ��@��
//

#include "stdafx.h"
#include "formview_dll.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"
#include "GridList.h"


// CGridList

IMPLEMENT_DYNAMIC(CGridList, CLinkCtrl)
CMap<unsigned int, unsigned int, WPARAM_STRUCT, WPARAM_STRUCT&> *CGridList::_pMap;
CArray <WPARAM_STRUCT, WPARAM_STRUCT&> *CGridList::_pArray;

CGridList::CGridList()
{

}

CGridList::~CGridList()
{
}


BEGIN_MESSAGE_MAP(CGridList, CLinkCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, &CGridList::OnLvnGetdispinfo)
	ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, &CGridList::OnNMCustomdraw)
	ON_WM_ERASEBKGND()
	ON_NOTIFY(HDN_ENDTRACKA, 0, &CGridList::OnHdnEndtrack)
	ON_NOTIFY(HDN_ENDTRACKW, 0, &CGridList::OnHdnEndtrack)
END_MESSAGE_MAP()



// CGridList �T���B�z�`��


//int CALLBACK CGridList::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
//{
//	unsigned long p1 = (unsigned long)lParam1;
//	unsigned long p2 = (unsigned long)lParam2;
//	int rtn = 0;
//
//	if (p1 > p2)
//		rtn = 1;
//	else if (p1 < p2)
//		rtn = -1;
//
//	return rtn;
//}



void CGridList::OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult)
{
	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
	// TODO: �b���[�J����i���B�z�`���{���X
	WPARAM_STRUCT pkt;
	int Ident;
	CString str;

	memset(&pkt, 0x00, sizeof(WPARAM_STRUCT));
	if (pDispInfo->item.mask & LVIF_TEXT) {
		Ident = (unsigned long)pDispInfo->item.iItem;
		if (Ident >= _pArray->GetSize())
			goto __error;
		pkt = _pArray->GetAt(Ident);
		switch (pDispInfo->item.iSubItem) {
		case 0:
			str.Format(_T("%d"), pkt.counter);
			break;
		case 1:
			str.Format(_T("%2X"), pkt.param.Ident);
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
		default:
			str = "";
			break;
		}
	}
__error:	
	::lstrcpy(pDispInfo->item.pszText, str);
	*pResult = 0;
}


void CGridList::OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult)
{
	//LPNMCUSTOMDRAW pNMCD = reinterpret_cast<LPNMCUSTOMDRAW>(pNMHDR);
	// TODO: �b���[�J����i���B�z�`���{���X
	NMLVCUSTOMDRAW *pNMCD = reinterpret_cast<LPNMLVCUSTOMDRAW>(pNMHDR);
	*pResult = CDRF_DODEFAULT;

	switch (pNMCD->nmcd.dwDrawStage) {
	case CDDS_PREPAINT:
		*pResult = CDRF_NOTIFYITEMDRAW;
		break;
	case CDDS_ITEMPREPAINT:
	{
		int rowNumber = pNMCD->nmcd.dwItemSpec;
		if ((rowNumber & 0x01)) {
			COLORREF backgroundColor;
			backgroundColor = RGB(219, 220, 175);
			pNMCD->clrTextBk = backgroundColor;
		}
	}
		break;
	default:
		break;
    }
}

BOOL CGridList::OnEraseBkgnd(CDC* pDC)
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	return FALSE;
	return CListCtrl::OnEraseBkgnd(pDC);
}

void CGridList::PreSubclassWindow()
{

	// TODO: �b���[�J�S�w���{���X�M (��) �I�s�����O
	CString str;
	str = _T("Courier New");
                                      
	LOGFONT lf;
	_curFont = GetFont();
	_curFont->GetLogFont(&lf);

	lf.lfCharSet = ANSI_CHARSET;
	wcscpy_s(lf.lfFaceName, str);
	font_.CreateFontIndirect(&lf);
	SetFont(&font_);
	CListCtrl::PreSubclassWindow();
}


void CGridList::OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	*pResult = 0;
}


//void CGridList::OnSize(UINT nType, int cx, int cy)
//{
//	CListCtrl::OnSize(nType, cx, cy);
//
//	// TODO: �b���[�J�z���T���B�z�`���{���X
//}

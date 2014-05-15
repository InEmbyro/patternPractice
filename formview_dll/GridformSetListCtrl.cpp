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
	ON_WM_LBUTTONUP()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CGridformSetListCtrl 訊息處理常式




void CGridformSetListCtrl::OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	*pResult = TRUE;
}



void CGridformSetListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	LVHITTESTINFO info;
	CRect rect;

	memset((void*)&info, 0x00, sizeof(LVHITTESTINFO));
	info.pt = point;
	ListView_HitTest(m_hWnd, &info);
	
	if (info.iItem != -1) {
		GetClientRect(&rect);
		if (point.x >= (rect.Width() / 2))
			info.iSubItem = 1;
		else
			info.iSubItem = 0;
	}
	CListCtrl::OnLButtonDown(nFlags, point);

	GetItemRect(info.iItem, &rect, LVIR_BOUNDS);

	if (info.iSubItem == 1) {
		rect.left = GetColumnWidth(info.iSubItem);
	}
	DWORD style = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
	rect.bottom = rect.bottom + (rect.Height() * 5);
	m_box.Create(style, rect, this, 0);
	m_box.AddString(_T("01"));
	m_box.AddString(_T("02"));
	m_box.AddString(_T("03"));


}


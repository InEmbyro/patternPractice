// GridformSetListCtrl.cpp : 實作檔
//

#include "stdafx.h"
#include "formview_dll.h"
#include "GridformSetListCtrl.h"


// CGridformSetListCtrl

IMPLEMENT_DYNAMIC(CGridformSetListCtrl, CListCtrl)

CGridformSetListCtrl::CGridformSetListCtrl()
	:m_contentIdx(DISPLAY_MODE)
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
	ON_MESSAGE(WM_CONFIG_UPDATE, &CGridformSetListCtrl::OnConfigUpdate)
	ON_MESSAGE(WM_CONFIG_GET_SEL, &CGridformSetListCtrl::OnConfigGetSel)
	ON_WM_SETFOCUS()
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

	if (m_box.m_hWnd != NULL)
		m_box.SendMessage(WM_CLOSE);
	if (m_edit.m_hWnd != NULL)
		m_edit.SendMessage(WM_CLOSE);

	memset((void*)&info, 0x00, sizeof(LVHITTESTINFO));
	info.pt = point;
	ListView_HitTest(m_hWnd, &info);
	
	if (info.iItem != -1) {
		GetClientRect(&rect);
		if (point.x >= (rect.Width() / 2))
			info.iSubItem = 1;
		else {
			CListCtrl::OnLButtonDown(nFlags, point);
			return;
		}
	} else {
		CListCtrl::OnLButtonDown(nFlags, point);
		return;
	}
	CListCtrl::OnLButtonDown(nFlags, point);

	GetItemRect(info.iItem, &rect, LVIR_BOUNDS);

	if (info.iSubItem == 1) {
		rect.left = GetColumnWidth(info.iSubItem);
	}
	rect.bottom = rect.bottom + (rect.Height() * 5);

	COMBODRAWINFO drawinfo;
	drawinfo.pParent = this;
	drawinfo.rect = rect;
	drawinfo.item = info.iItem;
	drawinfo.subitem;
	drawinfo.conIdx = m_contentIdx;
	if (!m_box.ShowInPlaceCombo(&drawinfo)) {
		m_edit.ShowInPlaceEdit(&drawinfo);
	}
}

void CGridformSetListCtrl::CloseShownComboBox()
{
	if (m_box.m_hWnd)
		m_box.SendMessage(WM_CLOSE);
}

void CGridformSetListCtrl::ShowListContent(LIST_CONTENT idx)
{
	CString str;
	int j = 0;

	DeleteAllItems();
	m_contentIdx = idx;

	switch (idx) {
	case DISPLAY_MODE:
		str.LoadString(IDS_DISPLAYMODE);
		InsertItem(0, str);
		DISPLAY_MODE_SET info;
		GetParent()->SendMessage(WM_CONFIG_GET_SEL, (WPARAM)DISPLAY_MODE, (LPARAM)&info);
		if (info.mode == 0) {
			str.SetString(_T("Buffer Mode"));
		} else if (info.mode == 1) {
			str.SetString(_T("Ident Mode"));
		} else {
			str = "";
		}
		SetItemText(0, 1, str);

		str.LoadString(IDS_NUMBER_OF_GRID_ROWS);
		InsertItem(1, str);
		str.Format(_T("%d"), info.rows);
		SetItemText(1, 1, str);
		break;
	case FILTER:
		str.LoadString(IDS_FILTER_ACTIVE);
		InsertItem(j, str);
		SetItemText(j, 1, _T("No (example)"));

		j++;
		str.LoadString(IDS_FILTER_IDENTIFIER);
		InsertItem(j, str);
		SetItemText(j, 1, _T(" (example)"));

		j++;
		str.LoadString(IDS_FILTER_MODE);
		InsertItem(j, str);
		SetItemText(j, 1, _T("Show only filtered (example)"));
		break;
	default:
		m_contentIdx = DISPLAY_MODE;
		break;
	}
}


afx_msg LRESULT CGridformSetListCtrl::OnConfigUpdate(WPARAM wParam, LPARAM lParam)
{
	COMBOBOX_CONTENT c = (COMBOBOX_CONTENT) wParam;

	switch (c)	{
	case C_DISPLAY_MODE:
		GetParent()->SendMessage(WM_DISPLAY_MODE, 0, lParam);
		break;
	case C_DISPLAY_ROWS:
		GetParent()->SendMessage(WM_DISPLAY_ROWS, 0, lParam);
		break;
	default:
		break;
	}
	return 0;
}


afx_msg LRESULT CGridformSetListCtrl::OnConfigGetSel(WPARAM wParam, LPARAM lParam)
{
	return GetParent()->SendMessage(WM_CONFIG_GET_SEL, wParam, lParam);
}


void CGridformSetListCtrl::OnSetFocus(CWnd* pOldWnd)
{
	CListCtrl::OnSetFocus(pOldWnd);

	ShowListContent(m_contentIdx);
	// TODO: 在此加入您的訊息處理常式程式碼
}

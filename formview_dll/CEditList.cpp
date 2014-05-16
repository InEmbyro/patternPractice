// CEditList.cpp : 實作檔
//

#include "stdafx.h"
#include "formview_dll.h"
#include "CEditList.h"


// CCEditList

IMPLEMENT_DYNAMIC(CCEditList, CEdit)

CCEditList::CCEditList()
{

}

CCEditList::~CCEditList()
{
}


BEGIN_MESSAGE_MAP(CCEditList, CEdit)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

BOOL CCEditList::ShowInPlaceEdit(LPEDITDRAWINFO pInfo)
{
	DWORD style = ES_AUTOHSCROLL | ES_AUTOVSCROLL | ES_LEFT | ES_NOHIDESEL | ES_NUMBER;
	CString str;
	CFont *pFont = NULL;

	if (pInfo->subitem == 0)
		return FALSE;

	str = "";
	switch (pInfo->conIdx) {
	case DISPLAY_MODE:
		if (pInfo->item != 1)
			return FALSE;
		m_Content = C_DISPLAY_ROWS;
		Create(style, pInfo->rect, pInfo->pParent, 0);
		DISPLAY_MODE_SET info;
		GetParent()->SendMessage(WM_CONFIG_GET_SEL, (WPARAM)DISPLAY_MODE, (LPARAM)&info);
		str.Format(_T("%d"), info.rows);

		pFont = GetParent()->GetFont();
		SetFont(pFont);
		ShowWindow(SW_SHOW);
		SetSel(0, -1);
		SetFocus();
		SetWindowText(str);
		break;
	case FILTER:
	default:
		return FALSE;
		break;
	}
	return TRUE;
}

// CCEditList 訊息處理常式




void CCEditList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CCEditList::PreTranslateMessage(MSG* pMsg)
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別
	if (WM_KEYDOWN == pMsg->message && VK_RETURN == pMsg->wParam) {
		GetParent()->SetFocus();
		return TRUE;
	}

	return CEdit::PreTranslateMessage(pMsg);
}


void CCEditList::OnKillFocus(CWnd* pNewWnd)
{
	CEdit::OnKillFocus(pNewWnd);
	CString str;
	GetWindowText(str);

	int i = _wtoi(str);
	GetParent()->SendMessage(WM_CONFIG_UPDATE, (WPARAM)m_Content, (LPARAM)i);
	GetParent()->SetFocus();


	PostMessage(WM_CLOSE);
	// TODO: 在此加入您的訊息處理常式程式碼
}

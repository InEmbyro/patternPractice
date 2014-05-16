// ComboBoxList.cpp : ��@��
//

#include "stdafx.h"
#include "formview_dll.h"
#include "ComboBoxList.h"

// CComboBoxList

IMPLEMENT_DYNAMIC(CComboBoxList, CComboBox)

CComboBoxList::CComboBoxList()
	:m_comboContent(C_COMBO_UNDEF)
{

}

CComboBoxList::~CComboBoxList()
{
}


BEGIN_MESSAGE_MAP(CComboBoxList, CComboBox)
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, &CComboBoxList::OnCbnCloseup)
END_MESSAGE_MAP()



// CComboBoxList �T���B�z�`��
void CComboBoxList::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	PostMessage(WM_CLOSE);
	// TODO: �b���[�J�z���T���B�z�`���{���X
}

void CComboBoxList::OnCbnCloseup()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	GetParent()->SendMessage(WM_CONFIG_UPDATE, (WPARAM)m_comboContent, (LPARAM)GetCurSel());
	GetParent()->SetFocus();
}

void CComboBoxList::ShowInPlaceCombo(LPCOMBODRAWINFO pInfo)
{
	DWORD style = WS_BORDER | WS_CHILD | WS_VISIBLE | ES_AUTOHSCROLL | ES_AUTOVSCROLL | CBS_DROPDOWNLIST | CBS_DISABLENOSCROLL;
	int idx = -1;

	if (pInfo->subitem == 0)
		return;

	m_ItemList.RemoveAll();
	switch (pInfo->conIdx) {
	case DISPLAY_MODE:
		m_ItemList.AddTail(_T("Buffer Mode"));
		m_ItemList.AddTail(_T("Ident Mode"));
		m_comboContent = C_DISPLAY_MODE;
		break;
	case FILTER:
	default:
		return;
	}

	Create(style, pInfo->rect, pInfo->pParent, 0);
	CString str;
	while (!m_ItemList.IsEmpty()) {
		str = m_ItemList.GetHead();
		AddString(str);
		m_ItemList.RemoveHead();
	}
	GetParent()->SendMessage(WM_CONFIG_GET_SEL, (WPARAM)m_comboContent, (LPARAM)&idx);
	SetCurSel(idx);
}

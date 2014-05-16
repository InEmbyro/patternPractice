#pragma once

#include "confDef.h"

// CComboBoxList

class CComboBoxList : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxList)

public:
	CComboBoxList();
	virtual ~CComboBoxList();
	void	ShowInPlaceCombo(LPCOMBODRAWINFO);

	CStringList m_ItemList;

protected:
	DECLARE_MESSAGE_MAP()
	COMBOBOX_CONTENT m_comboContent;
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCbnCloseup();
};



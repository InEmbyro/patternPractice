#pragma once


// CComboBoxList

class CComboBoxList : public CComboBox
{
	DECLARE_DYNAMIC(CComboBoxList)

public:
	CComboBoxList();
	virtual ~CComboBoxList();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
//	afx_msg void OnClose();
	afx_msg void OnCbnCloseup();
};



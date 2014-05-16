#pragma once

#include "confDef.h"
#include "ComboBoxList.h"
#include "CEditList.h"

// CGridformSetListCtrl

class CGridformSetListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CGridformSetListCtrl)

public:
	CGridformSetListCtrl();
	virtual ~CGridformSetListCtrl();
	void ShowListContent(LIST_CONTENT idx);
	void CloseShownComboBox(void);
	void SetContentIdx(LIST_CONTENT idx) { m_contentIdx = idx; }
	LIST_CONTENT GetContentIdx() {return m_contentIdx;}

private:
	LIST_CONTENT m_contentIdx;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	CComboBoxList m_box;
	CCEditList m_edit;

protected:
	afx_msg LRESULT OnConfigUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConfigGetSel(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};



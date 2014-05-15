#pragma once

#include "ComboBoxList.h"

// CGridformSetListCtrl

class CGridformSetListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CGridformSetListCtrl)

public:
	CGridformSetListCtrl();
	virtual ~CGridformSetListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHdnBegintrack(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);

	CComboBoxList m_box;
};



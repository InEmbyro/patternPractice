#pragma once


// CTargetListCtrl

class CTargetListCtrl : public CListCtrl
{
	DECLARE_DYNAMIC(CTargetListCtrl)

public:
	CTargetListCtrl();
	virtual ~CTargetListCtrl();

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
};



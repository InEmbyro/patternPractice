#pragma once


// CGridList

class CGridList : public CListCtrl
{
	DECLARE_DYNAMIC(CGridList)

public:
	static int CALLBACK CGridList::CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static CMap<unsigned int, unsigned int, WPARAM_STRUCT, WPARAM_STRUCT&> *_pMap;
	CGridList();
	virtual ~CGridList();
	CFont font_;
	CFont *_curFont;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnLvnGetdispinfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMCustomdraw(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	virtual void PreSubclassWindow();
	afx_msg void OnHdnEndtrack(NMHDR *pNMHDR, LRESULT *pResult);
};



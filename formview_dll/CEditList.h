#pragma once


// CCEditList

class CCEditList : public CEdit
{
	DECLARE_DYNAMIC(CCEditList)

public:
	CCEditList();
	virtual ~CCEditList();
	BOOL ShowInPlaceEdit(LPEDITDRAWINFO);
	EDITBOX_CONTENT m_Content;

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};



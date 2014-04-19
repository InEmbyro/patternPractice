#pragma once

#include "CGridFormChildView.h"
// CGridFormChildFrm 框架

class CGridFormChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CGridFormChildFrm)
protected:
	CGridFormChildFrm();           // 動態建立所使用的保護建構函式
	virtual ~CGridFormChildFrm();

public:
	GridFormChildView *_pView;
	POSITION	rawPos;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
	afx_msg void OnDestroy();
};



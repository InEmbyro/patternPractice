#pragma once

#include "CGridFormChildView.h"
// CGridFormChildFrm �ج[

class CGridFormChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CGridFormChildFrm)
protected:
	CGridFormChildFrm();           // �ʺA�إߩҨϥΪ��O�@�غc�禡
	virtual ~CGridFormChildFrm();

public:
	GridFormChildView *_pView;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};



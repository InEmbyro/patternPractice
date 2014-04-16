// CGridFormChildFrm.cpp : ��@��
//

#include "stdafx.h"
#include "dllTest2.h"
#include "CGridFormChildFrm.h"


// CGridFormChildFrm

IMPLEMENT_DYNCREATE(CGridFormChildFrm, CMDIChildWnd)

CGridFormChildFrm::CGridFormChildFrm()
{

}

CGridFormChildFrm::~CGridFormChildFrm()
{
}


BEGIN_MESSAGE_MAP(CGridFormChildFrm, CMDIChildWnd)
	ON_WM_CREATE()
END_MESSAGE_MAP()


// CGridFormChildFrm �T���B�z�`��


int CGridFormChildFrm::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIChildWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �b���[�J�S�O�إߪ��{���X
	_pView = new GridFormChildView();
	CRect rect;
	rect.SetRect(0, 0, lpCreateStruct->cx, lpCreateStruct->cy);
	_pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW, rect, this, AFX_IDW_PANE_FIRST, NULL);
	
	/*	Calling UpdateDate(FALSE) will cause GridFormChildView::DoDataExchange is called.
		In the function, GridFormChildView::m_hWnd will be assigned */
	_pView->UpdateData(FALSE);

	return 0;
}

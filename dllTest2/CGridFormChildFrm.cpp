// CGridFormChildFrm.cpp : ��@��
//

#include "stdafx.h"
#include "dllTest2.h"
#include "CGridFormChildFrm.h"
#include "CGridFormThread.h"
#include "../canEngine/canEngineApi.h"

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
	ON_WM_CLOSE()
	ON_WM_DESTROY()
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

void CGridFormChildFrm::OnClose()
{
	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
	//ShowWindow(SW_HIDE);
	DeregisterAcquire(rawPos);
	_pView->CloseAllHnd();
	_pView->_pThread->TerminateThread();
	WaitForSingleObject(_pView->_pThread->getConfirmHnd(), 5000);

	Sleep(10);
	CMDIChildWnd::OnClose();
}


void CGridFormChildFrm::OnDestroy()
{
	CMDIChildWnd::OnDestroy();

	// TODO: �b���[�J�z���T���B�z�`���{���X
}

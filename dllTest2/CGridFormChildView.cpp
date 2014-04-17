// CGridFormChildView.cpp : 實作檔
//

#include "stdafx.h"
#include "dllTest2.h"
#include "CGridFormChildView.h"


// GridFormChildView
IMPLEMENT_DYNCREATE(GridFormChildView, CFormView)

GridFormChildView::GridFormChildView()
	: CFormView(GridFormChildView::IDD), _pThread(NULL)
{

}

GridFormChildView::~GridFormChildView()
{
	delete _pThread;
}

void GridFormChildView::OnInitialUpdate()
{
	int idx = 0;
	if (m_GridList.GetSafeHwnd() != NULL) {
		m_GridList.InsertColumn(idx++, _T("#Msgs"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Ident"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Len"), LVCFMT_LEFT, 50, 0);
		m_GridList.InsertColumn(idx++, _T("Data Bytes [7...0]"), LVCFMT_LEFT, 200, 0);
	}
}

void GridFormChildView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_GRIDLIST, m_GridList);
}

BOOL GridFormChildView::Create(LPCTSTR p1, LPCTSTR p2, DWORD p3, const RECT& p4, CWnd* p5, UINT p6, CCreateContext*p7)
{
	return CFormView::Create(p1, p2, p3, p4, p5, p6, p7);
}

BEGIN_MESSAGE_MAP(GridFormChildView, CFormView)
END_MESSAGE_MAP()


// GridFormChildView 診斷

#ifdef _DEBUG
void GridFormChildView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void GridFormChildView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// GridFormChildView 訊息處理常式

// CGridFormChildView.cpp : 實作檔
//

#include "stdafx.h"
#include "dllTest2.h"
#include "CGridFormChildView.h"
#include "CGridFormThread.h"


// GridFormChildView
IMPLEMENT_DYNCREATE(GridFormChildView, CFormView)

GridFormChildView::GridFormChildView()
	: CFormView(GridFormChildView::IDD), _pThread(NULL)
{
	_counter = 0;
	_ListMutex = CreateMutex(NULL, FALSE, NULL);
}

GridFormChildView::~GridFormChildView()
{
	CloseHandle(_ListMutex);
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
//	ON_WM_CLOSE()
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_DRAW, &GridFormChildView::OnUserDraw)
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

void GridFormChildView::CloseAllHnd()
{
	::SendMessage(m_GridList.m_hWnd, WM_CLOSE, 0, 0);
}

void GridFormChildView::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值

	CFormView::OnClose();
}


afx_msg LRESULT GridFormChildView::OnUserDraw(WPARAM wParam, LPARAM lParam)
{
	if (GetSafeHwnd() == NULL)
		return 0;
	if (_List.IsEmpty())
		return 0;

	POSITION pos;
	PARAM_STRUCT data;
	CString str;
	int idx = 0;

	pos = _List.GetHeadPosition();
	idx = 0;
	if (_counter == (sizeof(_counter)-1))
		_counter = 0;
	while (pos) {
		data = _List.GetNext(pos);
		if (m_GridList.GetItemCount() > 10)
			m_GridList.DeleteItem(10);
		str.Format(_T("%d"), _counter++);
		m_GridList.InsertItem(idx, str);
		str.Format(_T("0x%X"), data.Ident);
		m_GridList.SetItemText(idx, 1, str);
		str.Format(_T("%d"), data.DataLength);
		m_GridList.SetItemText(idx, 2, str);
	}
	WaitForSingleObject(_ListMutex, INFINITE);
	_List.RemoveAll();
	ReleaseMutex(_ListMutex);
	return 0;
}

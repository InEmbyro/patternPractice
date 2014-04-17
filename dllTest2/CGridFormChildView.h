#pragma once
#include "afxcmn.h"
#include "resource.h"


// GridFormChildView ����˵�
class CGridFormThread;

class GridFormChildView : public CFormView
{
	DECLARE_DYNCREATE(GridFormChildView)

	GridFormChildView();           // �ʺA�إߩҨϥΪ��O�@�غc�禡
	virtual ~GridFormChildView();

public:
	enum { IDD = IDD_GRIDFORMCHILDVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

	BOOL Create(LPCTSTR, LPCTSTR, DWORD, const RECT&, CWnd*, UINT, CCreateContext*);
	void OnInitialUpdate();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	CGridFormThread *_pThread;
	CListCtrl m_GridList;
};



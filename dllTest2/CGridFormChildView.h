#pragma once
#include "afxcmn.h"
#include "resource.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"

// GridFormChildView 表單檢視
#define WM_USER_DRAW		(WM_USER + 1)

class CGridFormThread;

class GridFormChildView : public CFormView
{
	DECLARE_DYNCREATE(GridFormChildView)

	GridFormChildView();           // 動態建立所使用的保護建構函式
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

	unsigned long long _counter;
	HANDLE _ListMutex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	void	CloseAllHnd(void);
	CGridFormThread *_pThread;
	CListCtrl m_GridList;
	afx_msg void OnClose();
	CList <PARAM_STRUCT, PARAM_STRUCT&> _List;
protected:
	afx_msg LRESULT OnUserDraw(WPARAM wParam, LPARAM lParam);
};



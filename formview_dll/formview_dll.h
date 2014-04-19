#pragma once

#include "stdafx.h"
#include "resource.h"

extern "C" AFX_EXT_API void WINAPI InitGridForm();

class AFX_EXT_CLASS GridFormChildView : public CFormView
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
//	CGridFormThread *_pThread;
	CListCtrl m_GridList;
	afx_msg void OnClose();
//	CList <PARAM_STRUCT, PARAM_STRUCT&> _List;
protected:
	afx_msg LRESULT OnUserDraw(WPARAM wParam, LPARAM lParam);
};


	
// CGridFormChildFrm 框架

class AFX_EXT_CLASS CGridFormChildFrm : public CMDIChildWnd
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

#undef AFX_DATA
#define AFX_DATA


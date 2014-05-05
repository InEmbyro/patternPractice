#pragma once

#include "stdafx.h"
#include "resource.h"
#include "../Softing/Can_def.h"
#include "../Softing//CANL2.H"
#include "GridList.h"

#define	WM_USER_DRAW	(WM_USER + 1)

extern "C" AFX_EXT_API LPVOID WINAPI InitGridForm();

class CGridFormThread;
class AFX_EXT_CLASS GridFormChildView : public CFormView
{
	DECLARE_DYNCREATE(GridFormChildView)
	static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	GridFormChildView();           // 動態建立所使用的保護建構函式
	virtual ~GridFormChildView();
	CGridFormThread *pFormThread;

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
	static const char* mailslot;

	void	CloseAllHnd(void);

	CGridList m_GridList;
	afx_msg void OnClose();
	CList <PARAM_STRUCT, PARAM_STRUCT&> _List;
	CMap <unsigned int, unsigned int, WPARAM_STRUCT, WPARAM_STRUCT&> _Map;

protected:
	afx_msg LRESULT OnUserDraw(WPARAM wParam, LPARAM lParam);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};


class CGridFormThread;
// CGridFormChildFrm 框架

class AFX_EXT_CLASS CGridFormChildFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CGridFormChildFrm)

	CGridFormThread *pGridFormThread;

protected:
	CGridFormChildFrm();           // 動態建立所使用的保護建構函式
	~CGridFormChildFrm();

public:

	GridFormChildView *_pView;
	POSITION	rawPos;
	CGridFormChildFrm* pMyself;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnClose();
//	afx_msg void OnDestroy();
};

#undef AFX_DATA
#define AFX_DATA


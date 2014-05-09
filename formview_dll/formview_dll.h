#pragma once

#include "stdafx.h"
#include "resource.h"
#include "../Softing/Can_def.h"
#include "../Softing//CANL2.H"
#include "GridList.h"

typedef int (__cdecl *GENERICCOMPAREFN)(const void * elem1, const void * elem2);

#define	WM_USER_DRAW	(WM_USER + 1)


extern "C" AFX_EXT_API LPVOID WINAPI InitGridForm();

class CGridFormThread;
class AFX_EXT_CLASS GridFormChildView : public CFormView
{
	DECLARE_DYNCREATE(GridFormChildView)
	//static int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int __cdecl Compare(const WPARAM_STRUCT * p1, const WPARAM_STRUCT * p2);

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

	HANDLE _ListMutex;
	HANDLE _MapMutex;
	HANDLE _ArrayMutex;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()

public:
	static const char* mailslot;

	void	CloseAllHnd(void);

	CGridList m_GridList;
	afx_msg void OnClose();
	CList <PARAM_STRUCT, PARAM_STRUCT&> _List;
	CList<WPARAM_STRUCT, WPARAM_STRUCT&> _ListBack;
	CArray <WPARAM_STRUCT, WPARAM_STRUCT&> _Array;
	CMap <unsigned int, unsigned int, WPARAM_STRUCT, WPARAM_STRUCT&> _Map;

protected:
	afx_msg LRESULT OnUserDraw(WPARAM wParam, LPARAM lParam);
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnContextMenu(CWnd* /*pWnd*/, CPoint /*point*/);
	afx_msg void OnTopCleargrid();
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
	afx_msg void OnNcRButtonUp(UINT nHitTest, CPoint point);
};

#undef AFX_DATA
#define AFX_DATA


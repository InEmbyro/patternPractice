#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"


extern "C" AFX_EXT_API LPVOID WINAPI InitTargetListForm();
#define	WM_USER_DRAW	(WM_USER + 1)

// CTargetList ����˵�

class CReceiveThread;

class CTargetList : public CFormView
{
	DECLARE_DYNCREATE(CTargetList)

	CTargetList();           // �ʺA�إߩҨϥΪ��O�@�غc�禡
	CReceiveThread *pRcvThread;
protected:
	virtual ~CTargetList();

public:
	enum { IDD = IDD_TARGETLIST_FORMVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

	DECLARE_MESSAGE_MAP()
public:
	static const char* mailslot;

	CComboBox m_combo;
	CListCtrl m_listctrl;
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnUserDraw(WPARAM wParam, LPARAM lParam);
};

// CTargetListForm �ج[

class CTargetListForm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTargetListForm)
	CTargetListForm();           // �ʺA�إߩҨϥΪ��O�@�غc�禡

	CReceiveThread *pRcvThread;
protected:
	virtual ~CTargetListForm();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CTargetList *_pView;
	POSITION	rawPos;
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};



#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"


extern "C" AFX_EXT_API LPVOID WINAPI InitTargetListForm();

// CTargetList ����˵�

class CTargetList : public CFormView
{
	DECLARE_DYNCREATE(CTargetList)

	CTargetList();           // �ʺA�إߩҨϥΪ��O�@�غc�禡
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
	CComboBox m_combo;
	CListCtrl m_listctrl;
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};

// CTargetListForm �ج[

class CTargetListForm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTargetListForm)
	CTargetListForm();           // �ʺA�إߩҨϥΪ��O�@�غc�禡

protected:
	virtual ~CTargetListForm();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CTargetList *_pView;
//	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle = WS_CHILD | WS_VISIBLE | WS_OVERLAPPEDWINDOW, const RECT& rect = rectDefault, CMDIFrameWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};



#pragma once
#include "stdafx.h"
#include "resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include "TargetListCtrl.h"

extern unsigned long recvId[];
extern "C" AFX_EXT_API LPVOID WINAPI InitTargetListForm();

typedef int (__cdecl *GENERICCOMPAREFN1)(const void * elem1, const void * elem2);

#define	WM_USER_DRAW	(WM_USER + 1)
#define WM_FAKE_KEYDOWN			(WM_USER + 3)
#define RAD_CONVER	(3.1416 / 180)


// CTargetList 表單檢視
class CReceiveThread;

class CTargetList : public CFormView
{
	DECLARE_DYNCREATE(CTargetList)

	CTargetList();           // 動態建立所使用的保護建構函式
	CReceiveThread *pRcvThread;
	void ParseRawObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void ParseRawObject2nd(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void ParseTrackingObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void ParseTrackingObject2nd(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);

	void SetTrackingHeader();
	void SetRawHeader();

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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	static const char* mailslot;
	CMap <unsigned long, unsigned long, unsigned long, unsigned long&> _filterMap;
#if 1
	HANDLE _MapShowMutex;
	HANDLE _MapStoreMutex;
	CMap <unsigned char, unsigned char, RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT> *_MapShow;
	CMap <unsigned char, unsigned char, RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT> *_MapStore;
#else
	HANDLE _listMutex;
	HANDLE _listStoreMutex;
	CList <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&> *_list;
	CList <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&> *_listStore;
#endif

	CArray <RAW_OBJECT_STRUCT, RAW_OBJECT_STRUCT&> _listArray;
	static int __cdecl Compare0(const RAW_OBJECT_STRUCT * p1, const RAW_OBJECT_STRUCT * p2);
	static int __cdecl Compare1(const RAW_OBJECT_STRUCT * p1, const RAW_OBJECT_STRUCT * p2);

	CComboBox m_combo;
	CTargetListCtrl m_listctrl;
	int GetComboIdx();
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnUserDraw(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnCbnSelchangeTargetlistCombo();
	virtual void OnInitialUpdate();
	void	ChangeShow();
protected:
	afx_msg LRESULT OnFakeKeydown(WPARAM wParam, LPARAM lParam);
private:
	bool	m_bShow;
};

// CTargetListForm 框架

class CTargetListForm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CTargetListForm)
	CTargetListForm();           // 動態建立所使用的保護建構函式

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
protected:
	afx_msg LRESULT OnFakeKeydown(WPARAM wParam, LPARAM lParam);
};



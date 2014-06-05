#pragma once

#include "stdafx.h"
#include "resource.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"

#define	WM_USER_DRAW	(WM_USER + 1)

extern "C" AFX_EXT_API LPVOID WINAPI InitBirdviewForm(void);



// CBirdviewView 檢視
class CReceiveThread;
class CBirdviewView : public CView
{
	DECLARE_DYNCREATE(CBirdviewView)
	static const char* mailslot;
	static const unsigned int slotKey;

	CBirdviewView();           // 動態建立所使用的保護建構函式
	virtual ~CBirdviewView();
	void DrawCarLine(CDC *pDc);
	void SetOrigin(CDC *pDc, BOOL action, int x_offset = 0, int y_offset = 0);
	CArray <CPoint, CPoint&> _Array;

	CReceiveThread *pRcvThread;
	int count;

public:
	HANDLE _ListMutex;
	HANDLE _MapMutex;

	CList <PARAM_STRUCT, PARAM_STRUCT&> _List;
	CMap <unsigned long, unsigned long, unsigned long, unsigned long&> _ReceiveMap;


#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnUserDraw(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnPaint();
	virtual void OnDraw(CDC* /*pDC*/);
};

#pragma once


// CBirdviewFrm 框架

class AFX_EXT_CLASS CBirdviewFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CBirdviewFrm)
	CReceiveThread *pRcvThread;

protected:
	CBirdviewFrm();           // 動態建立所使用的保護建構函式
	virtual ~CBirdviewFrm();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CBirdviewView *_pView;
	POSITION	rawPos;

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnClose();
};

#pragma once

#include "stdafx.h"
extern "C" AFX_EXT_API LPVOID WINAPI InitBirdviewForm(void);

#pragma once


// CBirdviewView 檢視

class CBirdviewView : public CView
{
	DECLARE_DYNCREATE(CBirdviewView)

	CBirdviewView();           // 動態建立所使用的保護建構函式
	virtual ~CBirdviewView();

public:
	virtual void OnDraw(CDC* pDC);      // 覆寫以描繪此檢視
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	DECLARE_MESSAGE_MAP()
};

#pragma once


// CBirdviewFrm 框架

class AFX_EXT_CLASS CBirdviewFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CBirdviewFrm)
protected:
	CBirdviewFrm();           // 動態建立所使用的保護建構函式
	virtual ~CBirdviewFrm();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CBirdviewView *_pView;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

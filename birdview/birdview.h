#pragma once

#include "stdafx.h"
extern "C" AFX_EXT_API LPVOID WINAPI InitBirdviewForm(void);

#pragma once


// CBirdviewView �˵�

class CBirdviewView : public CView
{
	DECLARE_DYNCREATE(CBirdviewView)

	CBirdviewView();           // �ʺA�إߩҨϥΪ��O�@�غc�禡
	virtual ~CBirdviewView();

public:
	virtual void OnDraw(CDC* pDC);      // �мg�H�yø���˵�
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


// CBirdviewFrm �ج[

class AFX_EXT_CLASS CBirdviewFrm : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CBirdviewFrm)
protected:
	CBirdviewFrm();           // �ʺA�إߩҨϥΪ��O�@�غc�禡
	virtual ~CBirdviewFrm();

protected:
	DECLARE_MESSAGE_MAP()

public:
	CBirdviewView *_pView;
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
};

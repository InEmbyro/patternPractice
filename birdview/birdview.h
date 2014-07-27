#pragma once

#include "stdafx.h"
#include "resource.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"

#define	WM_USER_DRAW	(WM_USER + 1)

extern "C" AFX_EXT_API LPVOID WINAPI InitBirdviewForm(void);

typedef struct {
	unsigned long canId;
	int	rowIdx;
} ARRAY_IDX;

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
	void CreateRGBPalette();

	CReceiveThread *pRcvThread;
	void ParseRawObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void ParseTrackingObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void DrawRawOjbect(PARAM_STRUCT *, CDC*);
	void DrawTrackingObject(PARAM_STRUCT*, CDC*);

public:
	CPalette    m_cPalette;
	CPalette    *m_pOldPalette;
	CRect       m_oldRect;
	CClientDC   *m_pDC;
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
	void	Init();
	float	m_fRadius;
	BOOL bSetupPixelFormat(void);
	void DrawScene();

	HANDLE _ListMutex;
	HANDLE _MapMutex;

	CList <PARAM_STRUCT, PARAM_STRUCT&> _List;
	CArray <CList <PARAM_STRUCT, PARAM_STRUCT&>*, CList <PARAM_STRUCT, PARAM_STRUCT&>*> _ListArray;	//for showing
	CArray <HANDLE, HANDLE&> _ListArrayMutex;
	CArray <CList <PARAM_STRUCT, PARAM_STRUCT&>*, CList <PARAM_STRUCT, PARAM_STRUCT&>*> _ListStoreArray;	//for storing
	CArray <HANDLE, HANDLE&> _ListStoreArrayMutex;
	CMap <unsigned long, unsigned long, ARRAY_IDX, ARRAY_IDX&> _FilterMap;


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
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
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

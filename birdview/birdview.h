#pragma once

#include "stdafx.h"
#include "resource.h"
#include "gl/gl.h"
#include "gl/glu.h"
#include "../Softing/Can_def.h"
#include "../Softing/CANL2.H"

#define	WM_USER_DRAW			(WM_USER + 1)
#define WM_UPDATE_SPEED_DRAWING (WM_USER + 2)

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
	static float halfCarWidth;
	static float halfCarLen;
	static float halfCarHeight;
	static float halfRoadWidth;
	static float m_Speed;
	char quote[20];
	HGLRC	m_hrc;

	CBirdviewView();           // 動態建立所使用的保護建構函式
	virtual ~CBirdviewView();
	void DrawCarLine(CDC *pDc);
	void SetOrigin(CDC *pDc, BOOL action, int x_offset = 0, int y_offset = 0);
	CArray <CPoint, CPoint&> _Array;
	void CreateRGBPalette();

	CReceiveThread *pRcvThread;
	BOOL ParseRawObject2nd(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void ParseRawObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void ParseTrackingObject(PARAM_STRUCT *pSrc, RAW_OBJECT_STRUCT *pRaw);
	void DrawRawObject(PARAM_STRUCT *, CDC*);
	void DrawRawObject3D(PARAM_STRUCT *);
	void DrawRawObject3D(RAW_OBJECT_STRUCT *pRaw, unsigned char sensorNo);
	void DrawTrackingObject(PARAM_STRUCT*, CDC*);
	void DrawTrackingObject3D(PARAM_STRUCT*);
	void DrawFarTarget(void);
	void DrawText();

public:
	CPalette    m_cPalette;
	CPalette    *m_pOldPalette;
	CRect       m_oldRect;
	CClientDC   *m_pDC;
	unsigned char ComponentFromIndex(int i, UINT nbits, UINT shift);
	void	Init();
	float	m_fRadius;
	float	m_fOldRadius;
	float	m_fYoffset;
	BOOL bSetupPixelFormat(void);
	void DrawScene();
	GLfloat	m_fMaxObjSize;
	GLfloat m_fMaxObjSizeOld;
	GLfloat m_fAspect;
	GLfloat	m_fNearPlane;
	GLfloat m_fFarPlane;
	GLfloat m_fOldFarPlane;
	GLfloat m_fFov;
	GLfloat m_fOldFov;
	GLfloat m_RoadLineStartZ;
	GLfloat m_RoadLineStartZStep;


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
	afx_msg void OnSize(UINT nType, int cx, int cy);
protected:
	afx_msg LRESULT OnUpdateSpeedDrawing(WPARAM wParam, LPARAM lParam);
public:
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
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

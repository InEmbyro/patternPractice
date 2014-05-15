#pragma once
#include "afxcmn.h"
#include "GridformSetListCtrl.h"

typedef enum {
	ERROR_MODE = 0x113FF,
	BUFFER_MODE,
	IDENT_MODE,
} GRID_MODE;
// CGridformSet 對話方塊

class CGridformSet : public CDialog
{
	DECLARE_DYNAMIC(CGridformSet)

public:
	CGridformSet(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CGridformSet();

// 對話方塊資料
	enum { IDD = IDD_DLG_GRIDSETTING };
private:
	GRID_MODE	m_gridMode;
	HTREEITEM	m_hGridMode;
	HTREEITEM	m_hFilter;
	HTREEITEM ExpandItem(HTREEITEM node);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	CTreeCtrl _tree;
	CGridformSetListCtrl _list;
	afx_msg void OnTvnSelchangedTreeProperty(NMHDR *pNMHDR, LRESULT *pResult);
};

#pragma once
#include "afxcmn.h"
#include "confDef.h"
#include "GridformSetListCtrl.h"

// CGridformSet 對話方塊

class CGridformSet : public CDialog
{
	DECLARE_DYNAMIC(CGridformSet)

public:
	CGridformSet(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CGridformSet();
	GRID_MODE GetGridMode() { return m_dispMode.mode; }
	void SetGridMode(GRID_MODE idx) { m_dispMode.mode = idx; }
	unsigned GetGridRows() { return m_dispMode.rows; }
	void SetGridRows(unsigned int row) { m_dispMode.rows = row; }

// 對話方塊資料
	enum { IDD = IDD_DLG_GRIDSETTING };
private:
	DISPLAY_MODE_SET	m_dispMode;	
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
//	afx_msg void OnNMClickListProperty(NMHDR *pNMHDR, LRESULT *pResult);
protected:
	afx_msg LRESULT OnDisaplyMode(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnConfigGetSel(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnDisplayRows(WPARAM wParam, LPARAM lParam);
};

#pragma once
#include "afxcmn.h"
#include "confDef.h"
#include "GridformSetListCtrl.h"

// CGridformSet ��ܤ��

class CGridformSet : public CDialog
{
	DECLARE_DYNAMIC(CGridformSet)

public:
	CGridformSet(CWnd* pParent = NULL);   // �зǫغc�禡
	virtual ~CGridformSet();

// ��ܤ�����
	enum { IDD = IDD_DLG_GRIDSETTING };
private:
	GRID_MODE	m_gridMode;
	HTREEITEM	m_hGridMode;
	HTREEITEM	m_hFilter;
	HTREEITEM ExpandItem(HTREEITEM node);


protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �䴩

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
};

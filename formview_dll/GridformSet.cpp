// GridformSet.cpp : 實作檔
//

#include "stdafx.h"
#include "formview_dll.h"
#include "GridformSet.h"
#include "afxdialogex.h"


// CGridformSet 對話方塊

IMPLEMENT_DYNAMIC(CGridformSet, CDialog)

CGridformSet::CGridformSet(CWnd* pParent /*=NULL*/)
	: CDialog(CGridformSet::IDD, pParent)
{

}

CGridformSet::~CGridformSet()
{
}

void CGridformSet::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_PROPERTY, _tree);
	DDX_Control(pDX, IDC_LIST_PROPERTY, _list);
}


BEGIN_MESSAGE_MAP(CGridformSet, CDialog)
	ON_BN_CLICKED(IDOK, &CGridformSet::OnBnClickedOk)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PROPERTY, &CGridformSet::OnTvnSelchangedTreeProperty)
	ON_MESSAGE(WM_DISPLAY_MODE, &CGridformSet::OnDisaplyMode)
	ON_MESSAGE(WM_CONFIG_GET_SEL, &CGridformSet::OnConfigGetSel)
	ON_MESSAGE(WM_DISPLAY_ROWS, &CGridformSet::OnDisplayRows)
END_MESSAGE_MAP()


// CGridformSet 訊息處理常式


void CGridformSet::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CDialog::OnOK();
}


BOOL CGridformSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;

	// TODO:  在此加入額外的初始化
	//List
	CRect rect;
	_list.GetClientRect(&rect);
	_list.InsertColumn(0, _T("Param"), LVCFMT_LEFT, rect.Width() / 2);
	_list.InsertColumn(1, _T(" "), LVCFMT_LEFT, rect.Width() / 2);
	ListView_SetExtendedListViewStyle(_list.m_hWnd, LVS_EX_FULLROWSELECT );
	//ListView_SetExtendedListViewStyle(_list.m_hWnd, LVS_EX_GRIDLINES);

	//tree
	str.LoadString(IDS_DISPLAY_GRID);
	m_hGridMode = _tree.InsertItem(str); 
	str.LoadString(IDS_DISPLAYMODE);
	_tree.InsertItem(str, m_hGridMode);
	str.LoadString(IDS_NUMBER_OF_GRID_ROWS);
	_tree.InsertItem(str, m_hGridMode);
	TreeView_Expand(_tree.m_hWnd, m_hGridMode, TVE_EXPAND);
	
	str.LoadString(IDS_FILTER);
	m_hFilter = _tree.InsertItem(str);
	str.LoadString(IDS_FILTER_ACTIVE);
	_tree.InsertItem(str, m_hFilter);
	str.LoadString(IDS_FILTER_IDENTIFIER);
	_tree.InsertItem(str, m_hFilter);
	str.LoadString(IDS_FILTER_MODE);
	_tree.InsertItem(str, m_hFilter);
	_tree.SendMessage(TVM_EXPAND, (WPARAM)TVE_EXPAND, (LPARAM)m_hFilter);
	_tree.ModifyStyle(0, TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT );
	TreeView_SelectItem(_tree.m_hWnd, m_hGridMode);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

HTREEITEM CGridformSet::ExpandItem(HTREEITEM node)
{
	return NULL;
}

void CGridformSet::OnTvnSelchangedTreeProperty(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: 在此加入控制項告知處理常式程式碼
	TVITEM newItem = pNMTreeView->itemNew;
	HTREEITEM hTree;
	HTREEITEM newTree;

	newTree = newItem.hItem;
	hTree = TreeView_GetParent(_tree.m_hWnd, newTree);
	while (hTree) {
		newTree = hTree;
		hTree = TreeView_GetParent(_tree.m_hWnd, newTree);
	}

	/* all shown combo-box should be closed first */
	_list.CloseShownComboBox();
	if (newTree == m_hGridMode)
		_list.SetContentIdx(DISPLAY_MODE);
	else if (newTree == m_hFilter)
		_list.SetContentIdx(FILTER);
	_list.ShowListContent(_list.GetContentIdx());

	*pResult = 0;
}



afx_msg LRESULT CGridformSet::OnDisaplyMode(WPARAM wParam, LPARAM lParam)
{
	m_dispMode.mode = (GRID_MODE)lParam;
	return 0;
}


afx_msg LRESULT CGridformSet::OnConfigGetSel(WPARAM wParam, LPARAM lParam)
{
	void *p = NULL;

	switch ((LIST_CONTENT)wParam) {
	case DISPLAY_MODE:
		p = (void*)lParam;
		*((DISPLAY_MODE_SET*)p) = m_dispMode;
		break;
	default:
		break;
	}
	return 0;
}


afx_msg LRESULT CGridformSet::OnDisplayRows(WPARAM wParam, LPARAM lParam)
{
	m_dispMode.rows = (unsigned int)lParam;
	return 0;
}

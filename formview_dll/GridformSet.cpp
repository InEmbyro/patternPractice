// GridformSet.cpp : ��@��
//

#include "stdafx.h"
#include "formview_dll.h"
#include "GridformSet.h"
#include "afxdialogex.h"


// CGridformSet ��ܤ��

IMPLEMENT_DYNAMIC(CGridformSet, CDialog)

CGridformSet::CGridformSet(CWnd* pParent /*=NULL*/)
	: CDialog(CGridformSet::IDD, pParent), m_gridMode(IDENT_MODE)
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
END_MESSAGE_MAP()


// CGridformSet �T���B�z�`��


void CGridformSet::OnBnClickedOk()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	CDialog::OnOK();
}


BOOL CGridformSet::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;

	// TODO:  �b���[�J�B�~����l��
	str.LoadString(IDS_DISPLAY_GRID);
	m_hGridMode = _tree.InsertItem(str); 
	str.LoadString(IDS_DISPLAYMODE);
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

	//List
	CRect rect;
	_list.GetClientRect(&rect);
	_list.InsertColumn(0, _T("Param"), LVCFMT_LEFT, rect.Width() / 2);
	_list.InsertColumn(1, _T(" "), LVCFMT_LEFT, rect.Width() / 2);
	ListView_SetExtendedListViewStyle(_list.m_hWnd, LVS_EX_FULLROWSELECT );
	//ListView_SetExtendedListViewStyle(_list.m_hWnd, LVS_EX_GRIDLINES);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX �ݩʭ����Ǧ^ FALSE
}

HTREEITEM CGridformSet::ExpandItem(HTREEITEM node)
{
	return NULL;
}

void CGridformSet::OnTvnSelchangedTreeProperty(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMTREEVIEW pNMTreeView = reinterpret_cast<LPNMTREEVIEW>(pNMHDR);
	// TODO: �b���[�J����i���B�z�`���{���X
	TVITEM newItem = pNMTreeView->itemNew;
	HTREEITEM hTree;
	HTREEITEM newTree;

	newTree = newItem.hItem;
	hTree = TreeView_GetParent(_tree.m_hWnd, newTree);
	while (hTree) {
		newTree = hTree;
		hTree = TreeView_GetParent(_tree.m_hWnd, newTree);
	}

	int jj = 0;
	CString str;
	if (newTree == m_hGridMode) {
		_list.DeleteAllItems();
		str.LoadString(IDS_DISPLAYMODE);
		_list.InsertItem(0, str);
	} else if (newTree == m_hFilter) {
		_list.DeleteAllItems();
		str.LoadString(IDS_FILTER_ACTIVE);
		_list.InsertItem(jj, str);
		_list.SetItemText(jj, 1, _T("No"));
		
		jj++;
		str.LoadString(IDS_FILTER_IDENTIFIER);
		_list.InsertItem(jj, str);
		_list.SetItemText(jj, 1, _T(" "));

		jj++;
		str.LoadString(IDS_FILTER_MODE);
		_list.InsertItem(jj, str);
		_list.SetItemText(jj, 1, _T("Show only filtered"));
	}

	*pResult = 0;
}



//void CGridformSet::OnNMClickListProperty(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
//	// TODO: �b���[�J����i���B�z�`���{���X
//	*pResult = 0;
//}

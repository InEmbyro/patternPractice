// ComboBoxList.cpp : ��@��
//

#include "stdafx.h"
#include "formview_dll.h"
#include "ComboBoxList.h"


// CComboBoxList

IMPLEMENT_DYNAMIC(CComboBoxList, CComboBox)

CComboBoxList::CComboBoxList()
{

}

CComboBoxList::~CComboBoxList()
{
}


BEGIN_MESSAGE_MAP(CComboBoxList, CComboBox)
	ON_WM_KILLFOCUS()
//	ON_WM_CLOSE()
ON_CONTROL_REFLECT(CBN_CLOSEUP, &CComboBoxList::OnCbnCloseup)
END_MESSAGE_MAP()



// CComboBoxList �T���B�z�`��




void CComboBoxList::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	PostMessage(WM_CLOSE);
	// TODO: �b���[�J�z���T���B�z�`���{���X
}


//void CComboBoxList::OnClose()
//{
//	// TODO: �b���[�J�z���T���B�z�`���{���X�M (��) �I�s�w�]��
//	GetParent()->SetFocus();
//	CComboBox::OnClose();
//}


void CComboBoxList::OnCbnCloseup()
{
	// TODO: �b���[�J����i���B�z�`���{���X
	GetParent()->SetFocus();
}

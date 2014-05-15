// ComboBoxList.cpp : 實作檔
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



// CComboBoxList 訊息處理常式




void CComboBoxList::OnKillFocus(CWnd* pNewWnd)
{
	CComboBox::OnKillFocus(pNewWnd);

	PostMessage(WM_CLOSE);
	// TODO: 在此加入您的訊息處理常式程式碼
}


//void CComboBoxList::OnClose()
//{
//	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
//	GetParent()->SetFocus();
//	CComboBox::OnClose();
//}


void CComboBoxList::OnCbnCloseup()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	GetParent()->SetFocus();
}

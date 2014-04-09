// canEngine.cpp : �w�q DLL ����l�Ʊ`���C
//

#include "stdafx.h"
#include "canEngine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

//
//TODO: �p�G�o�� DLL �O�ʺA�a�� MFC DLL �s���A
//		����q�o�� DLL �ץX������|�I�s
//		MFC �������禡�A�������b�禡�@�}�Y�[�W AFX_MANAGE_STATE
//		�����C
//
//		�Ҧp:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// ���B�����`�禡�D��
//		}
//
//		�o�ӥ����@�w�n�X�{�b�C�@��
//		�禡���A�~����I�s MFC �������C�o�N����
//		�������O�禡�����Ĥ@�ӳ��z��
//		�A�Ʀܥ����b���󪫥��ܼƫŧi�e��
//		�A�]�����̪��غc�禡�i��|���͹� MFC
//		DLL �������I�s�C
//
//		�аѾ\ MFC �޳N���� 33 �M 58 ����
//		�ԲӸ�ơC
//

// CcanEngineApp



BEGIN_MESSAGE_MAP(CcanEngineApp, CWinApp)
END_MESSAGE_MAP()


// CcanEngineApp �غc

CcanEngineApp::CcanEngineApp()
{
	// TODO: �b���[�J�غc�{���X�A
	// �N�Ҧ����n����l�]�w�[�J InitInstance ��
}


// �Ȧ����@�� CcanEngineApp ����

CcanEngineApp theApp;

// CcanEngineApp ��l�]�w
BOOL CcanEngineApp::InitInstance()
{
	CWinApp::InitInstance();

	return TRUE;
}

CCanInfo::CCanInfo()
	:m_pBuf(NULL)
{

}

CCanInfo::~CCanInfo()
{
	if (m_pBuf)
		free(m_pBuf);
	
	POSITION pos;
	MY_L2CONF l2con;
	while ((pos = m_ListL2Config.GetHeadPosition())) {
		l2con = m_ListL2Config.GetAt(pos);
		if (l2con.HasInit)
			INIL2_close_channel(l2con.chHnd);
		m_ListL2Config.RemoveHead();
	}
}

void CCanInfo::GetDeviceType(int u32DeviceType)
{
	switch(u32DeviceType) {
	case D_CANCARD2:
		m_CurTypeName.SetString(_T("CANcard2"));
		break;
	case D_CANACPCI:
		m_CurTypeName.SetString(_T("CAN-AC PCI"));
		break;
	case D_CANACPCIDN:
		m_CurTypeName.SetString(_T("CAN-AC PCI/DN"));
		break;
	case D_CANAC104:
		m_CurTypeName.SetString(_T("CAN-AC PC/104"));
		break;
	case D_CANUSB:
		m_CurTypeName.SetString(_T("CANusb"));
		break;
	case D_EDICCARD2:
		m_CurTypeName.SetString(_T("EDICcard2"));
		break;
	case D_CANPROXPCIE:
		m_CurTypeName.SetString(_T("CANpro PCI Express"));
		break;
	case D_CANPROX104:
		m_CurTypeName.SetString(_T("CANpro PC/104plus"));
		break;
	case D_CANECO104:
		m_CurTypeName.SetString(_T("CAN-ECOx-104"));
		break;
	case D_CANFANUPC8:
		m_CurTypeName.SetString(_T("PC8 onboard CAN"));
		break;
	case D_CANPROUSB:
		m_CurTypeName.SetString(_T("CANpro USB"));
		break;
	default:
		m_CurTypeName.SetString(_T("default"));
		break;
	}
}

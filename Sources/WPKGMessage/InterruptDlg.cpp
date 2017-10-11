// InterruptDlg.cpp : implementation file
//

#include "stdafx.h"
#include "WPKGMessage.h"
#include "InterruptDlg.h"


// CInterruptDlg dialog

IMPLEMENT_DYNAMIC(CInterruptDlg, CDialog)

CInterruptDlg::CInterruptDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInterruptDlg::IDD, pParent)
	, m_strPwd(_T("**************"))
{

}

CInterruptDlg::~CInterruptDlg()
{
}

void CInterruptDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_PWD, m_strPwd);
	DDV_MaxChars(pDX, m_strPwd, 20);
}


BEGIN_MESSAGE_MAP(CInterruptDlg, CDialog)
END_MESSAGE_MAP()


// CInterruptDlg message handlers

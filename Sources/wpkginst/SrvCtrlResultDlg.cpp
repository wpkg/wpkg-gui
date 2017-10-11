// SrvCtrlResultDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "SrvCtrlResultDlg.h"


// CSrvCtrlResultDlg dialog

IMPLEMENT_DYNAMIC(CSrvCtrlResultDlg, CDialog)

CSrvCtrlResultDlg::CSrvCtrlResultDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSrvCtrlResultDlg::IDD, pParent)
	, m_strResult(_T(""))
{

}

CSrvCtrlResultDlg::~CSrvCtrlResultDlg()
{
}

void CSrvCtrlResultDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_RESULT, m_strResult);
}


BEGIN_MESSAGE_MAP(CSrvCtrlResultDlg, CDialog)
END_MESSAGE_MAP()


// CSrvCtrlResultDlg message handlers

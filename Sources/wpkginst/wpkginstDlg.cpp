// wpkginstDlg.cpp : implementation file
//

#include "stdafx.h"
#include "wpkginst.h"
#include "wpkginstDlg.h"
#include ".\wpkginstdlg.h"
#include "XmlSettings.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma warning(disable : 4996)

// CWpkgInstDlg dialog



CWpkgInstDlg::CWpkgInstDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CWpkgInstDlg::IDD, pParent)
	,m_strScriptFile(_T("wpkg.js"))
	,m_strScriptParameters(_T(""))
	,m_strScriptConnUser(_T(""))
	,m_strScriptConnPassword(_T(""))
	,m_strScriptExecUser(_T(""))
	,m_strScriptExecPassword(_T(""))
	,m_strPreAction(_T(""))
	,m_strPostAction(_T(""))
	, m_bShowGUI(FALSE)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CWpkgInstDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_FILE, m_strScriptFile);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PARAMATERS, m_strScriptParameters);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_USER, m_strScriptConnUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PATH_PASSWORD, m_strScriptConnPassword);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_USER, m_strScriptExecUser);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_EXEC_PASSWORD, m_strScriptExecPassword);
	DDX_Control(pDX, IDC_LIST_SCRIPT_VARIABLES, m_PathVariables);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_PREACTION, m_strPreAction);
	DDX_Text(pDX, IDC_EDIT_SCRIPT_POSTACTION, m_strPostAction);
	DDX_Check(pDX, IDC_CHECK_PRE, m_bPreAction);
	DDX_Check(pDX, IDC_CHECK_POST, m_bPostAction);
	DDX_Check(pDX, IDC_CHECK_SHOW_GUI, m_bShowGUI);
}

BEGIN_MESSAGE_MAP(CWpkgInstDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON_HELP, OnBnClickedButtonHelp)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_FILE, OnBnClickedButtonSelectFile)
	ON_BN_CLICKED(IDC_BUTTON_SCRIPTVARIABLE_NEW, OnBnClickedButtonPathvariableNew)
	ON_BN_CLICKED(IDC_BUTTON_SCRIPTVARIABLE_DELETE, OnBnClickedButtonPathvariableDelete)
	ON_BN_CLICKED(IDC_CHECK_PRE, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_CHECK_POST, OnBnClickedCheck)
	ON_BN_CLICKED(IDC_BUTTON_ADVANCED, OnBnClickedButtonAdvanced)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_PREFILE, OnBnClickedButtonSelectPrefile)
	ON_BN_CLICKED(IDC_BUTTON_SELECT_POSTFILE, OnBnClickedButtonSelectPostfile)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_SETTINGS, OnBnClickedButtonExportSettings)
END_MESSAGE_MAP()


// CWpkgInstDlg message handlers

BOOL CWpkgInstDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	m_PathVariables.SetExtendedStyle(LVS_EX_FLATSB|LVS_EX_INFOTIP|
		LVS_EX_GRIDLINES);
	m_PathVariables.InsertColumn(0,"Name",LVCFMT_LEFT,220);
	m_PathVariables.InsertColumn(1,"Value",LVCFMT_LEFT,220);
	
	int pos = 0;
	for(int i=0;i<m_strVarArray.GetCount();i+=2)
	{
		pos = m_PathVariables.InsertItem(pos,m_strVarArray.GetAt(i));
		m_PathVariables.SetItemText(pos,1,m_strVarArray.GetAt(i+1));
		pos++;
	}

	ShowAdvanced();

	
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CWpkgInstDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CWpkgInstDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CWpkgInstDlg::OnBnClickedButtonHelp()
{
	CRegKey key;
	char value[1024];
	DWORD size = 1024;
	key.Open(HKEY_LOCAL_MACHINE,"SOFTWARE\\WPKG.ORG\\Settings",KEY_READ);
	key.QueryValue(value,"Path",&size);
	key.Close();

	CString str(value);
	if(str.Right(1)!='\\')
		str+='\\';

	str += "WPKG Installer Parameters.txt";
	
	ShellExecute(NULL, _T("open"), str, NULL,NULL, SW_SHOW);
}

void CWpkgInstDlg::OnBnClickedOk()
{
	UpdateData();

	if(m_strScriptFile.IsEmpty() )
		AfxMessageBox(IDS_FIELD_REQUIRED);
	else
	{
		m_strVarArray.RemoveAll();

		for(int i=0;i<m_PathVariables.GetItemCount();i++)
		{
			m_strVarArray.Add(m_PathVariables.GetItemText(i,0));
			m_strVarArray.Add(m_PathVariables.GetItemText(i,1));
		}
		OnOK();
	}
}

void CWpkgInstDlg::OnBnClickedButtonSelectFile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Script files (*.js)|*.js||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		m_strScriptFile = fdlg.GetPathName();
		UpdateData(FALSE);
	}

}

void CWpkgInstDlg::OnBnClickedButtonPathvariableNew()
{
	int count = m_PathVariables.GetItemCount();
	count = m_PathVariables.InsertItem(count,"enter name...");
	m_PathVariables.SetItemText(count,1,"enter value...");
	m_PathVariables.EditSubLabel(count,0);
}


void CWpkgInstDlg::OnBnClickedButtonPathvariableDelete()
{
	int nItem = -1;
	nItem = m_PathVariables.GetNextItem(nItem,LVNI_SELECTED);
	if(nItem!=-1)
		m_PathVariables.DeleteItem(nItem);
}



void CWpkgInstDlg::AddScriptVarData(CStringArray& data)
{
	m_strVarArray.Copy(data);
	
}

void CWpkgInstDlg::GetScriptVarData(CStringArray& data)
{
	data.Copy(m_strVarArray);
}

void CWpkgInstDlg::ShowAdvanced()
{
	CRect rr;
	static BOOL bAdvanced = FALSE;
	int offset = bAdvanced ? 280 : -280;

	GetDlgItem(IDC_STATIC_GUI)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_STATIC_GUI)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_CHECK_SHOW_GUI)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_CHECK_SHOW_GUI)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_LIST_SCRIPT_VARIABLES)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_LIST_SCRIPT_VARIABLES)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_STATIC_GROUP_SCRIPTVARIABLES)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_STATIC_GROUP_SCRIPTVARIABLES)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_STATIC_SCRIPTVARIABLES)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_STATIC_SCRIPTVARIABLES)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_BUTTON_SCRIPTVARIABLE_DELETE)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_BUTTON_SCRIPTVARIABLE_DELETE)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_BUTTON_SCRIPTVARIABLE_NEW)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_BUTTON_SCRIPTVARIABLE_NEW)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_CHECK_PRE)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_CHECK_PRE)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_CHECK_POST)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_CHECK_POST)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->EnableWindow(bAdvanced & m_bPreAction);

	GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->EnableWindow(bAdvanced & m_bPostAction);

	GetDlgItem(IDC_STATIC_ACTIONS)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_STATIC_ACTIONS)->EnableWindow(bAdvanced);

	GetDlgItem(IDC_BUTTON_SELECT_PREFILE)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_BUTTON_SELECT_PREFILE)->EnableWindow(bAdvanced & m_bPreAction);

	GetDlgItem(IDC_BUTTON_SELECT_POSTFILE)->ShowWindow(bAdvanced);
	GetDlgItem(IDC_BUTTON_SELECT_POSTFILE)->EnableWindow(bAdvanced & m_bPostAction);

	if(bAdvanced)
		GetDlgItem(IDC_BUTTON_ADVANCED)->SetWindowText("<< General");
	else
		GetDlgItem(IDC_BUTTON_ADVANCED)->SetWindowText("Advanced >>");

	GetDlgItem(IDC_BUTTON_ADVANCED)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDC_BUTTON_ADVANCED)->MoveWindow(&rr);

	GetDlgItem(IDC_BUTTON_HELP)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDC_BUTTON_HELP)->MoveWindow(&rr);
	
	GetDlgItem(IDC_BUTTON_EXPORT_SETTINGS)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDC_BUTTON_EXPORT_SETTINGS)->MoveWindow(&rr);

	GetDlgItem(IDOK)->GetWindowRect(&rr);
	ScreenToClient(&rr);
	rr.OffsetRect(0,offset);
	GetDlgItem(IDOK)->MoveWindow(&rr);
	
	GetWindowRect(&rr);
	rr.bottom += offset;
	MoveWindow(&rr);

	CenterWindow();

	bAdvanced = !bAdvanced;

}

void CWpkgInstDlg::OnBnClickedCheck()
{
	UpdateData();

	GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->EnableWindow(m_bPreAction);
	if(!m_bPreAction)
		GetDlgItem(IDC_EDIT_SCRIPT_PREACTION)->SetWindowText("");
	GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->EnableWindow(m_bPostAction);
	if(!m_bPostAction)
		GetDlgItem(IDC_EDIT_SCRIPT_POSTACTION)->SetWindowText("");
	GetDlgItem(IDC_BUTTON_SELECT_PREFILE)->EnableWindow(m_bPreAction);
	GetDlgItem(IDC_BUTTON_SELECT_POSTFILE)->EnableWindow(m_bPostAction);
}

void CWpkgInstDlg::OnBnClickedButtonAdvanced()
{
	ShowAdvanced();
}

void CWpkgInstDlg::OnBnClickedButtonSelectPrefile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Executable files (*.exe; *.bat; *com)|*.exe; *.bat; *.com|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		m_strPreAction = fdlg.GetPathName();
		UpdateData(FALSE);
	}

}

void CWpkgInstDlg::OnBnClickedButtonSelectPostfile()
{
	UpdateData();

	CFileDialog  fdlg( TRUE,NULL, NULL, 0, "Executable files (*.exe; *.bat; *com)|*.exe; *.bat; *.com|All files (*.*)|*.*||",  NULL );

	if(fdlg.DoModal()==IDOK)
	{
		m_strPostAction = fdlg.GetPathName();
		UpdateData(FALSE);
	}
}

void CWpkgInstDlg::OnBnClickedButtonExportSettings()
{
	UpdateData();
	CString xmlFilePath;

	CFileDialog  fdlg( FALSE,"xml", "settings", OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		"Settings files (*.xml)|*.xml||",  NULL );

	if(fdlg.DoModal()!=IDOK)
		return;
	
	xmlFilePath = fdlg.GetPathName();

	try
	{
		CXmlSettings st;
		

		st.CreateInstance();
		st.Init();
		st.WriteParameter("file",m_strScriptFile);
		st.WriteParameter("path-user",m_strScriptConnUser);
		st.WriteParameter("path-password",st.Crypt(m_strScriptConnPassword));
		st.WriteParameter("exec-user",m_strScriptExecUser);
		st.WriteParameter("exec-password",st.Crypt(m_strScriptExecPassword));
		st.WriteParameter("parameters",m_strScriptParameters);
		st.WriteParameter("silent","YES");
		st.WriteParameter("pre-action",m_strPreAction);
		st.WriteParameter("post-action",m_strPostAction);
		st.WriteParameter("show-GUI",m_bShowGUI?"YES":"NO");

		for(int i=0; i<m_PathVariables.GetItemCount(); i++)
		{
			st.WriteParameterEx(m_PathVariables.GetItemText(i,0),
				m_PathVariables.GetItemText(i,1));
		}
		
		st.Save(xmlFilePath);
		AfxMessageBox("Export successfully done.");
	}
	catch(_com_error e)
	{
		CString str;
		str.Format("Error occured while writing parameter to settings file:\n%s",e.ErrorMessage());
		AfxMessageBox(str);
	}
	catch(...)
	{
		AfxMessageBox("Unknown error occured while writing parameter to settings file");
	}
}

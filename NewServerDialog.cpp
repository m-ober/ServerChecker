/*
ServerChecker will keep supported servers up and running
Copyright (C) 2005  [DumB]H!GHGuY

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


// NewServerDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ServerCheckerGUI.h"
#include "NewServerDialog.h"
#include ".\newserverdialog.h"
#include "Control.h"


// NewServerDialog dialog

IMPLEMENT_DYNAMIC(CNewServerDialog, CDialog)
CNewServerDialog::CNewServerDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CNewServerDialog::IDD, pParent)
	, type(0)
{
}

CNewServerDialog::~CNewServerDialog()
{
}

void CNewServerDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_IP, m_IP);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_EXEC, m_Executable);
	DDX_Control(pDX, IDC_COMBO1, m_Type);
	DDX_Control(pDX, IDC_NAME, m_Name);
}


BEGIN_MESSAGE_MAP(CNewServerDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
END_MESSAGE_MAP()


// NewServerDialog message handlers


void CNewServerDialog::OnBnClickedOk()
{
	// fill in variables
	m_IP.GetWindowText(ip);
	m_Port.GetWindowText(port);
	m_Executable.GetWindowText(exec);
	m_Name.GetWindowText(name);
	type = m_Type.GetCurSel();
	if (type != NORMAL_PROGRAM_INDEX && (ip == _T("") || port == _T("") || exec == _T("") || name == _T("") || type == CB_ERR ))
	{
		int answer = MessageBox("All fields must be completed. Do you want to complete them now?","Warning...",MB_YESNO | MB_APPLMODAL | MB_ICONQUESTION );
		switch (answer)
		{
		case IDYES:
			return; // skip OnOk(), so user can edit the values
			break;
		case IDNO:
			OnCancel(); // Close the dialog
			return;
			break;
		}
	}
	OnOK();
}

CString CNewServerDialog::getIP(void)
{
	return ip;
}

int CNewServerDialog::getPort(void)
{
	return atoi(port);
}

CString CNewServerDialog::getExecutable(void)
{
	return exec;
}

CString CNewServerDialog::getName(void)
{
	return name;
}

BOOL CNewServerDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	for (int i = 0; i < NR_GAMES; i++)
	{
		m_Type.InsertString(i,GameTypes[i]);
	}
	m_Type.UpdateData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

int CNewServerDialog::getType(void)
{
	return type;
}

void CNewServerDialog::OnBnClickedBrowse()
{
	// TODO: Add your control notification handler code here
	CFileDialog dlg(true,"*.exe",NULL,0,"Executable files (*.exe)|*.exe||",this,0);
	if (dlg.DoModal() == IDOK)
	{
		m_Executable.SetWindowText(dlg.GetPathName());
	}
	UpdateWindow();
}

void CNewServerDialog::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	if (m_Type.GetCurSel() == NORMAL_PROGRAM_INDEX)
	{
		GetDlgItem(IDC_IP)->EnableWindow(false);
		GetDlgItem(IDC_PORT)->EnableWindow(false);
	}
	else
	{
		GetDlgItem(IDC_IP)->EnableWindow(true);
		GetDlgItem(IDC_PORT)->EnableWindow(true);
	}
}

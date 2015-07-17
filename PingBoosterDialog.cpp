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

// PingBoosterDialog.cpp : implementation file
//

#include "stdafx.h"
#include "ServerCheckerGUI.h"
#include "PingBoosterDialog.h"
#include ".\pingboosterdialog.h"


// CPingBoosterDialog dialog

IMPLEMENT_DYNAMIC(CPingBoosterDialog, CDialog)
CPingBoosterDialog::CPingBoosterDialog(int val, CWnd* pParent /*=NULL*/)
	: CDialog(CPingBoosterDialog::IDD, pParent)
	, boosterval(val)
{
}

CPingBoosterDialog::~CPingBoosterDialog()
{
}

void CPingBoosterDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, boostervalue);
}


BEGIN_MESSAGE_MAP(CPingBoosterDialog, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CPingBoosterDialog message handlers

int CPingBoosterDialog::getValue(void)
{
	return boosterval;
}

void CPingBoosterDialog::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	CString bval;
	boostervalue.GetWindowText(bval);
	boosterval = atoi(bval);
	OnOK();
}

BOOL CPingBoosterDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	CString val;
	val.Format("%d",boosterval);
	boostervalue.SetWindowText(val);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

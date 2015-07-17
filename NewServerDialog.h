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


#pragma once
#include "afxwin.h"


// NewServerDialog dialog

class CNewServerDialog : public CDialog
{
	DECLARE_DYNAMIC(CNewServerDialog)

public:
	CNewServerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~CNewServerDialog();

// Dialog Data
	enum { IDD = IDD_NEWSERVER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	CString getIP(void);
	int getPort(void);
	CString getExecutable(void);
	int getType(void);
	CString getName();
private:
	CString ip, port, exec, name;
	int type;
private:
	CEdit m_IP;
	CEdit m_Port;
	CEdit m_Executable;
	CComboBox m_Type;
	CEdit m_Name;
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnEditCopy();	
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeCombo1();
};

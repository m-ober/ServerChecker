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

// CPingBoosterDialog dialog

class CPingBoosterDialog : public CDialog
{
	DECLARE_DYNAMIC(CPingBoosterDialog)

public:
	CPingBoosterDialog(int val, CWnd* pParent = NULL);   // standard constructor
	virtual ~CPingBoosterDialog();

// Dialog Data
	enum { IDD = IDD_PINGBOOSTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	CEdit boostervalue;
public:
	int getValue(void);
	afx_msg void OnBnClickedOk();
private:
	int boosterval;
public:
	virtual BOOL OnInitDialog();
};

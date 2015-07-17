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
#include "Control.h"
#include "afxdtctl.h"

// CDetailView form view

class CDetailView : public CFormView
{
	DECLARE_DYNCREATE(CDetailView)

protected:
	CDetailView();           // protected constructor used by dynamic creation
	virtual ~CDetailView();

public:
	enum { IDD = IDD_DETAILVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	afx_msg void OnEnChangeEdit1();
	CEdit m_Port;
	CEdit m_IP;
	CComboBox m_Type;
	CEdit m_Executable;
	virtual void OnInitialUpdate();
	afx_msg void OnCbnSelchangeType();
	afx_msg void OnEnChangeIp();
	CButton btn_Update;
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnEnChangePort();
	afx_msg void OnEnChangeExecutable();
	CEdit m_TimeOut;
	CEdit m_Pause;
	CEdit m_Startup;
	afx_msg void OnEnChangeTimeout();
	afx_msg void OnEnChangePause();
	afx_msg void OnEnChangeStartup();
	afx_msg void OnCbnSelchangeCombo1();
	afx_msg void OnEditPaste();
	afx_msg void OnEditCopy();
private:
	CComboBox m_Priority;
	CEdit m_Name;
public:
	afx_msg void OnEnChangeName();
	CButton m_TimeControlled;
	CDateTimeCtrl m_StartTime;
	CDateTimeCtrl m_StopTime;
	afx_msg void OnBnClickedTimecontrol();
	afx_msg void OnDtnDatetimechangeStarttime(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnDtnDatetimechangeStoptime(NMHDR *pNMHDR, LRESULT *pResult);
};



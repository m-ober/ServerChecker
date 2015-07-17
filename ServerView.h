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
#include "afxcmn.h"
#include "afxwin.h"
#include "afx.h"

#define ID_CHECK_TIME 0x0001
// CServerView form view

class CServerView : public CFormView
{
	DECLARE_DYNCREATE(CServerView)

protected:
	CServerView();           // protected constructor used by dynamic creation
	virtual ~CServerView();

public:
	enum { IDD = IDD_SERVERVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnInitialUpdate();
	afx_msg void OnPaint();
	afx_msg void OnLvnItemchangedServerlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnUpdateUIState(UINT /*nAction*/, UINT /*nUIElement*/);
protected:
	CListCtrl m_ServerList;
public:
	afx_msg void OnLvnItemActivateServerlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMClickServerlist(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedNew();
	afx_msg void OnBnClickedDelete();
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedStop();
//	virtual INT_PTR OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
protected:
	virtual void OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/);
public:
	CButton btn_New;
	CButton btn_Delete;
	CButton btn_Start;
	CButton btn_Stop;
	afx_msg LRESULT OnServerUpdate(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnStartAllServers(WPARAM wParam, LPARAM lParam);
private:
	CFile* m_LogFile;
public:
	afx_msg void OnTimer(UINT nIDEvent);
	bool setTimer(bool on);
	UINT_PTR m_Timer;
};



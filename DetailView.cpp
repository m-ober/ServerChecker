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


// DetailView.cpp : implementation file
//

#include "stdafx.h"
#include "ServerCheckerGUI.h"
#include "DetailView.h"
#include "ServerDoc.h"
#include ".\detailview.h"


// CDetailView

IMPLEMENT_DYNCREATE(CDetailView, CFormView)

CDetailView::CDetailView()
	: CFormView(CDetailView::IDD)
{
}

CDetailView::~CDetailView()
{
}

void CDetailView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PORT, m_Port);
	DDX_Control(pDX, IDC_IP, m_IP);
	DDX_Control(pDX, IDC_TYPE, m_Type);
	DDX_Control(pDX, IDC_EXECUTABLE, m_Executable);
	DDX_Control(pDX, IDC_UPDATE, btn_Update);
	DDX_Control(pDX, IDC_TIMEOUT, m_TimeOut);
	DDX_Control(pDX, IDC_PAUSE, m_Pause);
	DDX_Control(pDX, IDC_STARTUP, m_Startup);
	DDX_Control(pDX, IDC_COMBO1, m_Priority);
	DDX_Control(pDX, IDC_NAME, m_Name);
	DDX_Control(pDX, IDC_TIMECONTROL, m_TimeControlled);
	DDX_Control(pDX, IDC_STARTTIME, m_StartTime);
	DDX_Control(pDX, IDC_STOPTIME, m_StopTime);
}

BEGIN_MESSAGE_MAP(CDetailView, CFormView)
	ON_CBN_SELCHANGE(IDC_TYPE, OnCbnSelchangeType)
	ON_EN_CHANGE(IDC_IP, OnEnChangeIp)
	ON_BN_CLICKED(IDC_UPDATE, OnBnClickedUpdate)
	ON_EN_CHANGE(IDC_PORT, OnEnChangePort)
	ON_EN_CHANGE(IDC_EXECUTABLE, OnEnChangeExecutable)
	ON_EN_CHANGE(IDC_TIMEOUT, OnEnChangeTimeout)
	ON_EN_CHANGE(IDC_PAUSE, OnEnChangePause)
	ON_EN_CHANGE(IDC_STARTUP, OnEnChangeStartup)
	ON_CBN_SELCHANGE(IDC_COMBO1, OnCbnSelchangeCombo1)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_EN_CHANGE(IDC_NAME, OnEnChangeName)
	ON_BN_CLICKED(IDC_TIMECONTROL, OnBnClickedTimecontrol)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STARTTIME, OnDtnDatetimechangeStarttime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STOPTIME, OnDtnDatetimechangeStoptime)
END_MESSAGE_MAP()


// CDetailView diagnostics

#ifdef _DEBUG
void CDetailView::AssertValid() const
{
	CFormView::AssertValid();
}

void CDetailView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG


// CDetailView message handlers

void CDetailView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	CServerDoc* doc = (CServerDoc*)GetDocument();
	ServerControl* sc= doc->getSelectedServer();
	if (sc)
	{
		m_IP.SetWindowText(sc->getServerIP());
		m_Executable.SetWindowText(sc->getExecutable());

		CString tmp;
		tmp.Format("%d",sc->getPort());

		m_Port.SetWindowText( tmp );
		m_Type.SetCurSel(sc->getGameType());

		if (sc->getGameType() == NORMAL_PROGRAM_INDEX)
		{
			GetDlgItem(IDC_TIMEOUT)->EnableWindow(false);
			GetDlgItem(IDC_STARTUP)->EnableWindow(false);
			GetDlgItem(IDC_PAUSE)->EnableWindow(false);
			GetDlgItem(IDC_IP)->EnableWindow(false);
			GetDlgItem(IDC_PORT)->EnableWindow(false);
		}
		else
		{
			GetDlgItem(IDC_TIMEOUT)->EnableWindow(true);
			GetDlgItem(IDC_STARTUP)->EnableWindow(true);
			GetDlgItem(IDC_PAUSE)->EnableWindow(true);
			GetDlgItem(IDC_IP)->EnableWindow(true);
			GetDlgItem(IDC_PORT)->EnableWindow(true);
		}

		tmp.Format("%d",sc->getPause());
		m_Pause.SetWindowText(tmp);

		tmp.Format("%d",sc->getStartup());
		m_Startup.SetWindowText(tmp);

		tmp.Format("%d",sc->getTimeOut());
		m_TimeOut.SetWindowText(tmp);

		m_Name.SetWindowText(sc->getServerName());

		PRIORITY prior = sc->getPriority();
		switch (prior)
		{
		case HIGH:
			m_Priority.SetCurSel(0);
			break;
		case LOW:
			m_Priority.SetCurSel(3);
			break;
		case ABOVE_NORMAL:
			m_Priority.SetCurSel(1);
			break;
		default:
			m_Priority.SetCurSel(2);
			break;
		}

		if (sc->IsTimeControlled())
		{
			m_TimeControlled.SetCheck(BST_CHECKED);
			GetDlgItem(IDC_STARTTIME)->EnableWindow(true);
			GetDlgItem(IDC_STOPTIME)->EnableWindow(true);
			
			//CTime starttime = sc->GetStartTime();
			SYSTEMTIME starttime = sc->GetStartTime();
			//CTimestoptime = sc->GetStopTime();
			SYSTEMTIME stoptime = sc->GetStopTime();
			m_StartTime.SetTime(&starttime);
			m_StopTime.SetTime(&stoptime);			
		}
		else
		{
			m_TimeControlled.SetCheck(BST_UNCHECKED);
			GetDlgItem(IDC_STARTTIME)->EnableWindow(false);
			GetDlgItem(IDC_STOPTIME)->EnableWindow(false);
			//CTime starttime = sc->GetStartTime();
			SYSTEMTIME starttime = sc->GetStartTime();
			//CTimestoptime = sc->GetStopTime();
			SYSTEMTIME stoptime = sc->GetStopTime();
			m_StartTime.SetTime(&starttime);
			m_StopTime.SetTime(&stoptime);		
		}
	}
	else
	{
		m_IP.SetWindowText(_T(""));
		m_Executable.SetWindowText(_T(""));
		m_Port.SetWindowText(_T(""));
		m_Type.SetCurSel(-1);
		m_Startup.SetWindowText(_T(""));
		m_Pause.SetWindowText(_T(""));
		m_TimeOut.SetWindowText(_T(""));
		m_Priority.SetCurSel(-1);
		m_Name.SetWindowText(_T(""));
		m_TimeControlled.SetCheck(BST_UNCHECKED);
		GetDlgItem(IDC_STARTTIME)->EnableWindow(false);
		GetDlgItem(IDC_STOPTIME)->EnableWindow(false);

		SYSTEMTIME nothing;
		memset(&nothing,0,sizeof(SYSTEMTIME));

		m_StartTime.SetTime(nothing);
		m_StopTime.SetTime(nothing);
	}
	GetDlgItem(IDC_UPDATE)->EnableWindow(false); // disable update button
	UpdateData();
}

void CDetailView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	for (int i = 0; i < NR_GAMES; i++)
	{
		m_Type.InsertString(i,GameTypes[i]);
	}
	for (int i = 0; i < 4; i++)
	{
		m_Priority.InsertString(i,Priorities[i]);
	}
	// TODO: Add your specialized code here and/or call the base class
	m_Type.UpdateData();
	m_Priority.UpdateData();
	Invalidate();
}

void CDetailView::OnCbnSelchangeType()
{
	// TODO: Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnEnChangeIp()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnBnClickedUpdate()
{
	// TODO: Add your control notification handler code here
	bool updateable = false; // state of the update button
	
	CServerDoc* doc = (CServerDoc*)GetDocument();
	ServerControl* sc= doc->getSelectedServer();
	if (sc)
	{
		int game = m_Type.GetCurSel();

		CString server = _T("");
		unsigned short port = 0;
		CString tmp;

		m_Port.GetWindowText(tmp);
		port = (unsigned short)(strtol(tmp,NULL,10)%USHRT_MAX);

		if (game != NORMAL_PROGRAM_INDEX)
		{
			if (!port)
			{
				MessageBox("Invalid port","Error:");
				updateable = true;
			}
		}

		m_IP.GetWindowText(server);

		CString executable;
		m_Executable.GetWindowText(executable);

		CString name;
		m_Name.GetWindowText(name);
		
		m_Startup.GetWindowText(tmp);
		int startup = strtol(tmp,NULL,10);

		m_TimeOut.GetWindowText(tmp);
		int timeout = strtol(tmp,NULL,10);

		m_Pause.GetWindowText(tmp);
		int pause = strtol(tmp,NULL,10);

		bool timecontrol = (m_TimeControlled.GetCheck() == BST_CHECKED);
		//CTime starttime;
		SYSTEMTIME starttime;
		m_StartTime.GetTime(&starttime);

		//CTime stoptime;
		SYSTEMTIME stoptime;
		m_StopTime.GetTime(&stoptime);

		try
		{
			if (timeout <= 0 || startup <= 0 || pause <= 0)
			{
				throw "Timeout, startup and pause must be positive, non-zero integers.";
			}
			sc->setServer(server,port)->setGame(game)->setExecutable(executable);
			sc->setTimeOut(timeout)->setPause(pause)->setStartUp(startup)->setServerName(name);
			sc->SetStartStopTime(starttime, stoptime);
		}
		catch (char* error)
		{
			MessageBox(error,"Error...",MB_OK | MB_ICONWARNING);
			updateable = true;
		}
	}
	GetDlgItem(IDC_UPDATE)->EnableWindow(updateable);
	doc->UpdateAllViews(NULL);
}

void CDetailView::OnEnChangePort()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnEnChangeExecutable()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnEnChangeTimeout()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnEnChangePause()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnEnChangeStartup()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	CServerDoc* doc = (CServerDoc*)GetDocument();
	ServerControl* sc = doc->getSelectedServer();
	if (sc)
	{
		bool func_ok;
		int prior = m_Priority.GetCurSel();
		switch (prior)
		{
		case 0: // high
			func_ok = sc->setPriority(HIGH);
			break;
		case 1: // above_normal
			func_ok = sc->setPriority(ABOVE_NORMAL);
			break;
		case 2: // normal
			func_ok = sc->setPriority(NORMAL);
			break;
		case 3:
			func_ok = sc->setPriority(LOW);
			break;
		default:
			func_ok = false;
		}

		if (!func_ok)
			MessageBox("Could not set process priority.","Error...");
	}
	else
		MessageBox("No server selected","Error...");
    doc->UpdateAllViews(NULL);
}

void CDetailView::OnEditPaste() // fix for paste issue
{
	// TODO: Add your command handler code here
	CWnd* focus = GetFocus();
	if (focus && focus->GetRuntimeClass() == RUNTIME_CLASS(CEdit)) // only editboxes
	{
		((CEdit*)focus)->Paste();
	}
}

void CDetailView::OnEditCopy() // fix for copy/cut issue
{
	CWnd* focus = GetFocus();
	if (focus && focus->GetRuntimeClass() == RUNTIME_CLASS(CEdit)) //only editboxes!
	{
		((CEdit*)focus)->Copy();
	}
}

void CDetailView::OnEnChangeName()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CFormView::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	// TODO:  Add your control notification handler code here
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnBnClickedTimecontrol()
{
	// TODO: Add your control notification handler code here
	CServerDoc* doc = (CServerDoc*)GetDocument();
	ServerControl* sc = doc->getSelectedServer();
	if (sc)
	{
		bool enableTime = false;
		if (m_TimeControlled.GetCheck() == BST_CHECKED)
		{
			enableTime = true;
		}
		GetDlgItem(IDC_STARTTIME)->EnableWindow(enableTime);
		GetDlgItem(IDC_STOPTIME)->EnableWindow(enableTime);

		sc->SetTimeControlled(enableTime);
	}
	else
	{
		m_TimeControlled.SetCheck(BST_UNCHECKED);
	}
    doc->UpdateAllViews(NULL);
}

void CDetailView::OnDtnDatetimechangeStarttime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

void CDetailView::OnDtnDatetimechangeStoptime(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMDATETIMECHANGE pDTChange = reinterpret_cast<LPNMDATETIMECHANGE>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
	GetDlgItem(IDC_UPDATE)->EnableWindow(true);
}

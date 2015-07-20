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


// ServerView.cpp : implementation file
//

#include "stdafx.h"
#include "ServerCheckerGUI.h"
#include "ServerView.h"
#include "Control.h"
#include "ServerDoc.h"
#include ".\serverview.h"
#include "NewServerDialog.h"
#include "UDPClient.h"
#include "afxcmn.h"
#include "Mainfrm.h"

const int CHAR_WIDTH = 10;

enum SubItems
{
	SI_NR = 0,
	SI_NAME = 1,
	SI_PID = 2,
	SI_IP = 3,
	SI_PORT = 4,
	SI_UP = 5,
	SI_STATUS = 6	
};

// CServerView

IMPLEMENT_DYNCREATE(CServerView, CFormView)

CServerView::CServerView()
	: CFormView(CServerView::IDD), m_LogFile(NULL)
	, m_Timer(NULL)
{
	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	
	strAppDirectory += "\\ServerChecker.log";

	bool tryagain = true;
	while (tryagain)
	{
		try
		{
			m_LogFile = new CFile(strAppDirectory,CFile::modeCreate | CFile::modeNoTruncate | CFile::modeReadWrite | CFile::shareDenyWrite | CFile::osWriteThrough |CFile::modeNoInherit);
			m_LogFile->SeekToEnd();
			tryagain = false;
		}
		catch (...)
		{
			tryagain = (MessageBox("Could not open logfile. \r\nMake sure the drive has enough free space\r\nand that the logfile is not in use by another program.\r\nTry again?\r\n\r\n(Note: clicking NO will disable logging)","Error...", MB_YESNO |MB_ICONWARNING)==IDYES);
		}
	}
	if (m_LogFile)
	{
		CString tmp;
		tmp.Format("%s : --- Logging Started ---\r\n", (CTime::GetCurrentTime()).Format("%c") );
		try
		{
			m_LogFile->Write(tmp.GetBuffer(),tmp.GetLength());
		}
		catch (CFileException* ex)
		{
			ex->ReportError();
		}
	}
}

CServerView::~CServerView()
{
	if (m_LogFile)
	{
		CString tmp;
		tmp.Format("%s : --- Logging Stopped ---\r\n", CTime::GetCurrentTime().Format("%c"));
		try
		{
			m_LogFile->Write(tmp.GetBuffer(),tmp.GetLength());
		}
		catch (CFileException* ex) {}
		m_LogFile->Close();
		delete m_LogFile;
	}
	//this->setTimer(false);
}

void CServerView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SERVERLIST, m_ServerList);
	DDX_Control(pDX, IDC_NEW, btn_New);
	DDX_Control(pDX, IDC_DELETE, btn_Delete);
	DDX_Control(pDX, IDC_START, btn_Start);
	DDX_Control(pDX, IDC_STOP, btn_Stop);
}

BEGIN_MESSAGE_MAP(CServerView, CFormView)
	ON_WM_PAINT()
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SERVERLIST, OnLvnItemchangedServerlist)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_SERVERLIST, OnLvnItemActivateServerlist)
	ON_NOTIFY(NM_CLICK, IDC_SERVERLIST, OnNMClickServerlist)
	ON_BN_CLICKED(IDC_NEW, OnBnClickedNew)
	ON_BN_CLICKED(IDC_DELETE, OnBnClickedDelete)
	ON_BN_CLICKED(IDC_START, OnBnClickedStart)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_MESSAGE(ID_SERVER_UPDATE,OnServerUpdate)
	ON_MESSAGE(ID_START_ALL_SERVERS,OnStartAllServers)
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CServerView diagnostics

#ifdef _DEBUG
void CServerView::AssertValid() const
{
	CFormView::AssertValid();
}

void CServerView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

void CServerView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();
	
	m_ServerList.InsertColumn(SI_NR ,"Nr",LVCFMT_LEFT,5*CHAR_WIDTH,SI_NR);
	m_ServerList.InsertColumn(SI_NAME ,"Name",LVCFMT_LEFT,12*CHAR_WIDTH,SI_NAME);
	m_ServerList.InsertColumn(SI_PID ,"PID",LVCFMT_LEFT,4*CHAR_WIDTH,SI_PID);
	m_ServerList.InsertColumn(SI_IP,"IP",LVCFMT_LEFT,10*CHAR_WIDTH,SI_IP);
	m_ServerList.InsertColumn(SI_PORT,"Port",LVCFMT_LEFT,5*CHAR_WIDTH,SI_PORT);
	m_ServerList.InsertColumn(SI_UP,"Up",LVCFMT_LEFT,6*CHAR_WIDTH,SI_UP);
	m_ServerList.InsertColumn(SI_STATUS,"Status",LVCFMT_LEFT,11*CHAR_WIDTH,SI_STATUS);


	CServerDoc* doc = (CServerDoc*)GetDocument();
	for (int i = 0; i <= doc->getServerCount(); i++)
	{
		ServerControl* sc = doc->getServer(i);
		if (sc != NULL)
		{
			CString nr; nr.Format("    %d    ",i+1);
			m_ServerList.InsertItem(i,nr);
			m_ServerList.SetItemText(i,SI_IP,sc->getServerIP());
			m_ServerList.SetItemText(i,SI_UP,_T("Down"));
			CString port; port.Format("%d",sc->getPort());
			m_ServerList.SetItemText(i,SI_PORT,port);
			m_ServerList.SetItemText(i,SI_STATUS,_T("Stopped"));
			m_ServerList.SetItemText(i,SI_NAME,sc->getServerName());
			sc->setParent(this);
		}
	}
}
// CServerView message handlers

void CServerView::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	// Do not call CFormView::OnPaint() for painting messages
}

void CServerView::OnLvnItemchangedServerlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CServerView::OnLvnItemActivateServerlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: Add your control notification handler code here
	//CServerDoc* doc = (CServerDoc*)this->GetDocument();
	//doc->SelectServer(pNMIA->iItem);
	*pResult = 0;
}

void CServerView::OnNMClickServerlist(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: Add your control notification handler code here
	CServerDoc* doc = (CServerDoc*)this->GetDocument();
	CString nr = m_ServerList.GetItemText(m_ServerList.GetSelectionMark(),SI_NR);
	doc->SelectServer(atoi(nr)-1);
	*pResult = 0;
}

void CServerView::OnBnClickedNew()
{
	// TODO: Add your control notification handler code here
	CServerDoc* doc = (CServerDoc*)GetDocument();
	CNewServerDialog dialog = new CNewServerDialog(this);
	INT_PTR nRet = dialog.DoModal();

	CString ip = dialog.getIP();
	int port =	dialog.getPort();
	CString exec = dialog.getExecutable();
	int type = dialog.getType();
	CString name = dialog.getName();

	switch (nRet)
	{
	case -1:
	case IDABORT:
		MessageBox("Could not create New Server Dialog","Error");
		break;
	case IDOK: // the dialog already checked if all values were entered
		ServerControl* sc;
		sc = NULL; // suppress error: initialization of sc is skipped by case label
		int nr;
		try
		{
			sc = new ServerControl(ip,port,exec,type);
			sc->setServerName(name);

			doc->addServer(sc);
			nr = doc->getServerNumber(sc);

			CString str_nr; str_nr.Format("    %d    ",nr+1);
			m_ServerList.InsertItem(nr,str_nr);
			m_ServerList.SetItemText(nr,SI_IP,sc->getServerIP());
			m_ServerList.SetItemText(nr,SI_UP,_T("Down"));
			CString port; port.Format("%d",sc->getPort());
			m_ServerList.SetItemText(nr,SI_PORT,port);
			m_ServerList.SetItemText(nr,SI_STATUS,_T("Stopped"));
			m_ServerList.SetItemText(nr,SI_NAME,sc->getServerName());
			DWORD pid = sc->getProcessID();
			CString PID; PID.Format("%d",pid);
			if (pid)
				m_ServerList.SetItemText(nr,SI_PID,PID);
			sc->setParent(this);

			doc->SelectServer(nr);
		}
		catch (...)
		{
			MessageBox("Error while creating new server.\r\n(If a DNS-name is entered, make sure it contains no errors and that it exists!)","Error",MB_ICONEXCLAMATION |MB_OK | MB_APPLMODAL);
			if (sc)
				delete sc;
		}
		break;
	case IDCANCEL:
		return;
		break;
	}
}

void CServerView::OnBnClickedDelete()
{
	// TODO: Add your control notification handler code here
	CServerDoc* doc = (CServerDoc*)GetDocument();
	int nr = doc->getSelectedServerNumber();
	if (nr >= 0)
	{
		m_ServerList.DeleteItem(nr);
		for (int i = 0 ; i < m_ServerList.GetItemCount() ; i++)
		{
			CString str_nr = m_ServerList.GetItemText(i,SI_NR);
			int curnr = atoi(str_nr);
			if (curnr > nr)
			{
				str_nr.Format("    %d    ",curnr-1);
				m_ServerList.SetItemText(i,SI_NR,str_nr);
			}
		}
		doc->delSelectedServer();
		Sleep(250);
	}
	doc->UpdateAllViews(NULL);
}

void CServerView::OnBnClickedStart()
{
	// TODO: Add your control notification handler code here
	CServerDoc* doc = (CServerDoc*)GetDocument();
	ServerControl* sc = doc->getSelectedServer();
	if (sc)
	{
		doc->startServer(sc);
	}
	else
		MessageBox("No server Selected","Error");
	Sleep(250);
	doc->UpdateAllViews(NULL);
}

void CServerView::OnBnClickedStop()
{
	// TODO: Add your control notification handler code here
	CServerDoc* doc = (CServerDoc*)GetDocument();
	ServerControl* sc = doc->getSelectedServer();
	if (sc)
	{
		doc->stopServer(sc);
	}
	else
		MessageBox("No server Selected","Error");
	doc->UpdateAllViews(NULL);
}

void CServerView::OnUpdate(CView* /*pSender*/, LPARAM /*lHint*/, CObject* /*pHint*/)
{
	// TODO: Add your specialized code here and/or call the base class
	CServerDoc* doc = (CServerDoc*)GetDocument();
	ServerControl* sc;
	bool usetimer = false;
	for (int i = 0; i < doc->getServerCount() ; i++)
	{
		sc = doc->getServer(i);
		if (sc)
		{
			int nr = doc->getServerNumber(sc);
			m_ServerList.SetItemText(nr,SI_IP,sc->getServerIP());
			CString tmp; tmp.Format("%d",sc->getPort());
			m_ServerList.SetItemText(nr,SI_PORT,tmp);
			m_ServerList.SetItemText(nr,SI_NAME,sc->getServerName());
			usetimer |= (sc->IsTimeControlled());
		}
	}

	if (usetimer && m_Timer == NULL)
	{
		if (!this->setTimer(usetimer))
		{
			MessageBox("Could not set timer. Time control may not be active.");
		}
	}

	sc = doc->getSelectedServer();
	if (sc)
	{
		bool active = sc->isActive();
		GetDlgItem(IDC_DELETE)->EnableWindow(!active);
		GetDlgItem(IDC_START)->EnableWindow(!active);
		GetDlgItem(IDC_STOP)->EnableWindow(active);
	}
	else
	{
		GetDlgItem(IDC_DELETE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START)->EnableWindow(FALSE);
		GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
	}
	CMainFrame* parent = (CMainFrame*)this->GetParentFrame();
	parent->PostMessage(ID_UPDATE_MENU);
}

afx_msg LRESULT CServerView::OnServerUpdate(WPARAM wParam, LPARAM lParam)
{
	ServerControl* sc = (ServerControl*)wParam;
	CServerDoc* doc = (CServerDoc*)GetDocument();
	int nr = doc->getServerNumber(sc);

	int up = sc->getUp();
	int status = sc->getStatus();

	while (up != INVALID_STATUS_UP && status != INVALID_STATUS_UP)
	{
		DWORD pid = sc->getProcessID();
		CString PID = _T("");
		if (pid)
		{
			PID.Format("%d",sc->getProcessID());
		}
		m_ServerList.SetItemText(nr,SI_STATUS,Status[status]);
		m_ServerList.SetItemText(nr,SI_UP,Up[up]);
		m_ServerList.SetItemText(nr,SI_PID,PID);
		if (m_LogFile)
		{
			try
			{
				CString tmp;
				tmp.Format("%s : %s:%d(%s) -> %s(%s)\r\n", (CTime::GetCurrentTime()).Format("%c"),
					sc->getServerIP(), sc->getPort(), sc->getServerName(), Up[up], Status[status]);

				m_LogFile->Write(tmp.GetBuffer(),tmp.GetLength());
			}
			catch(...) {}
		}
		m_ServerList.UpdateWindow();
		up = sc->getUp();
		status = sc->getStatus();
	}
	return 0;
}

afx_msg LRESULT CServerView::OnStartAllServers(WPARAM wParam, LPARAM lParam)
{
	CServerDoc* doc = (CServerDoc*)GetDocument();
	doc->startAllServers();
	doc->UpdateAllViews(NULL);
	return 0;
}

bool operator<(const SYSTEMTIME& first, const SYSTEMTIME& second)
{
	if (first.wHour < second.wHour)
		return true;
	else if (first.wHour > second.wHour)
		return false;

	if (first.wMinute < second.wMinute)
		return true;
	else if (first.wMinute > second.wMinute)
		return false;

	if (first.wSecond < second.wSecond)
		return true;
	else if (first.wSecond > second.wSecond)
		return false;

	return false;
}

bool operator>(const SYSTEMTIME& first, const SYSTEMTIME& second)
{
	if (first.wHour > second.wHour)
		return true;
	else if (first.wHour < second.wHour)
		return false;

	if (first.wMinute > second.wMinute)
		return true;
	else if (first.wMinute < second.wMinute)
		return false;

	if (first.wSecond > second.wSecond)
		return true;
	else if (first.wSecond < second.wSecond)
		return false;

	return false;
}

void CServerView::OnTimer(UINT nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	if (nIDEvent == ID_CHECK_TIME)
	{
		//CTime tmp = CTime::GetCurrentTime();
		//CTime now(1970,1,1,tmp.GetHour(),tmp.GetMinute(),tmp.GetSecond());
		SYSTEMTIME now;
		//::GetSystemTime(&now);
		::GetLocalTime(&now);

		CServerDoc* doc = (CServerDoc*)GetDocument();
		int nr = doc->getServerCount();
		ServerControl* sc;
		for (int i = 0; i < nr; i++)
		{
			sc = doc->getServer(i);
			//CTime start = sc->GetStartTime();
			//CTime stop = sc->GetStopTime();
			SYSTEMTIME start = sc->GetStartTime();
			SYSTEMTIME stop = sc->GetStopTime();
			if (sc->IsTimeControlled())
			{
				if (sc->isActive()) // active
				{
					if ((start > stop && now > stop && now < start) || (start < stop && (now > stop || now < start )))
					{
						doc->stopServer(sc);
					}
				} 
				else // inactive
				{
					bool a = (start > stop);
					bool x = (start > stop);
					bool b = (now < stop);
					bool c = (now < start);
					bool d = (now > stop);
					bool e = (now > start);


					if ((start > stop && (now < stop || now > start)) || (start < stop && now < stop && now > start))
					{
						doc->startServer(sc);
					}
				} // inactive
			} // if timecontrolled
		} // for all servers
		Sleep(250);
		doc->UpdateAllViews(NULL);
	} // if nIDEvent == ID_CHECK_TIME
	else
		CFormView::OnTimer(nIDEvent);
}

bool CServerView::setTimer(bool on)
{
	if (on)
	{ // check every 30 seconds
		m_Timer = SetTimer(ID_CHECK_TIME,30000,NULL);
		return m_Timer != 0;
	}
	else if (m_Timer != NULL)
	{
		bool ret = KillTimer(m_Timer);
		m_Timer = NULL;
		return ret;
	}
	return false;
}

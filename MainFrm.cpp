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


// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "ServerCheckerGUI.h"
#include "ServerView.h"
#include "ServerDoc.h"
#include "DetailView.h"
#include "MainFrm.h"
#include ".\mainfrm.h"
#include "PingBoosterDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_VIEW_LOG, OnViewLog)
	ON_MESSAGE(ID_UPDATE_MENU, OnUpdateMenu)
	ON_COMMAND(ID_PINGBOOSTER_CHANGEVALUE, OnPingboosterChangevalue)
	ON_COMMAND(ID_PINGBOOSTER_RUN, OnPingboosterRun)
	ON_MESSAGE(ID_ICON_CALLBACK, OnIconCallback)
	ON_WM_SIZE()
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};


// CMainFrame construction/destruction

CMainFrame::CMainFrame()
: iconData(NULL)
, m_bOwnerCreated(false)
{
	// TODO: add member initialization code here
	this->m_bAutoMenuEnable = false;
}

CMainFrame::~CMainFrame()
{
	if (iconData)
		delete iconData;
}


int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// don't need toolbar
	/*if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}*/

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}
	// TODO: Delete these three lines if you don't want the toolbar to be dockable
	//m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	//EnableDocking(CBRS_ALIGN_ANY);
	//DockControlBar(&m_wndToolBar);

	return 0;
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT /*lpcs*/,
	CCreateContext* pContext)
{
	BOOL res = m_wndSplitter.CreateStatic(this,2,1);
	if (!res)
		return FALSE;
	res = m_wndSplitter.CreateView(0,0,RUNTIME_CLASS(CServerView),CSize(450,245),pContext);
	if (!res)
		return FALSE;
	m_wndSplitter.CreateView(1,0,RUNTIME_CLASS(CDetailView),CSize(450,240),pContext);
	if (!res)
		return FALSE;
	m_wndSplitter.SetRowInfo(0,245,0);
	m_wndSplitter.SetRowInfo(1,240,0);
	m_wndSplitter.RecalcLayout();
	return res;

	/*return m_wndSplitter.Create(this,
		2, 1,               // TODO: adjust the number of rows, columns
		CSize(10, 10),      // TODO: adjust the minimum pane size
		pContext);*/
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs


	// http://www.codeguru.com/Cpp/frameworks/advancedui/article.php/c3227/
	if (!m_bOwnerCreated) // "dialog-is-created" flag
    {
        // m_MainFrameOwner - CDialog object mapped to the resource dialog template
        m_bOwnerCreated = m_MainFrameOwner.Create(IDD_FRAMEOWNER);
        if (m_bOwnerCreated) m_MainFrameOwner.ShowWindow(SW_HIDE);
    };

    // set the dialog as a parent of CMainFrame window
    if (m_bOwnerCreated)
    cs.hwndParent = m_MainFrameOwner.GetSafeHwnd();
	cs.dwExStyle = WS_EX_APPWINDOW; // otherwise no taskbar button @ untill minimized and restored

	// fixed window size
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= (0xFFFFFFFF ^ WS_SIZEBOX);
	cs.style |= WS_BORDER;
	cs.style &= (0xFFFFFFFF ^ WS_MAXIMIZEBOX);

	return TRUE;
}


// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


// CMainFrame message handlers


void CMainFrame::OnClose()
{
	// TODO: Add your message handler code here and/or call default

	// stopping the servers must be done here for some reason I forgot (prolly cause else the program crashes when trying to get the document)
	CServerView* serverview = (CServerView*)m_wndSplitter.GetPane(0,0);
	CServerDoc* doc = (CServerDoc*)serverview->GetDocument();
	bool no_kill = false;
	if (doc->runningServers())
	{
		int res = MessageBox("Shut down servers?\r\n\r\nNote: When running ServerChecker again, you must stop these servers manually first!\r\nServerChecker cannot obtain control over running servers.","Servers:",MB_YESNOCANCEL |MB_APPLMODAL | MB_ICONQUESTION );
		if (res != IDCANCEL)
			no_kill = (res == IDNO);
		else
			return;
	}
	doc->stopAllServers(no_kill);
	CFrameWnd::OnClose();
}

void CMainFrame::OnViewLog()
{
	// TODO: Add your command handler code here
	PROCESS_INFORMATION procinfo;
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	memset(&procinfo,0,sizeof(procinfo));
	si.cb = sizeof(si);

	char    szAppPath[MAX_PATH] = "";
	CString strDir;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strDir = szAppPath;
	strDir = strDir.Left(strDir.ReverseFind('\\'));
	
	strDir += "\\ServerChecker.log";
	CString exec = _T("notepad ");
	exec += strDir;

	BOOL res = CreateProcess(NULL,exec.GetBuffer(),NULL,NULL,false,0,NULL,NULL,&si,&procinfo);
	CloseHandle(procinfo.hThread);
	CloseHandle(procinfo.hProcess);
}

void CMainFrame::OnPingboosterChangevalue()
{
	// TODO: Add your command handler code here
	CServerView* serverview = (CServerView*)m_wndSplitter.GetPane(0,0);
	CServerDoc* doc = (CServerDoc*)serverview->GetDocument();
	
	CPingBoosterDialog dlg(doc->getBoosterValue(),this);

	if (dlg.DoModal() == IDOK)
	{
		doc->setBoosterValue(dlg.getValue());
	}
}

void CMainFrame::OnPingboosterRun()
{
	// TODO: Add your command handler code here
	CMenu* menu = GetMenu();
	UINT state = menu->GetMenuState(ID_PINGBOOSTER_RUN,MF_BYCOMMAND);
	UINT teststate = menu->GetMenuState(ID_PINGBOOSTER_CHANGEVALUE,MF_BYCOMMAND);
	bool turnon = !(state & MF_CHECKED);
	if (!turnon)
	{
		menu->CheckMenuItem(ID_PINGBOOSTER_RUN,MF_BYCOMMAND | MF_UNCHECKED);
		menu->EnableMenuItem(ID_PINGBOOSTER_CHANGEVALUE, MF_BYCOMMAND | MF_ENABLED);
	}
	else
	{
		menu->CheckMenuItem(ID_PINGBOOSTER_RUN,MF_BYCOMMAND | MF_CHECKED);
		menu->EnableMenuItem(ID_PINGBOOSTER_CHANGEVALUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}

	CServerView* serverview = (CServerView*)m_wndSplitter.GetPane(0,0);
	CServerDoc* doc = (CServerDoc*)serverview->GetDocument();
	doc->boost(turnon);
}

afx_msg LRESULT CMainFrame::OnUpdateMenu(WPARAM wParam, LPARAM lParam)
{
	CServerView* serverview = (CServerView*)m_wndSplitter.GetPane(0,0);
	CServerDoc* doc = (CServerDoc*)serverview->GetDocument();

	CMenu* menu = GetMenu();
	ASSERT(menu);

	if (doc->boosts())
	{
		menu->CheckMenuItem(ID_PINGBOOSTER_RUN,MF_BYCOMMAND | MF_CHECKED);
		menu->EnableMenuItem(ID_PINGBOOSTER_CHANGEVALUE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
	}
	return 0;
}

// Shell_NotifyIcon

void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	if (nType & SIZE_MINIMIZED)
	{
		if (!iconData)
		{
			iconData = new NOTIFYICONDATA;
			iconData->cbSize = NOTIFYICONDATA_V1_SIZE;
			iconData->hWnd = GetSafeHwnd();
			iconData->uID = 0;
			iconData->uFlags = NIF_ICON | NIF_MESSAGE;
			iconData->uCallbackMessage = ID_ICON_CALLBACK;
			iconData->hIcon = (HICON)theApp.LoadIcon(IDR_MAINFRAME);
		}
		Shell_NotifyIcon(NIM_ADD,iconData);
		ShowTaskBarButton(false);
	}
	CFrameWnd::OnSize(nType, cx, cy);
	// TODO: Add your message handler code here
	
}

afx_msg LRESULT CMainFrame::OnIconCallback(WPARAM wParam, LPARAM lParam)
{
	if ((lParam == WM_LBUTTONUP) || (lParam == WM_RBUTTONUP))
	{
		this->ShowWindow(SW_RESTORE);
		Shell_NotifyIcon(NIM_DELETE,iconData);
		ShowTaskBarButton(true);
	}
	return 0;
}

// http://www.codeguru.com/Cpp/frameworks/advancedui/article.php/c3227/
BOOL CMainFrame::ShowTaskBarButton(BOOL bVisible)
{
    if (!m_bOwnerCreated) return FALSE;

    ShowWindow(SW_HIDE);

    if (bVisible)
        ModifyStyleEx(0, WS_EX_APPWINDOW);
    else
        ModifyStyleEx(WS_EX_APPWINDOW, 0);

    ShowWindow(SW_SHOW);

    return TRUE;
}
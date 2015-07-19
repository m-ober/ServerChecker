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


// ServerDoc.cpp : implementation file
//

#include "stdafx.h"
#include "ServerCheckerGUI.h"
#include "Control.h"
#include "ServerDoc.h"
#include <mmsystem.h>


DWORD WINAPI check(void* param) // this function is run in a different thread
{
	TRACE("check() called\r\n");
	ServerControl* sc = (ServerControl*)param;
	bool* control = sc->getControl();
	*control = true;
	while (*control)
	{
		try
		{
			sc->start();
		}
		catch (char* str_error)
		{
			TRACE("error in start(): %s",str_error);
			MessageBox(0,str_error,"Error...",MB_OK | MB_ICONEXCLAMATION);
			Sleep(10000);
		}
		catch (errors n_err)
		{
			TRACE("error in start(): %d",n_err);
			if (n_err == INVALID_EXECUTABLE)
			{
				MessageBox(NULL,"The executable specified is invalid.\r\nCheck if the path exists and there are no typing errors.","Error...",MB_OK | MB_ICONEXCLAMATION);
				return -1; // would be dumb trying to start a bad executable over and over again, right ?
			}
			else if (n_err == INVALID_PATH)
			{
				MessageBox(NULL,"The system could not find the path specified.","Error...",MB_OK | MB_ICONEXCLAMATION);
				return -1;
			}
			else if (n_err == NORMAL_PROGRAM_ERROR)
			{
				char* buff = new char[1024];
				FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,GetLastError(),0,buff,1024,0);
				CString message;
				message.Format("An error occurred while monitoring a normal program.\r\nError: %s",buff);
				MessageBox(NULL, message.GetBuffer(),"Error...",MB_OK | MB_ICONEXCLAMATION);
				delete[] buff;
			}
		}
		catch(...)
		{
			TRACE("unknown exception caught");
		}
	}
	return 0;
}
// CServerDoc

IMPLEMENT_DYNCREATE(CServerDoc, CDocument)

CServerDoc::CServerDoc()
: m_SelectedServer(-1), pingboostervalue(1), pingboost(false)
{
}
/*
DWORD WINAPI runremote(void* arg)
{
	CRemote* r = (CRemote*)arg;
	bool* run = r->getRun();
	while (*run)
	{
		try
		{
			r->Run();
		}
		catch (char* ex)
		{
			MessageBox(NULL,ex,"Error...",MB_OK | MB_ICONEXCLAMATION);
		}
	}
	return 0;
}*/

BOOL CServerDoc::OnNewDocument()
{
	stopAllServers();
	m_Servers.RemoveAll();
	if (!CDocument::OnNewDocument())
		return FALSE;
	
	SetTitle("Servers");

	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	
	strAppDirectory += "\\servers.sch";

	bool prevversion = false;
	try // deserializing
	{
		CFile f(strAppDirectory,CFile::modeRead);
		CArchive ar(&f,CArchive::load);
	
		ServerControl* sc;
		int nr;
		ar >> nr;
		for (int i = 0; i <= nr; i++)
		{
			sc = (ServerControl*)ar.ReadObject(RUNTIME_CLASS(ServerControl));
			m_Servers.Add(sc);
		}
		prevversion = true;

		int tmp;
		ar >> tmp;
		pingboostervalue = (UINT)tmp;
		ar >> tmp;
		bool b = (bool)tmp;
		boost(b);
	}
	catch(CException* ex)
	{
		if (!prevversion)
		{
			ex->ReportError();
			MessageBox(NULL,"Error loading servers from file","Error...",MB_OK | MB_SYSTEMMODAL |MB_ICONEXCLAMATION);
			m_Servers.RemoveAll();
		}
	}
	/*remoterun = new bool;
	*remoterun = true;
	try
	{
		remote = new CRemote(this,remoterun);
		HANDLE threadhandle = CreateThread(NULL,0,&runremote,(void*)remote,0,NULL);
		remote->setThreadHandle(threadhandle);
	}
	catch (char* ex)
	{
		MessageBox(NULL,ex,"error",MB_OK | MB_ICONEXCLAMATION);
	}	*/
	return TRUE;
}
ServerControl* CServerDoc::getServer(int n)
{
	if (n >= 0 && n <= m_Servers.GetUpperBound())
		return m_Servers[n];
	return NULL;
}

CServerDoc::~CServerDoc()
{
	//*remoterun = false;
	//delete remote;
	stopAllServers();
	writeFile(true);
	boost(false);
}

void CServerDoc::writeFile(bool cleanup)
{
	char    szAppPath[MAX_PATH] = "";
	CString strAppDirectory;

	::GetModuleFileName(0, szAppPath, sizeof(szAppPath) - 1);

	strAppDirectory = szAppPath;
	strAppDirectory = strAppDirectory.Left(strAppDirectory.ReverseFind('\\'));
	
	strAppDirectory += "\\servers.sch";

	CFile f(strAppDirectory,CFile::modeCreate | CFile::modeWrite);
	CArchive ar(&f,CArchive::store);
	
	INT_PTR max = m_Servers.GetUpperBound();
	ar << (int)max;
	for (INT_PTR i = 0; i <= max; i++)
	{
		ServerControl* sc = m_Servers[i];
		ar.WriteObject(sc);
		if (cleanup) delete sc;
	}
	ar << (int)pingboostervalue;
	ar << (int)pingboost;
	if (cleanup) m_Servers.RemoveAll();
	ar.Close();
	f.Close();
}

BEGIN_MESSAGE_MAP(CServerDoc, CDocument)
END_MESSAGE_MAP()

// CServerDoc diagnostics

#ifdef _DEBUG
void CServerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CServerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CServerDoc serialization

void CServerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}


// CServerDoc commands

int CServerDoc::getServerCount(void)
{
	return (int)m_Servers.GetCount();
}

void CServerDoc::SelectServer(int nr)
{
	m_SelectedServer = nr;
	this->UpdateAllViews(NULL);
}

ServerControl* CServerDoc::getSelectedServer(void)
{
	if (m_SelectedServer >=0 && m_SelectedServer <= m_Servers.GetCount())
		return m_Servers[m_SelectedServer];
	return NULL;
}

BOOL CServerDoc::startServer(ServerControl* server)
{
	ASSERT(server);
	if (server->isActive())
		return TRUE;
	
	HANDLE handle = CreateThread(NULL,0,&check,(void*)server,0,0);
	server->setHandle(handle);

	UpdateAllViews(NULL);
	return TRUE;
}

void CServerDoc::stopAllServers(bool dontStop)
{
	for (int i = 0; i <= m_Servers.GetUpperBound(); i++)
	{
		ServerControl* sc = m_Servers[i];
		if (sc)
		{
			sc->dontStopServer(dontStop);
			stopServer(sc);
		}
	}
}

void CServerDoc::stopServer(ServerControl* server)
{
	ASSERT(server);
	if (!server->isActive())
		return;
	bool* go = server->getControl();
	*go = false;
	HANDLE threadhandle = server->getHandle();
	int time = server->getStartup() + 2* server->getPause(); // time is in seconds
	DWORD res = WaitForSingleObject(threadhandle,time*1000);
	if (res == WAIT_TIMEOUT) // thread is still busy
	{
		TerminateThread(threadhandle, 1);
		CloseHandle(threadhandle);
	}
	else if (res == WAIT_OBJECT_0)
	{
		CloseHandle(threadhandle);
	}	
	UpdateAllViews(NULL);
}

int CServerDoc::getServerNumber(ServerControl* server)
{
	int i = 0;
	while (i <= m_Servers.GetUpperBound())
	{
		if (server == getServer(i))
		{
			return i;
		}
		i++;
	}
	return -1;
}



void CServerDoc::delSelectedServer(void)
{
	ServerControl* sc = getSelectedServer();
	stopServer(sc);
	m_Servers.RemoveAt(m_SelectedServer);
	delete sc;
	m_SelectedServer -= 1;
	writeFile(false);
}

int CServerDoc::getSelectedServerNumber(void)
{
	return getServerNumber(getSelectedServer());
}

void CServerDoc::addServer(ServerControl* server)
{
	m_Servers.Add(server);
	writeFile(false);
}

int CServerDoc::runningServers(void)
{
	int ret = 0;
	for (INT_PTR i = 0; i <= m_Servers.GetUpperBound(); i++)
	{
		ServerControl* sc = m_Servers.GetAt(i);
		if (sc && sc->isActive())
			ret++;
	}
	return ret;
}

void CServerDoc::startAllServers(void)
{
	for (int i = 0; i <= m_Servers.GetUpperBound(); i++)
	{
		ServerControl* sc = m_Servers[i];
		if (sc)
		{
			startServer(sc);
		}
	}
}

void CServerDoc::boost(bool on)
{
	if (pingboost != on) // prevent from 1 function being called without the other
	{
		pingboost = on;
		if (on)
			timeBeginPeriod(pingboostervalue);
		else
			timeEndPeriod(pingboostervalue);
	}
}

bool CServerDoc::boosts()
{
	return pingboost;
}

void CServerDoc::setBoosterValue(int value)
{
	if (!pingboost)
		pingboostervalue = value;
}

int CServerDoc::getBoosterValue(void)
{
	return pingboostervalue;
}

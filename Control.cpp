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


#include "stdafx.h"
#include "ServerCheckerGUI.h"
#include "Control.h"
#include "UDPClient.h"
#include ".\control.h"

/*
static const CString PINGS[] = { "\xFF\xFF\xFF\xFF\x54\x53\x6F\x75\x72\x63\x65\x20\x45\x6E\x67\x69\x6E\x65\x20\x51\x75\x65\x72\x79\x00"
								};*/
static const char* PINGS[] = {"\xFF\xFF\xFF\xFF\x54\x53\x6F\x75\x72\x63\x65\x20\x45\x6E\x67\x69\x6E\x65\x20\x51\x75\x65\x72\x79\x00", // HL/HL2
								"\x5C\x62\x61\x73\x69\x63\x5C", // UT/GS
								"\xFE\xFD\x00\x43\x4F\x52\x59\xFF\xFF\x00",	// GS2
								"\xFE\xFD\x00\x0C\xAE\x3D\x00\xFF\xFF\xFF\x01", // GS3 //"\xFE\xFD\x00\x04\xF7\x11\x00\x09\x04\x05\x06\x07\x0B\x01\x08\x0a\x13\x00\x00",
								"\xFF\xFF\xFF\xFF\x67\x65\x74\x73\x74\x61\x74\x75\x73", // Q3
								"\xFF\xFF\xFF\xFF\x73\x74\x61\x74\x75\x73", // Q2
								"\xFF\xFF\x67\x65\x74\x49\x6E\x66\x6F", //Doom3
								"\x73", // All Seeing Eye
								"\x52\x45\x50\x4F\x52\x54", // Raven Shield
								"" // Normal Program
								};
static const int PINGLEN[] = { 25, 7, 10, 11, 13, 10, 9, 1, 6, 0 };

const   int BSIZE=1500; // ~ ethernet MTU

const   int DEFAULT_TIMEOUT = 1000; // time in msec to wait for ping
const	int DEFAULT_STARTTIME= 30000; // time we give the process to start up
const	int DEFAULT_PAUSE = 3000; // time between pings

//can throw DNS resolver error
ServerControl::ServerControl(CString host, unsigned short _port, CString _executable, int type) : parent(NULL),
  client(), executable(_executable), ping_type(type), time_pause(DEFAULT_PAUSE), time_startup(DEFAULT_STARTTIME), 
  busy(false), priority(NORMAL), no_kill(false), processhandle(NULL), threadhandle(NULL), affinity(0), name(_T("")), processid(0),
  timecontrol(false)
{
	serv = UDPClient::getHost(host.GetBuffer(), _port);
	tval.tv_sec = DEFAULT_TIMEOUT/1000;
	tval.tv_usec = 0;
	buff = new char[BSIZE];
	go = new bool;
	*go = false;
}

// 0x200 first version
// 0x201 time-adjusted
IMPLEMENT_SERIAL(ServerControl,CObject, VERSIONABLE_SCHEMA | 0x201);

void ServerControl::start()
{
	try
	{
		TRACE("start() called\r\n");
		if (busy)
			throw "Server is already being monitored";
		busy = true;
		updateStatus(STATUS_UNKNOWN,STATUS_START);
		createProcess();
		updateStatus(STATUS_UP,STATUS_OK);
		
		int counter = 1;
		while (*go)
		{
			Sleep(time_pause); // wait 3 seconds
			try
			{
				if (!pingOnce())
				{
					counter+=2; // always add 2 then subtract 1 at end of loop
					updateStatus(STATUS_UNKNOWN,STATUS_NO_REPLY);
				}
			}
			catch (char*) // on errors in the pingonce, something bad happened so we let the process respawn immediately
			{
				updateStatus(STATUS_DOWN,STATUS_NO_REPLY);
				counter = 11;
			}
			
			if (counter > 10) // server has been gone for too long, or error
			{
				updateStatus(STATUS_DOWN,STATUS_STOP);
				endProcess();
				
				updateStatus(STATUS_UNKNOWN,STATUS_START);
				counter = 0;
				createProcess();
				updateStatus(STATUS_UP,STATUS_OK);
			}
			if (counter > 0) // subtract one to lower the number back to 0 when server is up (mapchanges etc)
			{
				if (counter == 1)
					updateStatus(STATUS_UP,STATUS_OK);
				counter--;
			}
		}
		
		if (!no_kill) // turned on if servers should remain active after program end
		{
			updateStatus(STATUS_UNKNOWN,STATUS_STOP);
			endProcess();
			updateStatus(STATUS_DOWN,STATUS_STOPPED);
		}
		
		busy = false;
		TRACE("start() stopped");
	}
	catch (...)
	{
		busy = false; // whatever happens, we're not busy anymore!
		throw;
	}
}

bool ServerControl::pingOnce()
{
	if (ping_type != NORMAL_PROGRAM_INDEX)
	{
		TRACE("pinging\r\n");
		FD_ZERO(&set);
		client.SET(&set);
		struct sockaddr_in peer;
		
		client.sendTo(&serv,PINGS[ping_type],PINGLEN[ping_type]);//PINGS[ping_type].GetLength()); // send data
		
		bool ret = false;
		int n = BSIZE;
		while (n == BSIZE && select(0,&set,NULL,NULL,&tval) > 0) // if buffer is full then more data may be present... flush it before proceeding
		{
			n = client.recvFrom(&peer,buff,BSIZE); // connection reset is given when the process stops => exception
			if (memcmp((void*) &(peer.sin_addr),(void*) &(serv.sin_addr), sizeof(in_addr))==0   &&   peer.sin_port == serv.sin_port) // only if data came from host we're checking
			{
				ret = true;
			}
			FD_ZERO(&set);
			client.SET(&set);
		}
		return ret;
	}
	else
	{
		DWORD exitcode;
		if (GetExitCodeProcess(processhandle,&exitcode))
		{
			if (exitcode != STILL_ACTIVE)
				throw "Program has stopped"; // no need to use the counter and wait
			else
				return true;
		}
		else // shouldn't happen, but program it anyway.
			throw NORMAL_PROGRAM_ERROR; // catched in check() - serverdoc.cpp
	}
}


void ServerControl::createProcess()
{
	TRACE("starting process\r\n");
	/*BOOL CreateProcess(
			  LPCTSTR lpApplicationName,					 // pointer to name of executable module
			  LPTSTR lpCommandLine,  					// pointer to command line string
			  LPSECURITY_ATTRIBUTES lpProcessAttributes,  	// process security attributes
			  LPSECURITY_ATTRIBUTES lpThreadAttributes,   	// thread security attributes
			  BOOL bInheritHandles, 					// handle inheritance flag
			  DWORD dwCreationFlags, 					// creation flags
			  LPVOID lpEnvironment,  					// pointer to new environment block
			  LPCTSTR lpCurrentDirectory,   				// pointer to current directory name
			  LPSTARTUPINFO lpStartupInfo, 				// pointer to STARTUPINFO
			  LPPROCESS_INFORMATION lpProcessInformation  	// pointer to PROCESS_INFORMATION
	);*/
	// prepare structs
	PROCESS_INFORMATION procinfo;
	STARTUPINFO si;
	memset(&si,0,sizeof(si));
	memset(&procinfo,0,sizeof(procinfo));
	si.cb = sizeof(si);
	si.dwFlags |= STARTF_USESHOWWINDOW;
	si.wShowWindow = SW_MINIMIZE;
	
	// try creating the process
	BOOL func_ok;
	int n = executable.ReverseFind('\\');
	if (n >= 0)
	{
		CString path = executable.Left(n+1);
		func_ok = CreateProcess(NULL, executable.GetBuffer(), NULL, NULL, false, CREATE_NEW_CONSOLE | priority , NULL, path.GetBuffer(), &si, &procinfo);
	}
	else
		func_ok = CreateProcess(NULL, executable.GetBuffer(), NULL, NULL, false, CREATE_NEW_CONSOLE | priority , NULL, NULL, &si, &procinfo);

	if (func_ok)
	{
		processhandle = procinfo.hProcess;
		threadhandle = procinfo.hThread;
		processid = procinfo.dwProcessId;

		int t = (int)(time_startup*2.0/3000); // 2/3d of the time "just" waiting
		for (int i = 0; *go && i<t; i++)
		{ // waiting behaviour updated to allow stopping while starting up
			Sleep(1000);
		}
		DWORD res = WaitForInputIdle(processhandle,1000); //check whether process is waiting for input
		if (res == WAIT_TIMEOUT) // if it isn't, wait another time_startup/2 for it
		{
			for (int j = 0; *go && j<(t/2) && (res!=0); j++)
			{
                res = WaitForInputIdle(processhandle,1000);
			}
		}

		// added: processor affinity
		// moved here. 
		this->SetAffinityMask((DWORD)1); // bind to proc 1
		this->SetAffinityMask(0); // unbind
	}
	else
	{
		updateStatus(STATUS_DOWN,STATUS_STOPPED);
		int lasterror = GetLastError();
		if (lasterror == 2)
		{
			throw INVALID_EXECUTABLE; // the executable doesn't exist
		}
		else if (lasterror == 3)
		{
			throw INVALID_PATH;
		}
		else
		{
			
			TRACE("CreateProcess returned error code %d\r\n",lasterror);
			throw "Could not create Process...";
		}
	}
}

bool ServerControl::endProcess()
{
	TRACE("Stopping process\r\n");
	processid = 0;
	DWORD exitcode; // try getting the exitcode from the process in a soft way...
	BOOL func_ok = GetExitCodeProcess(processhandle,&exitcode);
	if (func_ok)
	{
		if (exitcode == STILL_ACTIVE) // we're gonna have to do it the hard way
		{
			BOOL ret = TerminateProcess(processhandle, 1);
			CloseHandle(processhandle); // cleanup
			CloseHandle(threadhandle);
			return ret;
		}
		// else process has exited itself
	}
	else
	{
		DWORD error_code = GetLastError();
		TRACE("GetExitCodeProcess return with value %d\r\n",error_code);
	}
	CloseHandle(processhandle);
	CloseHandle(threadhandle);
	processhandle = 0;
	threadhandle = 0;
	return func_ok;
}

ServerControl::~ServerControl()
{
	TRACE("destructor ServerControl\r\n");
	delete[] buff;
	delete go;
}


void ServerControl::updateStatus(UP _up, STATUS _status)
{
	if (parent)
	{
		up.push(_up);
		status.push(_status);
		parent->PostMessage(ID_SERVER_UPDATE,(WPARAM)this,NULL);
		TRACE("%s %d",Status[_status],_up);
	}
	else
	{
		TRACE("%s %d",Status[_status],_up);
	}
}

CArchive & operator<< ( CArchive & os, const SYSTEMTIME& st )
{
	os	<<st.wDay
		<<st.wDayOfWeek
		<<st.wHour
		<<st.wMilliseconds
		<<st.wMinute
		<<st.wMonth
		<<st.wSecond
		<<st.wYear;
	return os;
}
CArchive & operator>> ( CArchive & is, SYSTEMTIME& st )
{
	is	>>st.wDay
		>>st.wDayOfWeek
		>>st.wHour
		>>st.wMilliseconds
		>>st.wMinute
		>>st.wMonth
		>>st.wSecond
		>>st.wYear;
	return is;
}

void ServerControl::Serialize(CArchive& ar)
{
	CObject::Serialize(ar);
	if (ar.IsStoring())
	{
		ar << getServerIP();
		ar << getPort();
		ar << name;
		ar << ping_type;
		ar << executable;
		ar << getTimeOut();
		ar << time_pause;
		ar << time_startup;
		ar << (int)priority;
		ar << starttime;
		ar << stoptime;
		ar << timecontrol;
	}
	else
	{
		CString server;
		unsigned short port;
		ar >> server;
		ar >> port;
		setServer(server,port);

		ar >> name;
		ar >> ping_type;
		ar >> executable;

		int ping_timeout;
		ar >> ping_timeout;
		setTimeOut(ping_timeout);

		ar >> time_pause;
		ar >> time_startup;
		int prior;
		ar >> prior;
		priority = (PRIORITY)prior;
		UINT version = ar.GetObjectSchema();
		CTime _starttime, _stoptime;
		switch (version)
		{
		case 0x200:
			
			ar >> _starttime;
			ar >> _stoptime;
			_starttime.GetAsSystemTime(starttime);
			_stoptime.GetAsSystemTime(stoptime);
			break;
		case 0x201:
			ar >> starttime;
			ar >> stoptime;
			break;
		default:
			memset(&starttime,0,sizeof(SYSTEMTIME));
			memset(&stoptime,0,sizeof(SYSTEMTIME));
		}
		ar >> timecontrol;
	}
}

ServerControl::ServerControl(void) : parent(NULL),
  client(), time_pause(DEFAULT_PAUSE), time_startup(DEFAULT_STARTTIME), 
  busy(false), priority(NORMAL), no_kill(false), processhandle(NULL), 
  threadhandle(NULL), affinity(0), name(_T("")), processid(0), timecontrol(false)
{
	tval.tv_sec = DEFAULT_TIMEOUT/1000;
	tval.tv_usec = 0; // microseconds!
	buff = new char[BSIZE];
	go = new bool;
	*go = false;
}

DWORD ServerControl::GetSystemAffinityMask(void)
{
	HANDLE procHandle = GetCurrentProcess();
	DWORD procAff, sysAff;
	if (GetProcessAffinityMask(procHandle, &procAff, &sysAff))
	{
		return sysAff;
	}
	return (DWORD)1; // assume 1 processor
}

DWORD ServerControl::GetAffinityMask(void)
{
	return affinity;
}

void ServerControl::SetAffinityMask(DWORD mask)
{
	DWORD sysaffinity = GetSystemAffinityMask();
	affinity = (mask & sysaffinity ? (mask & sysaffinity) : sysaffinity);
	if (processhandle)
		SetProcessAffinityMask(processhandle,affinity);
}

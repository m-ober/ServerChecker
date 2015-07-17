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
#ifndef __SERVERCONTROL
#define __SERVERCONTROL

#include <windows.h>
#include <queue>
//#include <iostream>
//#include <cstdio>
#include "UDPClient.h"
/*
static const CString GameTypes[] = {"HL/HL2"};
const	int NR_GAMES = 1;
*/
static const CString GameTypes[] = {"HL/HL2 Based",
									"GameSpy", 
									"GameSpy2",
									"GameSpy3",
									"Quake III Based", 
									"Quake II Based",
									"Doom 3",
									"All Seeing Eye",
									"Raven Shield",
									"Normal Program"
									};
const int NR_GAMES = 10;
const int NORMAL_PROGRAM_INDEX = 9;

const enum STATUS
{
	STATUS_OK = 0,
	STATUS_START = 1,
	STATUS_NO_REPLY = 2,
	STATUS_STOP = 3,
	STATUS_STOPPED = 4
};
static const CString Status[] = {"OK","Starting Up...","No Reply","Stopping Server...","Stopped"};

const enum UP
{
	STATUS_UP,
	STATUS_DOWN,
	STATUS_UNKNOWN // = no reply etc
};
static const CString Up[] = {"UP","Down","Unknown"};

const int INVALID_STATUS_UP = -1;

const enum PRIORITY
{
	HIGH = HIGH_PRIORITY_CLASS,
	ABOVE_NORMAL = ABOVE_NORMAL_PRIORITY_CLASS,
	NORMAL = NORMAL_PRIORITY_CLASS,
	LOW = BELOW_NORMAL_PRIORITY_CLASS
};
static const CString Priorities[] = {"High","Above Normal","Normal","Low"};

enum errors
{
	INVALID_EXECUTABLE = 0,
	NORMAL_PROGRAM_ERROR = 1,
	INVALID_PATH = 2
};

using namespace std;

class ServerControl : public CObject
{
public: //constructors
	DECLARE_SERIAL(ServerControl);
	ServerControl(const char* file); // to be implemented
	ServerControl(CString host, unsigned short _port, CString _executable, int type);
	~ServerControl();
public: // setters
	ServerControl* setTimeOut(int timeout);
	ServerControl* setPause(int pause);
	ServerControl* setStartUp(int startup);
	ServerControl* setGame(int game);
	ServerControl* setServer(CString host, unsigned short port);
	ServerControl* setExecutable(CString exec);
	ServerControl* setServerName(CString newname);
	//ServerControl* SetStartStopTime(CTime& newstarttime, CTime& newstoptime);
	ServerControl* SetStartStopTime(SYSTEMTIME& newstarttime, SYSTEMTIME& newstoptime);
	ServerControl* SetTimeControlled(bool on);
	void setHandle(HANDLE handle);
	ServerControl* setParent(CWnd* _parent);
	bool setPriority(PRIORITY prior);
	void dontStopServer(bool on);
	// getters
	bool isActive(void);
	CString getServerIP(void);
	int getTimeOut(void);
	int getPause(void);
	int getStartup(void);
	unsigned short getPort(void);
	int getGameType(void);
	int getStatus(void);
	int getUp(void);
	//CTime GetStartTime();
	SYSTEMTIME GetStartTime();
	//CTime GetStopTime();
	SYSTEMTIME GetStopTime();
	bool IsTimeControlled();
	DWORD getProcessID(void);
	CString getServerName(void);
	PRIORITY getPriority(void);
	CString getExecutable(void);
	HANDLE getHandle(void);
	bool* getControl();
	// main !! run in diff thread
	void start();
	
private:
	ServerControl(void); // for deserializing
	void updateStatus(UP _up, STATUS _status);
	void createProcess();
	bool endProcess();
	bool pingOnce();
private: // everything about the process spawned
	HANDLE processhandle;
	HANDLE threadhandle;
	DWORD processid;
	CString executable;
private: // everything about the pinging
	UDPClient client;
	fd_set set;
	timeval tval;
	struct sockaddr_in serv;
	char* buff;
private: // everything about me
	bool* go;
	bool busy;
	queue<int> up;
	queue<int> status;
	PRIORITY priority;
	bool no_kill;
private: // constants moved from global to private
	int ping_type;
	int time_pause;
	int time_startup;
	HANDLE myHandle;
	CWnd* parent;
	DWORD affinity;
	CString name;
	//CTime starttime;
	//CTime stoptime;
	SYSTEMTIME starttime, stoptime;
	bool timecontrol;
public:
	void Serialize(CArchive& ar);
	static DWORD GetSystemAffinityMask(void);
	DWORD GetAffinityMask(void);
	void SetAffinityMask(DWORD mask);
};

inline ServerControl* ServerControl::setTimeOut(int timeout) // in msec
{
	if(!busy)
	{
		tval.tv_sec = timeout;
		tval.tv_usec = 0;
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}

inline ServerControl* ServerControl::setPause(int pause)
{
	if(!busy)
	{
		time_pause = pause*1000;
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}

inline ServerControl* ServerControl::setStartUp(int startup)
{
	if(!busy)
	{
		time_startup = startup*1000;
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}
inline ServerControl* ServerControl::setGame(int game)
{
	if(!busy)
	{
		ping_type = game;
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}

inline CString ServerControl::getServerIP(void)
{
	return CString(inet_ntoa(serv.sin_addr));
}

inline int ServerControl::getTimeOut(void)
{
	return tval.tv_sec;
}

inline int ServerControl::getPause(void)
{
	return time_pause/1000;
}

inline int ServerControl::getStartup(void)
{
	return time_startup/1000;
}

inline unsigned short ServerControl::getPort(void)
{
	return ntohs(serv.sin_port);
}

inline CString ServerControl::getExecutable(void)
{
	return CString(executable);
}

inline bool* ServerControl::getControl()
{
	return go;
}

inline bool ServerControl::isActive(void)
{
	return busy;
}

inline int ServerControl::getGameType(void)
{
	return ping_type;
}

inline ServerControl* ServerControl::setServer(CString host, unsigned short port)
{
	if (!busy)
	{
		serv = UDPClient::getHost(host,port);
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}

inline ServerControl* ServerControl::setExecutable(CString exec)
{
	if (!busy)
	{
		executable = exec;
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}

inline void ServerControl::setHandle(HANDLE handle)
{
	myHandle = handle;
}

inline HANDLE ServerControl::getHandle(void)
{
	return myHandle;
}

inline ServerControl* ServerControl::setParent(CWnd* _parent)
{
	parent = _parent;
	return this;
}

inline int ServerControl::getStatus(void)
{
	if (status.size() == 0)
		return INVALID_STATUS_UP;
	STATUS tmp = (STATUS)status.front();
	status.pop();
	return tmp;
}

inline int ServerControl::getUp(void)
{
	if (up.size() == 0)
		return INVALID_STATUS_UP;
	UP tmp = (UP)up.front();
	up.pop();
	return tmp;
}

inline bool ServerControl::setPriority(PRIORITY prior)
{
	priority = prior;
	if (busy)
	{
		return SetPriorityClass(processhandle,priority);
	}
	return true;
}

inline PRIORITY ServerControl::getPriority(void)
{
	return priority;
}

inline void ServerControl::dontStopServer(bool on)
{
	no_kill = on;
}

inline ServerControl* ServerControl::setServerName(CString newname)
{
	if (!busy)
	{
		name = newname;
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}

inline CString ServerControl::getServerName(void)
{
	return name;
}

inline DWORD ServerControl::getProcessID(void)
{
	return processid;
}

inline SYSTEMTIME /*CTime*/ ServerControl::GetStartTime()
{
	return starttime;
}
inline SYSTEMTIME /*CTime*/ ServerControl::GetStopTime()
{
	return stoptime;
}
inline bool ServerControl::IsTimeControlled()
{
	return timecontrol;
}
//inline ServerControl* ServerControl::SetStartStopTime(CTime& newstarttime, CTime& newstoptime)
inline ServerControl* ServerControl::SetStartStopTime(SYSTEMTIME& newstarttime, SYSTEMTIME& newstoptime)
{
	if (!busy)
	{
		starttime = newstarttime;
		stoptime = newstoptime;
	}
	else
	{
		throw "Server is currently being monitored. cannot alter value";
	}
	return this;
}
inline ServerControl* ServerControl::SetTimeControlled(bool on)
{
	timecontrol = on;
	return this;
}
#endif
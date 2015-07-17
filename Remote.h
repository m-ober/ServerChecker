#pragma once

#ifndef Remote
#define Remote

class CServerDoc;

#include "TCPListener.h"
#include "ServerDoc.h"

class CRemote
{
private:
	TCPListener* listener;
	CServerDoc* document;
	bool* run;
	bool disposable;
	HANDLE h;
public:
	CRemote(CServerDoc* _doc, bool* _run);
	void Run();
	void setThreadHandle(HANDLE _h);
	HANDLE getThreadHandle();
	bool* getRun();
	virtual ~CRemote(void);
};

inline void CRemote::setThreadHandle(HANDLE _h)
{
	h = _h;
}

inline HANDLE CRemote::getThreadHandle()
{
	return h;
}

inline bool* CRemote::getRun()
{
	return run;
}

#endif
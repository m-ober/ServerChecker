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
#ifndef serverdocske
#define serverdocske

#include "afxtempl.h"
#include "Control.h"
//#include "Remote.h"
// CServerDoc document

class CServerDoc : public CDocument
{
	DECLARE_DYNCREATE(CServerDoc)

public:
	CServerDoc();
	ServerControl* getServer(int n);
	virtual ~CServerDoc();
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual BOOL OnNewDocument();

	DECLARE_MESSAGE_MAP()
private:
	//CRemote* remote;
	//bool* remoterun;
	bool* go;
	CArray<ServerControl*> m_Servers;
	UINT pingboostervalue;
	bool pingboost;
public:
	int getServerCount(void);
	void SelectServer(int nr);
	int m_SelectedServer;
	ServerControl* getSelectedServer(void);
	BOOL startServer(ServerControl* server);	
	void stopAllServers(bool dontStop = false);
	void stopServer(ServerControl* server);
	int getServerNumber(ServerControl* server);
	void delSelectedServer(void);
	int getSelectedServerNumber(void);
	void addServer(ServerControl* server);
	int runningServers(void);
	void startAllServers(void);
	void boost(bool on);
	bool boosts();
	void setBoosterValue(int value);
	int getBoosterValue(void);
	void writeFile(bool cleanup);
};

#endif
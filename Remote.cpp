#include "StdAfx.h"
#include "Remote.h"
#include "serverdoc.h"
#include "control.h"

static const CString head = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<servers>";
static const CString server = "<server><commandline>%s</commandline></server>";
static const CString tail = "</servers>";
const int port = 26015;

CRemote::CRemote(CServerDoc* _doc, bool* _run) : document(_doc), run(_run), h(NULL)
{
	listener = new TCPListener();
	listener->StartupDefault(port);
	listener->SetNonBlocking(true);
}

void CRemote::Run()
{
	TCPClient* client = listener->Accept(NULL);
	while (*run)
	{
		if (client != NULL)
		{
			client->Send(head,head.GetLength());
			CString myServer;
			for (int i = 0; i < document->getServerCount(); i++)
			{
				myServer.Format(server,document->getServer(i)->getExecutable());
				client->Send(myServer, myServer.GetLength());
			}
			client->Send(tail,tail.GetLength());
			delete client;
			client = NULL;
		}
		Sleep(1000); // timeout for reconnects
		client = listener->Accept(NULL);
	}
}

CRemote::~CRemote(void)
{
	if (*run)
		*run = false;
	if (h)
	{
		int count = 5;
		while (WaitForSingleObject(h,1000) == WAIT_TIMEOUT && count)
		{
			count--;
		}
		if (!count)
			TerminateThread(h,1);
	}

	delete listener;
	document = NULL;
}

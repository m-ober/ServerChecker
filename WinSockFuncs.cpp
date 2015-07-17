#include "stdafx.h"
#include "winsockfuncs.h"
#include "winsock2.h"

void WinSockInit()
{
	WSAData wsadata;
	int res = WSAStartup(0x0202,&wsadata);
	if (res != 0)
	{
		perror("WinSockInit");
		throw "Winsock Startup failed";
	}
}

int WinSockError()
{
	int ret = WSAGetLastError();
	WSASetLastError(0);
	return ret;
}

// let er op dat ALLE sockets gesloten zijn
// voor deze functie opgeroepen wordt!!
void WinSockClean()
{
	int res = WSACleanup();
	if (res != 0)
	{
		perror("WinSockClean");
		throw "Winsock Cleanup failed";
	}
}
#ifndef __TCPClient_INCLUDED
#define __TCPClient_INCLUDED

#include <windows.h>
#include <winsock2.h>
#include "winsockfuncs.h"

#include <errno.h>
#define SOCKADDR_IN_SIZE 16

class TCPClient
{
public:
	static struct sockaddr_in getHost(const char* host, unsigned short port);
public:
	TCPClient(int sock);
	TCPClient();
	void Connect(const struct sockaddr_in *serv_addr);
	void SET(fd_set* set);
	bool ISSET(fd_set* set);
	void CLR(fd_set* set);
	void SetNonBlocking(bool on);
	void Send(const char *msg, size_t len);
	int Recv(char *buf, size_t len);
	void Shutdown(int how); // SD_SEND of SD_RECEIVE of SD_BOTH
	virtual ~TCPClient();
private:
	void Close();
	int state;
	int sock;
};

#endif

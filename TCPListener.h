#ifndef __TCPListener_INCLUDED
#define __TCPListener_INCLUDED

#include "TCPClient.h"

class TCPListener
{
public:
	TCPListener();
	void StartupDefault(unsigned int prt);
	void Bind(unsigned short prt, sockaddr_in* my_addr = NULL, int addrlen = SOCKADDR_IN_SIZE);
	void Listen(int backlog);
	void SET(fd_set* set);
	bool ISSET(fd_set* set);
	void CLR(fd_set* set);
	void SetNonBlocking(bool on);
	TCPClient* Accept(sockaddr_in* client);
	virtual ~TCPListener();
private:
	void Close();
	int sock;
	int state;
};

#endif

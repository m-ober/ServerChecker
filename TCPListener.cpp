#include "stdafx.h"
#include "TCPListener.h"

TCPListener::TCPListener() : state(0), sock(-1)
{
	struct	protoent *ptrp = getprotobyname("tcp");
	if (ptrp == NULL)
	{
		perror("GetProtoByName");
		throw "GetProtoByName: couldn't find TCP";
	}
	sock = socket(PF_INET, SOCK_STREAM, ptrp->p_proto);
	if (sock < 0)
	{
		perror("Socket");
		throw "Socket: Could not create socket";
	}
	state++; // 1
}

void TCPListener::StartupDefault(unsigned int prt)
{
	Bind(prt);
	Listen(10);
}

void TCPListener::Bind(unsigned short prt, sockaddr_in* my_addr, int addrlen)
{
	if(state != 1)
	{
		throw "Bind: no valid socket.";
	}
	if(my_addr == NULL)
	{
		my_addr = new sockaddr_in;
		memset((char *)my_addr,0,sizeof(addrlen)); 	/* clear sockaddr structure	*/
		my_addr->sin_family = AF_INET;	  				/* set family to Internet	*/
		my_addr->sin_addr.s_addr = INADDR_ANY;			/* set the local IP address	*/
		my_addr->sin_port = htons((u_short)prt);
	}
	
	int yes = 1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(int));
	
	int res = bind(sock,(struct sockaddr*)my_addr,addrlen);
	if (res < 0)
	{
		perror("bind");
		throw "Could not bind to address.";
	}
	state++; // 2
}

void TCPListener::Listen(int backlog)
{
	if (state != 2)
	{
		throw "Listen: Socket was not bound or initialized";
	}
	int res = listen(sock,backlog);
	if (res < 0)
	{
		perror("Listen");
		throw "Listen: Could not set socket to listening mode";
	}
	state++; // 3
}

void TCPListener::SET(fd_set* set)
{
	FD_SET(sock,set);
}

void TCPListener::CLR(fd_set* set)
{
	FD_CLR(sock,set);
}

bool TCPListener::ISSET(fd_set* set)
{
	return FD_ISSET(sock,set);
}

void TCPListener::SetNonBlocking(bool on)
{
	u_long cmd = (u_long)on;
	int res = ioctlsocket(sock,FIONBIO,&cmd);
	if (res < 0)
	{
		perror("SetNonBlocking");
		throw "Could not set Non-Blocking mode";
	}
}

TCPClient* TCPListener::Accept(struct sockaddr_in* client)
{
	if(state != 3)
	{
		throw "Accept: socket hasn't entered listening state.";
	}
	int sasize = SOCKADDR_IN_SIZE;
	int newsock = accept(sock,(struct sockaddr*)client,&sasize);
	if (newsock < 0)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			perror("Accept");
			throw "Error accepting new connection";
		}
		else
			return NULL;
	}
	return new TCPClient(newsock);
}

void TCPListener::Close()
{
	closesocket(sock);
	state = 0;
}

TCPListener::~TCPListener()
{
	Close();
}

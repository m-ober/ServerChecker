#include "stdafx.h"
#include "TCPClient.h"

TCPClient::TCPClient() : state(0), sock(-1)
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

TCPClient::TCPClient(int socket) : state(2), sock(socket)
{
}

void TCPClient::Connect(const struct sockaddr_in *serv_addr)
{
	if (state != 1)
	{
		throw "Connect: no valid socket";
	}
	int res = connect(sock,(const sockaddr*)serv_addr,sizeof(struct sockaddr_in));
	if (res < 0)
	{
		perror("Connect");
		throw "Connect: could not connect to host";
	}
	state++; // 2
}

void TCPClient::Send(const char *msg, size_t len)
{
	if (state != 2)
	{
		throw "Send: no connected socket";
	}
	int res = send(sock,msg,len,0);
	if (res < 0)
	{
		perror("send");
		throw "Send: Could not send data";
	}
}

int TCPClient::Recv(char *buf, size_t len)
{
	if (state != 2)
	{
		throw "Send: no connected socket";
	}
	int res = recv(sock,buf,len,0);
	if (res < 0)
	{
		if (WSAGetLastError() != WSAEWOULDBLOCK)
		{
			perror("recv");
			throw "Recv: Could not Receive data";
		}
	}
	return res;
}

struct sockaddr_in TCPClient::getHost(const char* host, unsigned short port)
{
	struct sockaddr_in sad;
	memset(&sad,0,sizeof(sad));
	sad.sin_family = AF_INET;
	sad.sin_port = htons(port);
	
	struct hostent *ptrh = gethostbyname(host);
	if ( ptrh == NULL)
	{
		perror("gethostbyname");
		throw "error resolving localhost";
	}
	memcpy(&sad.sin_addr, ptrh->h_addr, ptrh->h_length);
	return sad;
}

void TCPClient::SetNonBlocking(bool on)
{
	u_long cmd = (u_long)on;
	int res = ioctlsocket(sock,FIONBIO,&cmd);
	if (res < 0)
	{
		perror("SetNonBlocking");
		throw "Could not set Non-Blocking mode";
	}
}

void TCPClient::SET(fd_set* set)
{
	FD_SET(sock,set);
}

void TCPClient::CLR(fd_set* set)
{
	FD_CLR(sock,set);
}

bool TCPClient::ISSET(fd_set* set)
{
	return FD_ISSET(sock,set);
}

void TCPClient::Shutdown(int how)
{
	if (state != 2)
	{
		throw "Shutdown: no connected socket";
	}
	int res = shutdown(sock,how);
	if (res < 0)
	{
		perror("Shutdown");
		throw "Could not shut down socket";
	}
}

void TCPClient::Close()
{
	if (sock >= 0)
		closesocket(sock);
	state = 0;
}

TCPClient::~TCPClient()
{
	Close();
}
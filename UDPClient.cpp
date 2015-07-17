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
#include <winsock2.h> // winsock2 FIRST, otherwise windows.h includes winsock.h
#include <windows.h>
#include "UDPClient.h"

#include "winsockfuncs.h"

#define SOCKADDR_IN_SIZE 16


struct sockaddr_in UDPClient::getHost(const char* host, unsigned short port)
{
	struct sockaddr_in sad;
	memset(&sad,0,SOCKADDR_IN_SIZE);
	sad.sin_port = htons(port);
	sad.sin_family = AF_INET;
	
	struct hostent* addr = gethostbyname(host);
	if (addr == NULL)
	{
		throw "Error resolving host";
	}
	memcpy(&(sad.sin_addr), addr->h_addr, addr->h_length);

	return sad;
}
char* UDPClient::getHost(struct sockaddr_in* host)
{
	struct hostent* addr = gethostbyaddr((char*)&(host->sin_addr),4,AF_INET);
	if (addr == NULL)
	{
		throw "Reverse DNS failed";
	}
	char* ret = new char[strlen(addr->h_name)+1];
	memcpy(ret,addr->h_name, strlen(addr->h_name)+1);
	return ret;
}

UDPClient::UDPClient() : sock(-1)
{
	struct protoent* p = getprotobyname("udp");
	if (p == NULL)
	{
		perror("getprotobyname");
		throw "could not get protocol.";
	}
	
	sock = socket(AF_INET, SOCK_DGRAM, p->p_proto);
	if (socket < 0)
	{
		perror("socket");
		throw "could not create socket";
	}
}

void UDPClient::setServerMode(unsigned short port)
{
	struct sockaddr_in myaddr = getHost("localhost",port);
	Bind(&myaddr);
}

void UDPClient::Bind(struct sockaddr_in* address)
{
	int b = bind(sock, (struct sockaddr*) address, SOCKADDR_IN_SIZE);
	if (b < 0)
	{
		perror("bind");
		throw "Could not bind to address";
	}
}

int UDPClient::recvFrom(struct sockaddr_in* peer, char* buffer, int maxlen)
{
	int peerlen = SOCKADDR_IN_SIZE;
	int ret = recvfrom(sock, buffer, maxlen, 0, (struct sockaddr*) peer, &peerlen);
	if (ret < 0)
	{
		int err = WinSockError();
		#ifdef WIN32
			if (err == WSAEWOULDBLOCK)
			{
				return 0; // to be changed ?
			}
		#else
			if (err == EWOULDBLOCK)
			{
				return 0; // to be changed ?
			}
		#endif
		#ifdef WIN32
			if (err == WSAECONNRESET)
			{
				throw "Connection reset by peer";
			}
		#else
			if (err == ECONNRESET)
			{
				throw "Connection reset by peer";
			}
		#endif
		perror("recvfrom");
		throw "Error while receiving data";
	}
	return ret;
}

int UDPClient::sendTo(struct sockaddr_in* peer, const char* buffer, int len)
{
	int ret = sendto(sock, buffer, len, 0, (struct sockaddr*) peer, SOCKADDR_IN_SIZE);
	if (ret < 0)
	{
		perror("sendto");
		throw "Error while sending data";
	}
	return ret;
}

void UDPClient::setNonBlocking(bool on)
{
	#ifdef WIN32
		u_long yes = on;
		ioctlsocket(sock,FIONBIO,&yes);
	#else
		int mode = fcntl(sock, F_GETFL, 0);
		if (on)
			mode |= O_NONBLOCK;
		else
			mode &= ~O_NONBLOCK;
		}
		fcntl(sock, F_SETFL, mode);
	#endif
}

void UDPClient::SET(fd_set* set)
{
	FD_SET(sock,set);
}

void UDPClient::CLR(fd_set* set)
{
	FD_CLR(sock,set);
}

bool UDPClient::ISSET(fd_set* set)
{
	return FD_ISSET(sock,set);
}


UDPClient::~UDPClient()
{
	closesocket(sock);
}

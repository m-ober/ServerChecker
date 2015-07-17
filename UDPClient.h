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
#ifndef __UDPCLIENT
#define __UDPCLIENT

class UDPClient
{
public:
	static struct sockaddr_in 	getHost(const char* host, unsigned short port);
	static char*				getHost(struct sockaddr_in* host);
public:
	UDPClient();
	void setServerMode(unsigned short port);
	void Bind(struct sockaddr_in* address);
	int recvFrom(struct sockaddr_in* peer, char* buffer, int maxlen);
	int sendTo(struct sockaddr_in* peer, const char* buffer, int len);
	void SET(fd_set* set);
	bool ISSET(fd_set* set);
	void CLR(fd_set* set);
	void setNonBlocking(bool on);
	virtual ~UDPClient();
private:
	int sock;
};

#endif
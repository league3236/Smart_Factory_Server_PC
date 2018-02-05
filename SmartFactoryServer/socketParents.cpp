#include "socketParents.h"

SocketParents::SocketParents(SOCKET _sock)
	: sock(_sock),label(0) {}

SocketParents::SocketParents(
	ADDRESS_FAMILY family,
	unsigned long __stdcall addr,
	int port) : label(0),lock(false)
{
	sock = socket(PF_INET, SOCK_STREAM, 0);
	memset(&addr_in, 0, sizeof(addr_in));
	addr_in.sin_family = family;
	addr_in.sin_addr.S_un.S_addr = addr;
	addr_in.sin_port = htons(port);
}

SocketParents::~SocketParents() 
{
	printf("delete\n");
}

int SocketParents::close()
{
	int res = closesocket(sock);
	return res;
}

void SocketParents::SetNonBlock() 
{
	u_long t = 1;
	ioctlsocket(sock, FIONBIO, &t);
}

SocketParents::operator SOCKET () 
{
	return sock;
}

void SocketParents::operator =(SOCKET& _sock)
{
	sock = _sock;
}

bool SocketParents::operator ==(SocketParents& socketParents) 
{
	return this->sock == socketParents.sock;
}

bool SocketParents::operator ==(SOCKET sock)
{
	return this->sock == sock;
}

bool SocketParents::operator !=(SocketParents& socketParents) 
{
	return this->sock != socketParents.sock;
}

bool SocketParents::operator !=(SOCKET sock)
{
	return this->sock != sock;
}

#include "socket.h"


Socket::Socket(ADDRESS_FAMILY family, const char* ip, int port)
	: SocketParents(family, inet_addr(ip), port) {

}

Socket::Socket(SOCKET sock)
	: SocketParents(sock)
{

}

Socket::Socket(Socket* sock) 
	: SocketParents(sock->sock) {

}
int Socket::connect() 
{
	SOCKADDR_IN ain = Socket::addr_in;
	return ::connect(Socket::sock,
		(SOCKADDR*)&ain,
		sizeof(Socket::addr_in));
}

int Socket::send(BUFFER buf, int bufSize) 
{
	//SocketParents sp(SOCKET sock);
	int res = ::send((SocketParents::sock), buf, bufSize, 0);
	return res;
}
int Socket::send(HSProtocol* hsp)
{
	//SocketParents sp(SOCKET sock);
	int res = ::send((SocketParents::sock), *hsp, 100, 0);
	return res;
}

int Socket::recv(BUFFER buf, int bufSize) 
{
	int n = 0;
	n = ::recv((SocketParents::sock), buf, bufSize, 0);
	return n;
}

int Socket::recv(HSProtocol* hsp) 
{
		int n = 0;

		n = ::recv((SocketParents::sock), *hsp, 100, 0);
		
		return n;
}


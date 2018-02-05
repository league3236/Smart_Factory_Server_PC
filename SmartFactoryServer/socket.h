#pragma once
#include "socketParents.h"
#include <string>
#include "hsprotocol.h"

#define HS_DO_NOTHING -1
#define HS_NORMAL_SOCKET 0
#define HS_FILE_SOCKET 1

typedef int SocketMode;


class Socket : public SocketParents {
public:
	
	Socket(ADDRESS_FAMILY family, const char* ip, int port);
	Socket(SOCKET sock);
	Socket(Socket* sock);
	int connect();
	int send(BUFFER buf,int bufsize);
	int send(HSProtocol* hsp);
	int recv(BUFFER buf, int bufSize);
	int recv(HSProtocol* hsp);
};


/*

class Socket : public SocketParents {
int mode;
public:
Socket(ADDRESS_FAMILY family, const char* ip, int port) : SocketParents(family, inet_addr(ip), port), mode(HS_NORMAL_SOCKET) {

}

Socket(SOCKET sock) : SocketParents(sock), mode(HS_NORMAL_SOCKET) {

}

int connect() {
SOCKADDR_IN ain = Socket::addr_in;
return ::connect(Socket::sock,
(SOCKADDR*)&ain,
sizeof(Socket::addr_in));
}

int send(const char* buf) {
SocketParents sp(SOCKET sock);
int res = ::send((SocketParents::sock), buf, strlen(buf), 0);
return res;
}


int recv(char* buf, int bufSize) {
if (mode != HS_NORMAL_SOCKET)
return HS_DO_NOTHING;
int n = ::recv((SocketParents::sock), buf, bufSize, 0);
return n;
}
int sendFile() {

}
int recvFile() {

}

void changeSocketMode(SocketMode mode) {
this->mode = mode;
}
};

*/
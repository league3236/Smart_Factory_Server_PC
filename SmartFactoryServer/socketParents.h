#pragma once
#include <WinSock2.h>
#include <iostream>

#ifdef Socket
#undef Socket
#endif

#define HS_ERROR_LISTEN_FAIL -3
#define HS_ERROR_BIND_FAIL -4;
#define HS_ERROR_ACCEPT_FAIL -5;

class SocketParents {
protected:
	SOCKET sock;
	SOCKADDR_IN addr_in;
	bool lock;
public:
	int label;
	
	void setDoorLock(bool _lock) {
		lock = _lock;
	}
	bool isLock() {
		return lock;
	}

	explicit SocketParents(SOCKET _sock);
	SocketParents(ADDRESS_FAMILY family, unsigned long __stdcall addr, int port);
	virtual ~SocketParents();
	void SetNonBlock();
	int close();
	void operator =(SOCKET& _sock);
	bool operator ==(SocketParents& socketParents);
	bool operator ==(SOCKET sock);
	bool operator !=(SocketParents& socketParents);
	bool operator !=(SOCKET sock);
	operator SOCKET ();
};

/*
#pragma once
#include <WinSock2.h>
#include <iostream>

#ifdef Socket
#undef Socket
#endif

#define HS_ERROR_LISTEN_FAIL -3
#define HS_ERROR_BIND_FAIL -4;
#define HS_ERROR_ACCEPT_FAIL -5;

class SocketParents {
protected:
	SOCKET sock;
	SOCKADDR_IN addr_in;
public:
	explicit SocketParents(SOCKET _sock) : sock(_sock) {}

	SocketParents(ADDRESS_FAMILY family, unsigned long __stdcall addr, int port) {
		sock = socket(PF_INET, SOCK_STREAM, 0);
		memset(&addr_in, 0, sizeof(addr_in));
		addr_in.sin_family = family;
		addr_in.sin_addr.S_un.S_addr = addr;
		addr_in.sin_port = htons(port);
	}

	virtual ~SocketParents() {
		closesocket(sock);
	}

	void SetNonBlock() {
		u_long t = 1;
		ioctlsocket(sock, FIONBIO, &t);
	}

	operator SOCKET () {
		return sock;
	}
	void operator =(SOCKET& _sock) {
		sock = _sock;

	}

	bool operator ==(SocketParents& socketParents) {
		return this->sock == socketParents.sock;
	}

	bool operator ==(SOCKET sock) {
		return this->sock == sock;
	}

	bool operator !=(SocketParents& socketParents) {
		return this->sock != socketParents.sock;
	}

	bool operator !=(SOCKET sock) {
		return this->sock != sock;
	}
	};*/
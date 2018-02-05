#include "serverSocket.h"


ServerSocket::ServerSocket(ADDRESS_FAMILY family, ULONG addr, int port)
	: SocketParents(family, htonl(addr), port) {
	addrSize = sizeof(SOCKADDR_IN);
	ain = SocketParents::addr_in;
	int res;
	res = bind(SocketParents::sock, (SOCKADDR*)&ain, sizeof(ain));
	if (res == SOCKET_ERROR) {
		printf("바인드 에러\n");
	}
	res = listen(SocketParents::sock, 10);
	if (res == SOCKET_ERROR) {
		printf("리슨 에러\n");
	}
}

Socket* ServerSocket::accept() {
	SOCKET sock_cli;
	sock_cli = ::accept(SocketParents::sock, (SOCKADDR*)&ain, &addrSize);
	if (sock_cli == INVALID_SOCKET || sock_cli == SOCKET_ERROR)
		return nullptr;
	else {
		return new Socket(sock_cli);
	}
}


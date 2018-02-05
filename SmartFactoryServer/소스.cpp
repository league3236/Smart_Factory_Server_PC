#include <iostream>
#include <WinSock2.h>
#include <String>


void main() {
	int port = 666;
	SOCKET sock_serv, *sock_cli;
	int    result;

	// 윈속 초기화 정보 구조체
	WSADATA    wsaData;

	// 윈속 초기화
	result = WSAStartup(MAKEWORD(2, 0), &wsaData);

	if (result != 0)
	{
		switch (result)
		{
		case WSASYSNOTREADY:
			printf("네트워크가 준비되지 않았습니다.\n");
			break;
		case WSAVERNOTSUPPORTED:
			printf("호환되지 않는 버전입니다.\n");
			break;
		case WSAEINPROGRESS:
			printf("이미 윈도우 소켓이 작업 진행 중 입니다.\n");
			break;
		case WSAEPROCLIM:
			printf("너무 많은 소켓 프로세스가 실행중 입니다.\n");
			break;
		case WSAEFAULT:
			printf("잘못된 소켓 데이터 구조입니다.\n");
			break;
		}
	}


	sock_serv = socket(PF_INET, SOCK_STREAM, 0);

	if (sock_serv == INVALID_SOCKET) {
		printf("소켓생성 실패\n");
		return;
	}
	printf("소켓 생성 성공\n");

	SOCKADDR_IN servAddr;


	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);

	result = bind(sock_serv, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (result == SOCKET_ERROR) {
		printf("바인드 실패\n");
		return;
	}
	printf("바인드 성공\n");

	result = listen(sock_serv, 5);
	if (result == SOCKET_ERROR) {
		printf("리슨 실패\n");
		return;
	}
	printf("리슨 성공\n");
	SOCKADDR_IN cliAddr;
	int n = sizeof(cliAddr);
	printf("클라이언트 대기중\n");

	u_long t=1;
	ioctlsocket(sock_serv, FIONBIO, &t);
	while(true){
		sock_cli = new SOCKET(accept(sock_serv, (SOCKADDR*)&cliAddr, &n));
		if (*sock_cli == INVALID_SOCKET) {
			printf("액셉트 실패\n");
			delete sock_cli;
		//	return;
		}
		else {
			printf("클라이언트가 접속하였습니다.\n");
			char* str = "welcome!!!!!";
			char buf[100];
			send(*sock_cli, str, sizeof(str), 0);
			int na = recv(*sock_cli, buf, strlen(str), 0);
			printf("\n%d\n", na);
		//	buf[na] = NULL;
			printf("cli : %s\n", buf);
			break;
		}
		
	}
	


	closesocket(*sock_cli);
	closesocket(sock_serv);



	// 윈속 제거
	WSACleanup();

}
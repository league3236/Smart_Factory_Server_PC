#include <iostream>
#include <WinSock2.h>
#include <String>


void main() {
	int port = 666;
	SOCKET sock_serv, *sock_cli;
	int    result;

	// ���� �ʱ�ȭ ���� ����ü
	WSADATA    wsaData;

	// ���� �ʱ�ȭ
	result = WSAStartup(MAKEWORD(2, 0), &wsaData);

	if (result != 0)
	{
		switch (result)
		{
		case WSASYSNOTREADY:
			printf("��Ʈ��ũ�� �غ���� �ʾҽ��ϴ�.\n");
			break;
		case WSAVERNOTSUPPORTED:
			printf("ȣȯ���� �ʴ� �����Դϴ�.\n");
			break;
		case WSAEINPROGRESS:
			printf("�̹� ������ ������ �۾� ���� �� �Դϴ�.\n");
			break;
		case WSAEPROCLIM:
			printf("�ʹ� ���� ���� ���μ����� ������ �Դϴ�.\n");
			break;
		case WSAEFAULT:
			printf("�߸��� ���� ������ �����Դϴ�.\n");
			break;
		}
	}


	sock_serv = socket(PF_INET, SOCK_STREAM, 0);

	if (sock_serv == INVALID_SOCKET) {
		printf("���ϻ��� ����\n");
		return;
	}
	printf("���� ���� ����\n");

	SOCKADDR_IN servAddr;


	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	servAddr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	servAddr.sin_port = htons(port);

	result = bind(sock_serv, (SOCKADDR*)&servAddr, sizeof(servAddr));
	if (result == SOCKET_ERROR) {
		printf("���ε� ����\n");
		return;
	}
	printf("���ε� ����\n");

	result = listen(sock_serv, 5);
	if (result == SOCKET_ERROR) {
		printf("���� ����\n");
		return;
	}
	printf("���� ����\n");
	SOCKADDR_IN cliAddr;
	int n = sizeof(cliAddr);
	printf("Ŭ���̾�Ʈ �����\n");

	u_long t=1;
	ioctlsocket(sock_serv, FIONBIO, &t);
	while(true){
		sock_cli = new SOCKET(accept(sock_serv, (SOCKADDR*)&cliAddr, &n));
		if (*sock_cli == INVALID_SOCKET) {
			printf("�׼�Ʈ ����\n");
			delete sock_cli;
		//	return;
		}
		else {
			printf("Ŭ���̾�Ʈ�� �����Ͽ����ϴ�.\n");
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



	// ���� ����
	WSACleanup();

}
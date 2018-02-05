#pragma once
#include <iostream>
#include <String>
#include <string.h>
#include <deque>
#include <vector>
#include "st.h"
#include "PTRList.h"
#include "fileSocket.h"
#include "hsprotocol.h"
#include "Face_recognition.h"
#include <io.h>

using namespace std;


//로그 출력안하려면 false로 두고 컴파일 할것
#define PRINT_LOG true

typedef PTRList<Socket> ClientList;
typedef PTRList<FileFromClientSocket> UploaderList;
typedef PTRList<FileToClientSocket> DownloaderList;

class LogPrinter {
	bool _isprint;
public:
	explicit LogPrinter(bool isprint) : _isprint(isprint) {}

	void printHsp(HSProtocol* hsp) {
		char* str = *hsp;
		if (_isprint) {
			for (int i = 0; i < 100; i++) {
				if (str[i] == NULL) {
					printf("_");
				}
				else {
					printf("%c", str[i]);
				}

			}
		}
	}

	void print(char* str) {
		if (_isprint)
			std::cout << str << std::endl;

	}
};


class HSServer
{

	WSADATA wsaData;
	ServerSocket *serverSocket;
	ClientList notLoginList;
	ClientList clientList;

	HSProtocol clientHSP;
	HSProtocol sendHSP;

	int port;
	//서버를 시작며 윈도우즈 DLL 설정과 서버소켓 개설 
	void serverStart(int port)
	{
		//mFace_recog = new Face_recognition();
		//mFace_recog.Traing_ready();
		Face_recognition::getInstance()->Traing_ready();
		WSAStartup(MAKEWORD(2, 0), &wsaData);
		serverSocket = new ServerSocket(AF_INET, INADDR_ANY, port);
		serverSocket->SetNonBlock();//논블럭 서버 소켓(listen socket) 개설
		sendHSP.setServerHSP();
	}

public:
	explicit HSServer(int _port) : port(_port) {

	}

	~HSServer() {
		//연결된 클라이언트에게 종료 사인 보내고 소켓 다 제거
		//{
		//
		//
		//}
		WSACleanup();
	}

	void start() {
		serverStart(port);
		working();
	}

	bool isHSP(HSProtocol *hsp) {
		if (hsp->getHSP() != 111)
			return false;
		else
			return true;
	}

	//클라이언트 거부
	void clientDenied(ClientList* p, index_t index) {
		Socket* sock = p->get(index);
		sock->close();
		p->remove(index);
	}

private:



	void reflesh(Socket* sock, ClientList* list, index_t index) {

		sock->close();
		list->remove(index);
		Face_recognition::getInstance()->Traing_ready();


	}

	//새 클라이언트가 있으면 접속 받는 소켓
	void accept() {
		Socket* newClient = serverSocket->accept();
		if (newClient != nullptr) {
			notLoginList.push(newClient);
			printf("새 클라이언트 접속 (%d번)\n", notLoginList.getLength());


		}

	}

	#define PORT	666
	#define MAXBUF	1024

	void notLoginListWork() {

		for (index_t i = 0; i < notLoginList.getLength(); i++) {
			Socket* sock = notLoginList[i];
			
			//int res = sock->recv(&clientHSP);
			//if (res > 0) {

			//	char buff[5];
			//	if (!isHSP(&clientHSP)) {
			//		recv(*sock, buff, 5, 0);

			//		if (!strcmp(buff, "true")) {
			//			FILE* file;
			//			int fileReadLen;
			//			char buf[MAXBUF];
			//			char rootDirectory[MAXBUF] = "C:\\PC_att_faces";
			//			char fileName1[MAXBUF];
			//			char fileName2[MAXBUF];
			//			sockaddr_in servAddr = { 0, };

			//			int folderCheck = 0;
			//			int fileCheck = 1;
			//			char buff[256];

			//			char finish[] = "-1";
			//			char flag[5];

			//			while (true)
			//			{
			//				strcpy_s(fileName1, MAX_PATH, rootDirectory);
			//				sprintf_s(buff, sizeof(buff), "%d", folderCheck);
			//				strcat_s(fileName1, MAX_PATH, "\\");
			//				strcat_s(fileName1, MAX_PATH, buff);

			//				recv(*sock, flag, 5, 0);
			//				if (_access(fileName1, 0) == -1)
			//				{
			//					cout << "전체 파일 전송 완료" << endl;
			//					cout << "fileName1: " << fileName1 << endl;
			//					send(*sock, finish, 3, 0);
			//					break;
			//				}
			//				else
			//				{
			//					cout << folderCheck << "번 폴더 전송 시작" << endl;
			//					send(*sock , buff, 256, 0);
			//				}
			//				strcat_s(fileName1, MAX_PATH, "\\");
			//				fileCheck = 1;
			//				recv(*sock, flag, 5, 0);

			//				while (true)
			//				{

			//					strcpy_s(fileName2, MAX_PATH, fileName1);
			//					sprintf_s(buff, sizeof(buff), "%d", fileCheck);
			//					strcat_s(fileName2, MAX_PATH, "\\");
			//					strcat_s(fileName2, MAX_PATH, buff);
			//					strcat_s(fileName2, MAX_PATH, ".png");

			//					/* 파일 열기  */
			//					fopen_s(&file, fileName2, "rb");
			//					cout << "before while" << endl;
			//					if (!file)
			//					{
			//						cout << "파일 전송 완료" << endl;
			//						send(*sock, finish, 3, 0);
			//						break;
			//					}
			//					else
			//					{
			//						cout << fileCheck << "번 파일 전송 시작" << endl;
			//						send(*sock, buff, 256, 0);
			//					}

			//					int i = 0;
			//					while (true) {
			//						memset(buf, 0x00, MAXBUF);
			//						fileReadLen = fread(buf, sizeof(char), MAXBUF, file);
			//						if (fileReadLen == EOF || fileReadLen == 0)
			//						{
			//							send(*sock, finish, 3, 0);
			//							cout << fileCheck << "번 파일 전송 완료" << endl;
			//							break;
			//						}
			//						else
			//						{
			//							send(*sock, buf, MAXBUF, 0);
			//						}
			//						//			cout << "파일 전송중: " << i << endl;
			//						//			i++;
			//					}
			//					recv(*sock, flag, 5, 0);
			//					fclose(file);
			//					fileCheck++;
			//				}
			//				folderCheck++;
			//			}
			//			Sleep(3000);
			//			closesocket(*sock);
			//		}

			//		clientDenied(&notLoginList, i);
			//		printf("ACCESS DENIED : CLIENT NOT HSP USE");
			//		continue;
			//	}
			//	notLoginListWorkDistinguish(sock, &clientHSP, &notLoginList, i);
			//}

			
			HSProtocol hsp;
			int res = sock->recv(&hsp);
			if (res > 0) {

				if (!isHSP(&hsp)) {
					clientDenied(&notLoginList, i);
					printf("ACCESS DENIED : CLIENT NOT HSP USE");
					continue;
				}
				notLoginListWorkDistinguish(sock, &hsp, &notLoginList, i);
			}
			
		}
	}
	//리스트 내 각각의 소켓마다 일을 함
	void notLoginListWorkDistinguish(
		Socket* sock,
		HSProtocol* hsp,
		ClientList* list,
		index_t index);



	void clientListWork() {
		printf("clientlistwork\n");
		for (index_t i = 0; i < clientList.getLength(); i++) {
			Socket* sock = clientList[i];
			/*int res = sock->recv(&clientHSP);

			if (res > 0) {
				if (!isHSP(&clientHSP)) {
					clientDenied(&clientList, i);
					printf("ACCESS DENIED : CLIENT NOT HSP USE");
					continue;
				}
				clientListWorkDistinguish(sock, &clientHSP, &clientList, i);
			}*/
			
			
			
			HSProtocol hsp;
			int res = sock->recv(&hsp);

			if (res > 0) {
				if (!isHSP(&hsp)) {
					clientDenied(&clientList, i);
					printf("ACCESS DENIED : CLIENT NOT HSP USE");
					continue;
				}
				clientListWorkDistinguish(sock, &hsp, &clientList, i);
			}
			
		}
	}

	void clientListWorkDistinguish(
		Socket* sock,
		HSProtocol* hsp,
		ClientList* list,
		index_t index);





	void working() {
		while (true) {
			accept();

			//파일 전송
			notLoginListWork();
			clientListWork();
		}
	}


};
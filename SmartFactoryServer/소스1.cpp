#include <iostream>
#include <String>
#include <deque>
#include <vector>
#include "st.h"
#include "PTRList.h"
#include "fileSocket.h"
#include "hsprotocol.h"

typedef PTRList<Socket> ClientList;
typedef PTRList<FileFromClientSocket> UploaderList;
typedef PTRList<FileToClientSocket> DownLoaderList;

void main() {
	WSADATA    wsaData;
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	
	ClientList cl;
	UploaderList ul;
	DownLoaderList dl;

	ServerSocket *serverSocket = new ServerSocket(AF_INET, INADDR_ANY, 666);
	serverSocket->SetNonBlock(); 
	Socket* list[10];
	int index = 0;
	Socket *a = new Socket(AF_INET,"127.0.0.1",666);
	Stinrg n = a.recv();
	a.send("아녕하세요");
	
	
	
	while (true) {
		//Sleep(500);
		Socket* newClient = serverSocket->accept();
		if (newClient != nullptr) {
			printf("새 클라이언트 접속 num : %d\n",cl.getLength());
			cl.push(newClient);
		}

		
		for (index_t i = 0; i < cl.getLength(); i++) {
			char buf[128];
			int nbyte;
			HSProtocol hsp;
			nbyte = cl[i]->recv(&hsp);
		
			//int res = sock->recv(hsp, 100);
			if (nbyte != -1) {
				printf("%s",hsp);
				//buf[nbyte] = '\0';

				if (buf[0] == '1') {
					printf("[%d] 1\n", i);
				}
				if (buf[0] == '2') {
					printf("[%d] 2\n", i);
				}
				if (buf[0] == '3') {
					printf("[%d] 3\n", i);
				}


				//업로드 신호에는 업로드할 파일 이름도 담기
				if (buf[0] == '8') {	
					char *name = &buf[1];
					ul.push(new FileFromClientSocket(cl[i], "C:/aa/", name));
					cl.remove(i);
				}

				if (buf[0] == '9') {
					cl.remove(i);
					printf("%d 접속끊음", i);
					
					continue;
				}

			
			}
		}

	

		//파일 업로드중인 클라이언트들
		for (index_t i = 0; i < ul.getLength(); i++) {
			int res = ul[i]->recvFile();
			if (res == HS_FS_RECEIVE_DONE || res ==HS_FS_RECEIVE_FAIL) {
				if (res == HS_FS_RECEIVE_DONE)
					printf("전송 완료");
				else 
					printf("전송 실패");
				cl.push(new Socket(ul[i]));
				ul.remove(i);
			}
			else if (res == HS_FS_RECEIVING) {
				printf("downloading\n");
			}
			else if (res == HS_FS_DO_NOTHING) {
			//	printf("nothing");
			}
		}	

		for (index_t i = 0; i < dl.getLength(); i++) {
			int res = dl[i]->sendFile();
			if (res == HS_FS_RECEIVE_DONE) {

			}
			else if (res == HS_FS_SEND_FAIL) {

			}
			else if (res == HS_FS_SENDING) {
				
			}
		
		}
	}
	
	WSACleanup();
}

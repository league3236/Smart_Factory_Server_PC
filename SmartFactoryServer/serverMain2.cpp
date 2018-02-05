#include "serverMain2.h"



void ServerMain2::start() {
	Face_recognition::getInstance()->Traing_ready();
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	serverSocket = new ServerSocket(AF_INET, INADDR_ANY, serverPort);
	serverSocket->SetNonBlock();//논블럭 서버 소켓(listen socket) 개설
	printf("Server Start!\n");
	std::thread t(ServerMain2::commandThread,this);
	t.detach();
	while (true) {
		accept();
		works();
	}
}


void ServerMain2::accept() {

	Socket* newClient = serverSocket->accept();
	if (newClient != nullptr) {
		newClient->label = this->assignID();
		Nlist.push(newClient);
		printf("new Client access [TraceID : %d]\n", newClient->label);
	}
}

void ServerMain2::works() {
	//로그인 안한 영역
	for (index_t i = 0; i < Nlist.getLength(); i++) {
		Socket* sock = Nlist[i];
		if (sock != nullptr) {
			int res = sock->recv(&clientHSP);
			//뭔가 시그널을 받았음.
			if (res > 0) {
				//HSP사용중이 아니면 걸러야 하지만 넘어가기로..
				//데스크탑 사용자가 로그인 하는 방법
				if (clientHSP.getMESSAGE() == HSP_MESSAGE_LOGIN_START) {
					sock = Nlist.poach(i);
					printf("Client [%d] is Login. [TraceID %d]\n", clientHSP.getID(), sock->label);
					Llist.push(sock);
					sendHSP.setMESSAGE(HSP_MESSAGE_OK);
					sock->send(&sendHSP);
					i--;
				}
				else if (clientHSP.getMESSAGE() == HSP_MESSAGE_FACEDETECTING_START) {
					sock = Nlist.poach(i);
					i--;
					//faceDetecting(sock);
					//스레드를 편법으로 생성하긴 했는데 될진 모르겠따..
					Arg_faceDetecting arg = { this,sock };
					std::thread t(faceDetectingThread, (LPVOID)&arg);

					t.detach();
					//t.detach();
				}
			}
		}

		

	}

	//로그인 한 영역
	for (int i = 0; i < Llist.getLength(); i++) {
		Socket* sock = Llist[i];
		if (socket != nullptr) {
			int res = sock->recv(&clientHSP);
			if (res > 0) {
				if (clientHSP.getMESSAGE() == HSP_MESSAGE_REQUEST_FILE) {
					char* fileName = new char[30];
					clientHSP.getFileName(fileName);
					sock = Llist.poach(i);
					printf("%d | Request %s [TraceID : %d]\n", clientHSP.getID(), fileName, sock->label);
					i--;		
					Arg_sendFile arg = { this,sock,fileName };
					std::thread t(sendFileThread, (LPVOID)&arg);

					t.detach();
				}
				else if (clientHSP.getMESSAGE() == HSP_MESSAGE_SIGN_UPLOAD) {
					printf("%d | SignUpload [TraceID : %d]\n", clientHSP.getID(), sock->label);

					sock = Llist.poach(i);
					i--;
					Arg_getSign arg = { this,sock };
					std::thread t(getSignThread, (LPVOID)&arg);
					
					t.detach();
				}

			}



		}


	}




}

void ServerMain2::sendFile(Socket* sock, char* fileName) {
	int nbyte = 0;
	int size = 0;
	int sendSize = 0;
	int readSize = 0;

	HSProtocol hsp;
	HSProtocol sendHSP;
	sendHSP.setServerHSP();
	ULONG fileSize = 0;
	string  path;

	sendHSP.setServerHSP();

	path.append("C:/smServer/works/");
	path.append(fileName);
	path.append(".PDF");
	printf("filePath : %s\n", path.c_str());
	FILE* fp = fopen(path.c_str(), "rb");

	if (fp == NULL) {
		printf("Wrong File Name.\n",path.c_str());
		sendHSP.setMESSAGE(HSP_MESSAGE_DENIED);
		sock->send(&sendHSP);
		Llist.push(sock);
		return;
	}



	struct _stati64 stat;
	_stati64(path.c_str(), &stat);
	fileSize = stat.st_size;

	sendHSP.setMESSAGE(HSP_MESSAGE_IMAGE_UPLOAD);
	sendHSP.setSize((int)fileSize);
	//printf("(%d BYTE)", (int)fileSize);

	sock->send(&sendHSP);


	//클라이언트가 파일을 받을 준비를 끝내고 OK메세지를 보내기를 대기하는 부분
	while (true) {
		nbyte = sock->recv(&hsp);
		//	printf("wating....\n");
		Sleep(500);
		if (nbyte > 0) {
			if (hsp.getMESSAGE() == HSP_MESSAGE_OK) {
				break;
			}
		}
	}



	//파일을 실제로 전송하는 부분
	while (true) {

		char buf[1024] = { '\0' };
		nbyte = fread(buf, 1, 1024, fp);
		//printf("읽은 량 : %d  ", nbyte);
		char* nbuf = new char[nbyte];
		memcpy(nbuf, buf, nbyte);

		int sendByte = -1;

		while (sendByte <= 0) {
			sendByte = sock->send(nbuf, nbyte);
			if (sendByte > 0) {
				sendSize += sendByte;
				Sleep(1);
				if (sendSize >= fileSize) {
					printf("%d | sending complete.\n",hsp.getID());
					Llist.push(sock);
					delete[] fileName;
					delete[] nbuf;
					fclose(fp);
					return;
				}
				delete[] nbuf;
			}

		}
	}

	printf("%d | sending complete.\n", hsp.getID());
	Llist.push(sock);
	delete[] fileName;
	return;
}

void ServerMain2::command() {
	while (true) {
		char buf[10];
		while (true) {
			std::cin >> buf;
			if (!strcmp(buf, "exit"))
				exit(0);
			if (!strcmp(buf, "show")) {
				printf("===================================\n");
				printf("Not Logined Clients %d\n", Nlist.getLength());
				for (int i = 0; i < Nlist.getLength(); i++) {
					Socket *sock;
					if (Nlist[i] != nullptr) {
						sock = Nlist[i];
						printf("TraceID : %d\n", sock->label);
					}
				}
				printf(" - - - - - - - - - - - - - - - - - \n");
				printf("Logined Clients %d\n",Llist.getLength());
				for (int i = 0; i < Llist.getLength(); i++) {
					Socket *sock = Nlist.get(i);
					if (sock != NULL) {
						sock = Llist[i];
						printf("TraceID : %d\n", sock->label);
					}
				}
				printf("===================================\n");
			}
			if (!strcmp(buf, "help")) {
				printf("exit\nshow client\n");
			}


		}



	}

}

//클라이언트로부터 사인한 이미지를 업로드 받는 함수
void ServerMain2::getSign(Socket* sock) {
	int size = 0;

	int receive_byte = 0;
	int nbyte = 0;
	HSProtocol hsp;
	HSProtocol sendHSP;
	string filePath;

	filePath.append("C:/smServer/signature/");


	sendHSP.setServerHSP();
	sendHSP.setMESSAGE(HSP_MESSAGE_OK);

	sock->send(&sendHSP);//
	
	while (true) {//클라이언트가 이미지업로드준비끝내고 메세지 보낼 때 까지 대기
		nbyte = sock->recv(&hsp);
		if (nbyte > 0) {
			if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
				//size세팅;
				char sizeBuf[20];
				char fileName[10];
				hsp.getSize(sizeBuf);
				hsp.getFileName(fileName);
				size = atoi(sizeBuf);
				filePath.append(fileName);
				filePath.append(".jpg");
				printf("File path : %s\n", filePath.c_str());
				sock->send(&sendHSP);
				break;
			}
		}
	}


	FILE* fp = fopen(filePath.c_str(), "wb");// PATH 지정하는 방식이 있어야해
	while (true) {//파일 받는 부분
		char buf[1024] = { 0 };
		nbyte = sock->recv(buf, 1024);

		if (nbyte > 0) {
			fwrite(buf, nbyte, 1, fp);
			receive_byte += nbyte;
			if (receive_byte >= size) {
				fclose(fp);
				break;
			}
		}
	}
	//sock->send(&sendHSP);//다 받고 OK보내줌
	Llist.push(sock);
	printf("%d | sign upload done.\n",hsp.getID());

}


void ServerMain2::faceDetecting(Socket* sock) {
	int nbyte = 0;
	int receive_byte = 0;
	int size = 10304;//정해진 값 만약 정해지지않으면 imgUPLOAD를 받고 해야해
	char imgBuf[10305];
	HSProtocol hsp;
	HSProtocol sendHSP;
	
	sendHSP.setServerHSP();
	sendHSP.setMESSAGE(HSP_MESSAGE_OK);
	sock->send(&sendHSP);

	printf("Face Detecting start\n");
	while (true) {
		//이미지 업로드를 시작한다는 메세지 받을 때 까지 대기
		//중간에 얼굴인식을 꺼버리면 스레드 종료
		int err_cnt = 0;
		while (true) {
			nbyte = sock->recv(&hsp);
			if (nbyte > 0) {
				if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
					sock->send(&sendHSP);
					break;
				}

				//얼굴인식 안하고 그냥 종료
				else if (hsp.getMESSAGE() == HSP_MESSAGE_FACEDETECTING_END) {
					Nlist.push(sock);
					printf("Detecting Fail.\n");
					return;
				}
			}
			else {
				printf(".");
				err_cnt++;
			}
			if (err_cnt >= 50)
				return;
			Sleep(500);
		}

		//파일을 전송 받는 부분
		int imsy_count = 0;
		while (true) {
			if (imsy_count > 1000){
				Nlist.push(sock);
				printf("Detecting fail\n");
				return;
			}

			imsy_count++;
			//printf("cnt : %d  ", imsy_count++);
			receive_byte = 0;
			nbyte = 0;
			while (true) {
				char buf[1024] = { '\0' };
				nbyte = sock->recv(buf, 1024);
				if (nbyte > 0) {
					if ((receive_byte + nbyte) > 10304) {
						nbyte = 10304 - receive_byte;//배열 참조에러나지않게
					}
					memcpy(&imgBuf[receive_byte], buf, nbyte);
					receive_byte += nbyte;
				//	printf("%d bytes download,  (%d/10304 bytes) \n", nbyte, receive_byte);
				}

				if (receive_byte >= size) {
					break;
				}
			}
			int Label = Face_recognition::getInstance()->Face_recog_start((unsigned char*)imgBuf);
			
			//안면인식 후 결과를 전송



			//누군가로 인식이 되었을경우
			//로그인 성공시킴
			if (Label > -1) {
				sendHSP.setMESSAGE(HSP_MESSAGE_LOGIN_START);
				sendHSP.setID(Label);
				sock->send(&sendHSP);
				printf("Client [%d] is Login. [TraceID %d]\n", Label, sock->label);
				Llist.push(sock);
				return;

			}
			//unknown으로 인식되었을경우 
			else {
				sendHSP.setMESSAGE(HSP_MESSAGE_FACEDETECTING_UNKNOWNPERSON);
				sock->send(&sendHSP);
				continue; //다시
			}
		}


	}


}


//접속한 소켓에 고유 ID를 할당하는 메서드
//일단은 0부터시작해서 그냥 증가하면서 할당해주지만
//나중엔 보완해서 없는거채우고 근데 귀찮아서 할지 안할지 모르게씀..
int ServerMain2::assignID() {

	return TraceID++;
}



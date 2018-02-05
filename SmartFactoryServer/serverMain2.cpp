#include "serverMain2.h"



void ServerMain2::start() {
	Face_recognition::getInstance()->Traing_ready();
	WSAStartup(MAKEWORD(2, 0), &wsaData);
	serverSocket = new ServerSocket(AF_INET, INADDR_ANY, serverPort);
	serverSocket->SetNonBlock();//��� ���� ����(listen socket) ����
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
	//�α��� ���� ����
	for (index_t i = 0; i < Nlist.getLength(); i++) {
		Socket* sock = Nlist[i];
		if (sock != nullptr) {
			int res = sock->recv(&clientHSP);
			//���� �ñ׳��� �޾���.
			if (res > 0) {
				//HSP������� �ƴϸ� �ɷ��� ������ �Ѿ���..
				//����ũž ����ڰ� �α��� �ϴ� ���
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
					//�����带 ������� �����ϱ� �ߴµ� ���� �𸣰ڵ�..
					Arg_faceDetecting arg = { this,sock };
					std::thread t(faceDetectingThread, (LPVOID)&arg);

					t.detach();
					//t.detach();
				}
			}
		}

		

	}

	//�α��� �� ����
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


	//Ŭ���̾�Ʈ�� ������ ���� �غ� ������ OK�޼����� �����⸦ ����ϴ� �κ�
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



	//������ ������ �����ϴ� �κ�
	while (true) {

		char buf[1024] = { '\0' };
		nbyte = fread(buf, 1, 1024, fp);
		//printf("���� �� : %d  ", nbyte);
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

//Ŭ���̾�Ʈ�κ��� ������ �̹����� ���ε� �޴� �Լ�
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
	
	while (true) {//Ŭ���̾�Ʈ�� �̹������ε��غ񳡳��� �޼��� ���� �� ���� ���
		nbyte = sock->recv(&hsp);
		if (nbyte > 0) {
			if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
				//size����;
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


	FILE* fp = fopen(filePath.c_str(), "wb");// PATH �����ϴ� ����� �־����
	while (true) {//���� �޴� �κ�
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
	//sock->send(&sendHSP);//�� �ް� OK������
	Llist.push(sock);
	printf("%d | sign upload done.\n",hsp.getID());

}


void ServerMain2::faceDetecting(Socket* sock) {
	int nbyte = 0;
	int receive_byte = 0;
	int size = 10304;//������ �� ���� �������������� imgUPLOAD�� �ް� �ؾ���
	char imgBuf[10305];
	HSProtocol hsp;
	HSProtocol sendHSP;
	
	sendHSP.setServerHSP();
	sendHSP.setMESSAGE(HSP_MESSAGE_OK);
	sock->send(&sendHSP);

	printf("Face Detecting start\n");
	while (true) {
		//�̹��� ���ε带 �����Ѵٴ� �޼��� ���� �� ���� ���
		//�߰��� ���ν��� �������� ������ ����
		int err_cnt = 0;
		while (true) {
			nbyte = sock->recv(&hsp);
			if (nbyte > 0) {
				if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
					sock->send(&sendHSP);
					break;
				}

				//���ν� ���ϰ� �׳� ����
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

		//������ ���� �޴� �κ�
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
						nbyte = 10304 - receive_byte;//�迭 �������������ʰ�
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
			
			//�ȸ��ν� �� ����� ����



			//�������� �ν��� �Ǿ������
			//�α��� ������Ŵ
			if (Label > -1) {
				sendHSP.setMESSAGE(HSP_MESSAGE_LOGIN_START);
				sendHSP.setID(Label);
				sock->send(&sendHSP);
				printf("Client [%d] is Login. [TraceID %d]\n", Label, sock->label);
				Llist.push(sock);
				return;

			}
			//unknown���� �νĵǾ������ 
			else {
				sendHSP.setMESSAGE(HSP_MESSAGE_FACEDETECTING_UNKNOWNPERSON);
				sock->send(&sendHSP);
				continue; //�ٽ�
			}
		}


	}


}


//������ ���Ͽ� ���� ID�� �Ҵ��ϴ� �޼���
//�ϴ��� 0���ͽ����ؼ� �׳� �����ϸ鼭 �Ҵ���������
//���߿� �����ؼ� ���°�ä��� �ٵ� �����Ƽ� ���� ������ �𸣰Ծ�..
int ServerMain2::assignID() {

	return TraceID++;
}



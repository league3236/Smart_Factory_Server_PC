#include "hsserver.h"
#include <thread>
#include "Face_recognition.h"
#include "TimeCounter.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <WinSock2.h>
#include <iostream>
#include <fstream>
#include <string>
#include <io.h>
#include <direct.h>

using namespace std;

typedef std::thread Thread;
int sendFileLog = 0;
int getSignLog = 0;
void logout(Socket* sock, ClientList* list, index_t index);
void faceDetect(Socket* sock, ClientList* notLoginList, ClientList* loginList);
void getSign(Socket* sock, ClientList* list);
void sendFILE(Socket* sock, char* fileName, ClientList* list);
//���� �۾��� ���� �ٽ� �ڵ� �κ��Դϴ�.
//Ŭ���̾�Ʈ���� ���� HSP ���۸� �а� 
//���ϴ� �۾��� ���� �� ��
//�Ű����� �� ���Ͽ� �����ָ� ��


void sendOK(Socket* sock) {
	HSProtocol hsp;
	hsp.setServerHSP();
	hsp.setMESSAGE(HSP_MESSAGE_OK);
	sock->send(&hsp);
}


//�α��� ���� ���� ������� �۾� ����
void HSServer::notLoginListWorkDistinguish(
	Socket* sock,
	HSProtocol* hsp,
	ClientList* list,
	index_t index) {

	//Ŭ���̾�Ʈ�� ����ũž
	if (hsp->getDEVICE() == HSP_DEVICE_DESKTOP) {
		//������
		if (hsp->getSERVICEID() == HSP_SERVICEID_ADMIN) {
	
			if (hsp->getMESSAGE() == HSP_MESSAGE_TRAING) {
				reflesh(sock,list,index);
				printf("���� ������ ���� Ʈ���̴� �߽��ϴ�.\n");
			}

		}//�Ϲ�����
		else if (hsp->getSERVICEID() == HSP_SERVICEID_USER) {
			printf("����ũž �Ϲ� ����(ID : %d �۾� : %d)\n", hsp->getID(), hsp->getMESSAGE());
			if (hsp->getMESSAGE() == HSP_MESSAGE_LOGIN_START) {
				list->poach(index);
				clientList.push(sock);
				sendOK(sock);
				printf("����� [%d]�� �α��� �߽��ϴ�.\n", hsp->getID());
				
			}
			else if(hsp->getMESSAGE()==HSP_MESSAGE_LOGOUT){
				logout(sock, list, index);
				
			}
		}
		else {
			//�߸���Ŭ���̾�Ʈ

		}

	}

	//Ŭ���̾�Ʈ�� �����
	else if (hsp->getDEVICE() == HSP_DEVICE_MOBILE) {
		int message = hsp->getMESSAGE();
		printf("����� ���� �޼��� : %d\n", message);

		if (hsp->getMESSAGE() == HSP_MESSAGE_FACEDETECTING_START) {
			printf("�� �ν� ���۸޼��� ����\n");
			list->poach(index);
			//Thread t(faceDetect, sock, &notLoginList, &clientList);
			//t.detach();
			faceDetect(sock, &notLoginList, &clientList);
			//cout << "detach" << endl;
		}
		else if (hsp->getMESSAGE() == HSP_MESSAGE_LOGOUT) {
			logout(sock, list, index);

		}
		//cout << "abcd" << endl;
	}




	else {
		
	}


}


//�α��� �� ������� �۾� ����
void HSServer::clientListWorkDistinguish(
	Socket* sock,
	HSProtocol* hsp,
	ClientList* list,
	index_t index) {
	printf("clientlistworkdis\n");

	int message = hsp->getMESSAGE();


	//���ξ��ε带 �䱸�ϴ� ���
	if (message == HSP_MESSAGE_SIGN_UPLOAD) {
		list->poach(index);
		Thread t(getSign, sock, list);
		t.detach();
	}//������ �����޶�� ��û�ϴ� ���
	else if (message == HSP_MESSAGE_REQUEST_FILE) {
		printf("REQUEST FILE\n");
		char* fileName = new char[30];
		hsp->getFileName(fileName);
		printf("File request : %s\n ", fileName);
		list->poach(index);
		Thread t(sendFILE, sock, fileName, list);
		t.detach();
	}
	else if (message == HSP_MESSAGE_LOGOUT) {
		logout(sock, list, index);
	}


}

//Ŭ���̾�Ʈ�κ��� ������ �̹����� ���ε� �޴� �Լ�
void getSign(Socket* sock, ClientList* list) {
	int size = 0;
	int receive_byte = 0;
	int nbyte = 0;
	HSProtocol hsp;
	HSProtocol sendHSP;
	string filePath;
	Timer timer(30);
	static int temp = 0;
	printf("%d��° getsign start\n",getSignLog);

	int id = 0;
	filePath.append("C:/smServer/signature/");

	sendHSP.setServerHSP();
	sendHSP.setMESSAGE(HSP_MESSAGE_OK);

	sock->send(&sendHSP);//

	timer.start();

	printf("Ŭ���̾�Ʈ�� �̹������ε��غ񳡳��� �޼��� ���� �� ���� ���\n");
	while (true) {//Ŭ���̾�Ʈ�� �̹������ε��غ񳡳��� �޼��� ���� �� ���� ���
		nbyte = sock->recv(&hsp);
		if (nbyte > 0) {
			if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
				
				//size����;
				char sizeBuf[20];
				char fileName[10];
				hsp.getSize(sizeBuf);
				printf("SIZE STR : %s\n", sizeBuf);
				hsp.getFileName(fileName);
				size = atoi(sizeBuf);
				printf("FILENAME : %s(%d BYTE)\n", fileName, size);
				filePath.append(fileName);
				filePath.append(".jpg");
				printf("Path : %s\n", filePath.c_str());
				
				sock->send(&sendHSP);//OK MSG SEND
				
				printf("Ŭ���̾�Ʈ ���ε� \n");
				break;
			}
		}
		if (timer.timeOver()) {
			sock->close();
			printf("%d Ŭ���̾�Ʈ �������. (getSign 1) %d\n", hsp.getID(), getSignLog);
			return;
		}
	}


	timer.start();
	FILE* fp = fopen(filePath.c_str(), "wb");// PATH �����ϴ� ����� �־����
	while (true) {//���� �޴� �κ�
		char buf[1024] = { 0 };
		nbyte = sock->recv(buf, 1024);

		if (nbyte > 0) {			
			printf("receive : %d (%d / %d)\n", nbyte, receive_byte,size);
			fwrite(buf, nbyte, 1, fp);
			//	printf("C\n");
			receive_byte += nbyte;
			if (receive_byte >= size) {
				fclose(fp);
				printf("done\n");
				break;
			}
		}

		if (timer.timeOver()) {
			sock->close();
			printf("%d Ŭ���̾�Ʈ �������.(getSign 2) %d\n", hsp.getID(), getSignLog);
			return;
		}

	}
	sock->send(&sendHSP);//�� �ް� OK������
	list->push(sock);
	getSignLog++;
	printf("sign upload done.\n");

}



void sendFILE(Socket* sock, char* fileName, ClientList* list) {
	int nbyte = 0;
	int size = 0;
	int sendSize = 0;
	int readSize = 0;
	static int temp = 0;
	Timer timer(30);
	HSProtocol hsp;
	HSProtocol sendHSP;
	sendHSP.setServerHSP();
	ULONG fileSize = 0;
	string  path;

	path.append("C:/smServer/works/");
	path.append(fileName);
	path.append(".PDF");
	printf("filePath : %s", path.c_str());
	FILE* fp = fopen(path.c_str(), "rb");

	if (fp == NULL) {
		printf("Ŭ���̾�Ʈ�κ��� �߸��� ������ ��û�޾ҽ��ϴ�.\n");
		sendHSP.setMESSAGE(HSP_MESSAGE_DENIED);
		sock->send(&hsp);
		list->push(sock);
		return;
	}


	printf("%d��° sendFile start\n", sendFileLog);

	struct _stati64 stat;
	_stati64(path.c_str(), &stat);
	fileSize = stat.st_size;

	sendHSP.setMESSAGE(HSP_MESSAGE_IMAGE_UPLOAD);
	sendHSP.setSize((int)fileSize);
	printf("(%d BYTE)", (int)fileSize);

	sock->send(&sendHSP);


	timer.start();

	//Ŭ���̾�Ʈ�� ������ ���� �غ� ������ OK�޼����� �����⸦ ����ϴ� �κ�
	while (true) {
		nbyte = sock->recv(&hsp);
		//	printf("wating....\n");
		if (nbyte > 0) {
			if (hsp.getMESSAGE() == HSP_MESSAGE_OK) {
				break;
			}
		}

		if (timer.timeOver()) {
			sock->close();
			printf("%d Ŭ���̾�Ʈ �������.(sendFILE 1) %d\n", hsp.getID(),sendFileLog);
			return;
		}
	}

	printf("���۽���\n");

	timer.start();
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
		
			/*for (int i = 0; i < nbyte; i++)
				printf("%d\n", nbuf[i]);*/
			
			if (sendByte > 0) {
				//printf("send : %d \n", sendByte);
				sendSize += sendByte;

				if (sendSize >= fileSize) {
					printf("AllSendSize : %d sendbyte : %d\n", sendSize, sendByte);
					printf("���ۿϷ�\n");

					list->push(sock);
					delete[] fileName;
					delete[] nbuf;
					fclose(fp);

					return;
				}
			}
		}
		delete[] nbuf;

		if (timer.timeOver()) {
			sock->close();
			printf("%d Ŭ���̾�Ʈ �������.(sendFILE 2) %d\n", hsp.getID(), sendFileLog);
			return;
		}
	}

	sendFileLog++;
	printf("���ۿϷ�\n");
	list->push(sock);
	delete[] fileName;
	return;
}


//�� ���� �Լ�
//���� ���� �ϰų� Ŭ���̾�Ʈ�� ������ ��(MESSAGE_FACEDETECHTING_END) ���� �������� ����
//���� �ϳ��� ũ��� 10305BYTE;
void faceDetect(Socket* sock, ClientList* notLoginList, ClientList* loginList) {
	int nbyte = 0;
	int receive_byte = 0;
	int size = 10304;//������ �� ���� �������������� imgUPLOAD�� �ް� �ؾ���
	char imgBuf[10305];

	HSProtocol hsp;
	HSProtocol sendHSP;
	

	sendHSP.setServerHSP();
	sendHSP.setMESSAGE(HSP_MESSAGE_OK);
	sock->send(&sendHSP);

	while (true) {
		//�̹��� ���ε带 �����Ѵٴ� �޼��� ���� �� ���� ���
		//�߰��� ���ν��� �������� ������ ����
		printf("���������\n");
		int err_cnt = 0;
		while (true) {
			nbyte = sock->recv(&hsp);
			if (nbyte > 0) {
				printf("�޼�������");
				if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
					printf("���ε����");
					sock->send(&sendHSP);
					break;
				}

				else if (hsp.getMESSAGE() == HSP_MESSAGE_FACEDETECTING_END) {
					notLoginList->push(sock);
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
			if (imsy_count > 10000)
				return;

			imsy_count++;
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
					//printf("%d byte ����, �� %d byte\n", nbyte, receive_byte); // �����̰� �ּ� ó�� Ǯ��
							
				}

				if (receive_byte >= size) {
					break;
				}
				
			}
			
			//�� �����ͼ� ���� ��ǻ�Ϳ� ����.
			Mat Detected_face(Size(92, 112), CV_8UC1);
			Detected_face.data = (unsigned char*)imgBuf;

			equalizeHist(Detected_face, Detected_face);
			imwrite("C:\\PC_att_facess\\B\\" + to_string(imsy_count) + ".png", Detected_face);
			
			int Label = Face_recognition::getInstance()->Face_recog_start((unsigned char*)imgBuf);
			//int Label = -1;
			//printf("Label is %d  cnt : %d\n", Label, imsy_count);
			//printf("=========================================\n");


			//�ȸ��ν� �� ����� ����

			//�������� �ν��� �Ǿ������
			// Label = 2;			//���� �� �ν��� ������ �ϰԲ� �Ҷ� Label�� ���� ���Ƿ� �Է½�Ű�� �ȴ�.
			if (Label > -1) {
				loginList->push(sock);
				sendHSP.setMESSAGE(HSP_MESSAGE_LOGIN_START);
				sendHSP.setID(Label);
				sock->send(&sendHSP);
				printf("����� [%d]�� �α��� �߽��ϴ�.\n", Label);
				
				return;
			}
			else {  //unknown���� �νĵǾ������ 			
				sendHSP.setMESSAGE(HSP_MESSAGE_FACEDETECTING_UNKNOWNPERSON);
				sock->send(&sendHSP);
				continue; //�ٽ�
			}
		}

		/*
		for (int i = 0; i < 10304; i++) {
			printf("%d ",imgBuf[i]);
			if (i % 10 == 0)
				printf("\n");
		}
		*/


		/****************************************
		�ȸ��ν��ϴ� �ڵ�
		int result = foo(imgBuf);

		�ȸ��ν� �� ����� ����


		if(result == ����){
			loginList->push(sock);
			sendHSP.setMESSAGE(HSP_MESSAGE_FACEDETECTING_IS_USER);
			sock->send(&sendHSP);
			break;
		}

		else if(result == ������){
			loginList->push(sock);
			sock->send(&sendHSP);
			break;
		}

		else if(result == UNKNOWN){
			notLoginList->push(sock);
			sendHSP.setMESSAGE(HSP_MESSAGE_FACEDETECTING_UNKNOWNPERSON);
			sock->send(&sendHSP);
			break;
		}


		else if(result == ����){
			�ǵ��ƶ�!
			continue;
		}
		********************************************/
	}
	printf("����������");
	return;
}


void logout(Socket* sock, ClientList* list, index_t index) {
	sock->close();
	list->remove(index);
	printf("�α׾ƿ� �Ͽ����ϴ�.");
}


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
//서버 작업의 가장 핵심 코드 부분입니다.
//클라이언트에게 받은 HSP 버퍼를 읽고 
//원하는 작업을 수행 한 뒤
//매개변수 속 소켓에 보내주면 됨


void sendOK(Socket* sock) {
	HSProtocol hsp;
	hsp.setServerHSP();
	hsp.setMESSAGE(HSP_MESSAGE_OK);
	sock->send(&hsp);
}


//로그인 하지 않은 사용자의 작업 구분
void HSServer::notLoginListWorkDistinguish(
	Socket* sock,
	HSProtocol* hsp,
	ClientList* list,
	index_t index) {

	//클라이언트가 데스크탑
	if (hsp->getDEVICE() == HSP_DEVICE_DESKTOP) {
		//관리자
		if (hsp->getSERVICEID() == HSP_SERVICEID_ADMIN) {
	
			if (hsp->getMESSAGE() == HSP_MESSAGE_TRAING) {
				reflesh(sock,list,index);
				printf("사진 파일을 새로 트레이닝 했습니다.\n");
			}

		}//일반유저
		else if (hsp->getSERVICEID() == HSP_SERVICEID_USER) {
			printf("데스크탑 일반 유저(ID : %d 작업 : %d)\n", hsp->getID(), hsp->getMESSAGE());
			if (hsp->getMESSAGE() == HSP_MESSAGE_LOGIN_START) {
				list->poach(index);
				clientList.push(sock);
				sendOK(sock);
				printf("사용자 [%d]가 로그인 했습니다.\n", hsp->getID());
				
			}
			else if(hsp->getMESSAGE()==HSP_MESSAGE_LOGOUT){
				logout(sock, list, index);
				
			}
		}
		else {
			//잘못된클라이언트

		}

	}

	//클라이언트가 모바일
	else if (hsp->getDEVICE() == HSP_DEVICE_MOBILE) {
		int message = hsp->getMESSAGE();
		printf("모바일 유저 메세지 : %d\n", message);

		if (hsp->getMESSAGE() == HSP_MESSAGE_FACEDETECTING_START) {
			printf("얼굴 인식 시작메세지 받음\n");
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


//로그인 한 사용자의 작업 구분
void HSServer::clientListWorkDistinguish(
	Socket* sock,
	HSProtocol* hsp,
	ClientList* list,
	index_t index) {
	printf("clientlistworkdis\n");

	int message = hsp->getMESSAGE();


	//사인업로드를 요구하는 경우
	if (message == HSP_MESSAGE_SIGN_UPLOAD) {
		list->poach(index);
		Thread t(getSign, sock, list);
		t.detach();
	}//파일을 보내달라고 요청하는 경우
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

//클라이언트로부터 사인한 이미지를 업로드 받는 함수
void getSign(Socket* sock, ClientList* list) {
	int size = 0;
	int receive_byte = 0;
	int nbyte = 0;
	HSProtocol hsp;
	HSProtocol sendHSP;
	string filePath;
	Timer timer(30);
	static int temp = 0;
	printf("%d번째 getsign start\n",getSignLog);

	int id = 0;
	filePath.append("C:/smServer/signature/");

	sendHSP.setServerHSP();
	sendHSP.setMESSAGE(HSP_MESSAGE_OK);

	sock->send(&sendHSP);//

	timer.start();

	printf("클라이언트가 이미지업로드준비끝내고 메세지 보낼 때 까지 대기\n");
	while (true) {//클라이언트가 이미지업로드준비끝내고 메세지 보낼 때 까지 대기
		nbyte = sock->recv(&hsp);
		if (nbyte > 0) {
			if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
				
				//size세팅;
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
				
				printf("클라이언트 업로드 \n");
				break;
			}
		}
		if (timer.timeOver()) {
			sock->close();
			printf("%d 클라이언트 응답없음. (getSign 1) %d\n", hsp.getID(), getSignLog);
			return;
		}
	}


	timer.start();
	FILE* fp = fopen(filePath.c_str(), "wb");// PATH 지정하는 방식이 있어야해
	while (true) {//파일 받는 부분
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
			printf("%d 클라이언트 응답없음.(getSign 2) %d\n", hsp.getID(), getSignLog);
			return;
		}

	}
	sock->send(&sendHSP);//다 받고 OK보내줌
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
		printf("클라이언트로부터 잘못된 파일을 요청받았습니다.\n");
		sendHSP.setMESSAGE(HSP_MESSAGE_DENIED);
		sock->send(&hsp);
		list->push(sock);
		return;
	}


	printf("%d번째 sendFile start\n", sendFileLog);

	struct _stati64 stat;
	_stati64(path.c_str(), &stat);
	fileSize = stat.st_size;

	sendHSP.setMESSAGE(HSP_MESSAGE_IMAGE_UPLOAD);
	sendHSP.setSize((int)fileSize);
	printf("(%d BYTE)", (int)fileSize);

	sock->send(&sendHSP);


	timer.start();

	//클라이언트가 파일을 받을 준비를 끝내고 OK메세지를 보내기를 대기하는 부분
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
			printf("%d 클라이언트 응답없음.(sendFILE 1) %d\n", hsp.getID(),sendFileLog);
			return;
		}
	}

	printf("전송시작\n");

	timer.start();
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
		
			/*for (int i = 0; i < nbyte; i++)
				printf("%d\n", nbuf[i]);*/
			
			if (sendByte > 0) {
				//printf("send : %d \n", sendByte);
				sendSize += sendByte;

				if (sendSize >= fileSize) {
					printf("AllSendSize : %d sendbyte : %d\n", sendSize, sendByte);
					printf("전송완료\n");

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
			printf("%d 클라이언트 응답없음.(sendFILE 2) %d\n", hsp.getID(), sendFileLog);
			return;
		}
	}

	sendFileLog++;
	printf("전송완료\n");
	list->push(sock);
	delete[] fileName;
	return;
}


//얼굴 검출 함수
//얼굴을 검출 하거나 클라이언트가 포기할 때(MESSAGE_FACEDETECHTING_END) 까지 종료하지 않음
//사진 하나의 크기는 10305BYTE;
void faceDetect(Socket* sock, ClientList* notLoginList, ClientList* loginList) {
	int nbyte = 0;
	int receive_byte = 0;
	int size = 10304;//정해진 값 만약 정해지지않으면 imgUPLOAD를 받고 해야해
	char imgBuf[10305];

	HSProtocol hsp;
	HSProtocol sendHSP;
	

	sendHSP.setServerHSP();
	sendHSP.setMESSAGE(HSP_MESSAGE_OK);
	sock->send(&sendHSP);

	while (true) {
		//이미지 업로드를 시작한다는 메세지 받을 때 까지 대기
		//중간에 얼굴인식을 꺼버리면 스레드 종료
		printf("스레드시작\n");
		int err_cnt = 0;
		while (true) {
			nbyte = sock->recv(&hsp);
			if (nbyte > 0) {
				printf("메세지들어옴");
				if (hsp.getMESSAGE() == HSP_MESSAGE_IMAGE_UPLOAD) {
					printf("업로드시작");
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

		//파일을 전송 받는 부분

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
						nbyte = 10304 - receive_byte;//배열 참조에러나지않게
					}
					memcpy(&imgBuf[receive_byte], buf, nbyte);
					receive_byte += nbyte;
					//printf("%d byte 수신, 총 %d byte\n", nbyte, receive_byte); // 승혁이가 주석 처리 풀음
							
				}

				if (receive_byte >= size) {
					break;
				}
				
			}
			
			//얼굴 가져와서 서버 컴퓨터에 저장.
			Mat Detected_face(Size(92, 112), CV_8UC1);
			Detected_face.data = (unsigned char*)imgBuf;

			equalizeHist(Detected_face, Detected_face);
			imwrite("C:\\PC_att_facess\\B\\" + to_string(imsy_count) + ".png", Detected_face);
			
			int Label = Face_recognition::getInstance()->Face_recog_start((unsigned char*)imgBuf);
			//int Label = -1;
			//printf("Label is %d  cnt : %d\n", Label, imsy_count);
			//printf("=========================================\n");


			//안면인식 후 결과를 전송

			//누군가로 인식이 되었을경우
			// Label = 2;			//만약 얼굴 인식을 억지로 하게끔 할때 Label에 값을 임의로 입력시키면 된다.
			if (Label > -1) {
				loginList->push(sock);
				sendHSP.setMESSAGE(HSP_MESSAGE_LOGIN_START);
				sendHSP.setID(Label);
				sock->send(&sendHSP);
				printf("사용자 [%d]가 로그인 했습니다.\n", Label);
				
				return;
			}
			else {  //unknown으로 인식되었을경우 			
				sendHSP.setMESSAGE(HSP_MESSAGE_FACEDETECTING_UNKNOWNPERSON);
				sock->send(&sendHSP);
				continue; //다시
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
		안면인식하는 코드
		int result = foo(imgBuf);

		안면인식 후 결과를 전송


		if(result == 유저){
			loginList->push(sock);
			sendHSP.setMESSAGE(HSP_MESSAGE_FACEDETECTING_IS_USER);
			sock->send(&sendHSP);
			break;
		}

		else if(result == 관리자){
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


		else if(result == 실패){
			또돌아라!
			continue;
		}
		********************************************/
	}
	printf("스레드종료");
	return;
}


void logout(Socket* sock, ClientList* list, index_t index) {
	sock->close();
	list->remove(index);
	printf("로그아웃 하였습니다.");
}


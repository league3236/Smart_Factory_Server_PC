#pragma once


#include <iostream>
#include <String>
#include "st.h"
#include "PTRList.h"
#include "fileSocket.h"
#include "hsprotocol.h"
#include "Face_recognition.h"
#include <thread>
#include <process.h>
#include <windows.h> 
typedef PTRList<Socket> SocketList;
class ServerMain2 {
	WSADATA wsaData;
	ServerSocket *serverSocket;
	SocketList Llist;
	SocketList Nlist;
	HSProtocol sendHSP;
	HSProtocol clientHSP;
	int serverPort;
	int TraceID = 0;

	struct Arg_faceDetecting {
		ServerMain2* pMain;
		Socket* pSocket;
	};

	struct Arg_getSign {
		ServerMain2* pMain;
		Socket* pSocket;
	};

	struct Arg_sendFile {
		ServerMain2* pMain;
		Socket* pSocket;
		char* pFileName;
	};



	static VOID WINAPI getSignThread(LPVOID arg) {
		Arg_faceDetecting* pArg = (Arg_faceDetecting*)arg;
		pArg->pMain->getSign(pArg->pSocket);
	}

	static VOID WINAPI faceDetectingThread(LPVOID arg) {
		Arg_faceDetecting* pArg = (Arg_faceDetecting*)arg;
		pArg->pMain->faceDetecting(pArg->pSocket);
	}

	static VOID WINAPI sendFileThread(LPVOID arg) {
		Arg_sendFile* pArg = (Arg_sendFile*)arg;
		pArg->pMain->sendFile(pArg->pSocket,pArg->pFileName);
	}


	static VOID WINAPI commandThread(LPVOID arg) {
		ServerMain2* pArg = (ServerMain2*)arg;
		pArg->command();
	}


public:
	explicit ServerMain2(int port) : serverPort(port) {
		sendHSP.setServerHSP();
		
	}

	void start();

	void command();

private:

	void accept();

	int assignID();

	void works();

	void faceDetecting(Socket* sock);

	void getSign(Socket* sock);

	void sendFile(Socket* sock, char* fileName);
};
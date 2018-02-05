#pragma once
#include "socket.h"
#include <Windows.h>
#include <string>

#define HS_FS_RECEIVING 100
#define HS_FS_RECEIVE_DONE 101
#define HS_FS_RECEIVE_FAIL 102
#define HS_FS_SENDING 110
#define HS_FS_SEND_DONE 111
#define HS_FS_SEND_FAIL 111
#define HS_FS_DO_NOTHING 130
#define HS_FS_WRONG_PATH 150

class FileSocket : public Socket {
	const char* path;
public:
	FileSocket(Socket* _sock, const char* _path) : Socket(*_sock), path(_path) {}
};


//Ŭ���̾�Ʈ���� ���� ���� �޴� Ŭ����
class FileFromClientSocket : public FileSocket {
	FILE *file;
	int size;
	int get;
	int error;
public:
	FileFromClientSocket(Socket* _sock,const char* filePath ,const char* fileName) : FileSocket(_sock, fileName) {
		std::string str;
		str.append(filePath);
		if (str.back() != '/' && str.back() != '\\') {
			str.append("/");
		}
		str.append(fileName);	
		printf("PATH  : %s", str.c_str());
		file = fopen(str.c_str(), "wb");
		if (file == nullptr) {
			printf("���� ����");
			error = HS_FS_WRONG_PATH;
		}
		printf("���� ����\n");
		get = 0;
		size = 0;
	}

	int	recvFile() {
		char buf[1024];
		memset(buf, 0, 1024);	
		int nbyte = 0;
		if (error == HS_FS_WRONG_PATH)
			return HS_FS_RECEIVE_FAIL;
		if (size == 0) {
			nbyte = ::recv(Socket::sock, buf, sizeof(buf), 0);
			if (nbyte != -1) {
				buf[nbyte] = '\0';
				size = atoi(buf);
				printf("Size : %d\n", size);
			}
		}
		else {
			nbyte = ::recv(Socket::sock, buf, sizeof(buf), 0);
			
			if (nbyte != -1) {
				get += nbyte;
				fwrite(buf, sizeof(char), nbyte, file);
				return HS_FS_RECEIVING;
			}

			if (get >= size) {
				fclose(file);
				printf("����Ϸ�");
				return HS_FS_RECEIVE_DONE;
			}
		}
		return HS_FS_DO_NOTHING;
	}
};

//Ŭ���̾�Ʈ���� ������ ���� �ϴ� Ŭ����
class FileToClientSocket : public FileSocket {
	FILE *file;
	int n;
	int num;
	int nnn;
	ULONG fileSize;

	void sendSize() {
		char sizeStr[20] = { '\0' };
		itoa(fileSize, sizeStr, 10);
		printf("sizeStr : %s\n", sizeStr);
		send(sizeStr,strlen(sizeStr));
	}

public:
	FileToClientSocket(Socket* _sock, const char* _path) : FileSocket(_sock, _path) {
		struct _stati64 s;
		_stati64(_path, &s);
		fileSize = s.st_size;
		file = fopen(_path, "rb");
		sendSize();
		printf("���� ���� size: %ld\n", s.st_size);
		n = 0;
		nnn = 0;
		num = 1;
	}

	int sendFile() {
		char buf[1024];
		memset(buf, 0, 1024);
		int nbyte = fread(buf, 1024, 1, file);
		int sending = ::send(sock, buf, 1024, 0);
		printf("[%d] read : %d send : %d Send : %d ReadSize : %d\n", num, strlen(buf), sending, nnn, n);
		num++;
		if (feof(file)) {
			if (sending >= fileSize)
				return HS_FS_SEND_DONE;
			else {
				return HS_FS_SEND_FAIL;//����
			}
		}
		return HS_FS_SENDING;


	}
};


#pragma once

#ifdef BUFFER
#undef BUFFER
#endif
#ifdef PARAM
#undef PARAM
#endif 
#include <stdlib.h>


typedef char HSPMESSAGE;
typedef char PARAM;
typedef char* BUFFER;

const HSPMESSAGE HSP_HSP_USEHSP = 111;

const HSPMESSAGE HSP_DEVICE_SERVER = 0;
const HSPMESSAGE HSP_DEVICE_MOBILE = 1;
const HSPMESSAGE HSP_DEVICE_DESKTOP = 2;

const HSPMESSAGE HSP_SERVICEID_SERVER = 0;
const HSPMESSAGE HSP_SERVICEID_ADMIN = 1;
const HSPMESSAGE HSP_SERVICEID_USER = 2;

const HSPMESSAGE HSP_ID_UNKNOWN = 100; //로그인 하기 전에 ID
const HSPMESSAGE HSP_ID_SERVER = 99;   //서버의 ID


const HSPMESSAGE HSP_MESSAGE_OK = 1;
const HSPMESSAGE HSP_MESSAGE_FAIL = 2;
const HSPMESSAGE HSP_MESSAGE_DENIED = 3;//거부
const HSPMESSAGE HSP_MESSAGE_PROGRAM_START = 4;//프로그램 시작합니다.
const HSPMESSAGE HSP_MESSAGE_LOGIN_START = 5;//로그인합니다.
const HSPMESSAGE HSP_MESSAGE_FACEDETECTING_START = 6;//안면인식시작
const HSPMESSAGE HSP_MESSAGE_FACEDETECTING_IS_USER = 7;//안면인식 유저
const HSPMESSAGE HSP_MESSAGE_FACEDETECTING_IS_ADMIN = 8;//안면인식 관리자
const HSPMESSAGE HSP_MESSAGE_FACEDETECTING_UNKNOWNPERSON = 9;//안면인식 알수없는 사람
const HSPMESSAGE HSP_MESSAGE_FACEDETECTING_END = 10;//안면인식 종료
const HSPMESSAGE HSP_MESSAGE_REQUIRED_SIGN = 11;//사인필요해요
const HSPMESSAGE HSP_MESSAGE_SIGN_UPLOAD = 12;//사인업로드할게요
const HSPMESSAGE HSP_MESSAGE_IMAGE_UPLOAD = 13;//이미지업로드할게요 //data영역값있음 (buf[10~29]은 파일사이즈 str로 buf[30~39]는 파일 이름)
const HSPMESSAGE HSP_MESSAGE_PAGE_DONE = 14;//done을 누름
const HSPMESSAGE HSP_MESSAGE_LOGOUT = 15;//로그아웃
const HSPMESSAGE HSP_MESSAGE_REQUEST_FILE = 16;//파일 요구
const HSPMESSAGE HSP_MESSAGE_TRAING = 17;//트레이닝 다시


class HSProtocol {
	BUFFER buf;
	const int legnth = 100;
public:
	HSProtocol() : buf(new char[100]){}
	explicit HSProtocol(BUFFER buffer): buf(buffer) { }

	explicit HSProtocol(
		HSPMESSAGE HSP,
		HSPMESSAGE DEVICE,
		HSPMESSAGE SERVICEID,
		HSPMESSAGE ID,
		HSPMESSAGE MESSAGE) : buf(new char[100])
	{
		
		memset(buf, '\0', 100);
		setHSP(HSP);
		setDEVICE(DEVICE);
		setSERVICEID(SERVICEID);
		setID(ID);
		setMESSAGE(MESSAGE);
	}

	HSProtocol(HSProtocol& hsp) {
		buf = hsp.buf;
	}

	~HSProtocol()
	{

		delete [] buf;
	}

	void setHSP(HSPMESSAGE HSP) {
		buf[0] = HSP;
	}

	void setServerHSP() {
		setHSP(HSP_HSP_USEHSP);
		setSERVICEID(HSP_SERVICEID_SERVER);
		setDEVICE(HSP_DEVICE_SERVER);
		setID(HSP_ID_SERVER);
	}


	HSPMESSAGE getHSP() {
		return buf[0];
	}

	void setDEVICE(HSPMESSAGE DEVICE) {
		buf[1] = DEVICE;
	}

	HSPMESSAGE getDEVICE() {
		return buf[1];
	}

	void setSERVICEID(HSPMESSAGE SERVICEID) {
		buf[2] = SERVICEID;
	}

	HSPMESSAGE getSERVICEID() {
		return buf[2];
	}

	void setID(HSPMESSAGE ID) {
		buf[3] = ID;
	}

	HSPMESSAGE getID() {
		return buf[3];
	}

	void setMESSAGE(HSPMESSAGE MESSAGE) {
		buf[4] = MESSAGE;
	}

	HSPMESSAGE getMESSAGE() {
		return buf[4];
	}

	void setPARAM1(PARAM PARAM1) {
		buf[5] = PARAM1;
	}

	HSPMESSAGE getPARAM1() {
		return buf[5];
	}

	void setPARAM2(PARAM PARAM2) {
		buf[6] = PARAM2;
	}

	HSPMESSAGE getPARAM2() {
		return buf[6];
	}

	void setPARAM3(PARAM PARAM3) {
		buf[7] = PARAM3;
	}

	HSPMESSAGE getPARAM3() {
		return buf[7];
	}

	void setPARAM4(PARAM PARAM4) {
		buf[8] = PARAM4;
	}

	HSPMESSAGE getPARAM4() {
		return buf[8];
	}

	void setARAM5(PARAM PARAM5) {
		buf[9] = PARAM5;
	}

	HSPMESSAGE getARAM5() {
		return buf[9];
	}
	
	operator BUFFER () {
		return buf;
	}

	void getSize(char* sizeBuf) {
		for (int i = 0; i < 20; i++) {
			sizeBuf[i] = buf[i + 10];
			if (buf[i+10] == '\0') {
				break;
			}
		}
	}

	void setSize(int size) {
		char sizeBuf[20];
		_itoa_s(size, sizeBuf, 10);
		for (int i = 0; i < 20; i++) {
			buf[i + 10] = sizeBuf[i];

			if (sizeBuf[i] == '\0')
				break;
		}
	}

	void getFileName(char* fileName) {
		for (int i = 0; i < 30; i++) {
			fileName[i] = buf[i + 30];
			if (buf[i+30] == '\0') 
				break;
		}
	}


	void operator = (HSProtocol hsp) {
		buf = hsp.buf;
	}

	
	void operator = (BUFFER buff) {
		if(buf!=nullptr)
			delete[] buf;
		buf = buff;
	}

	
	

	char operator [] (int n) {

		if (n >= 100 || n < 0)
			return NULL;
		return buf[n];

	}
};

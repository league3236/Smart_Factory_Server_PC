
#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include "opencv.hpp"

using namespace cv;

class Face_recognition
{
private:
	//static  String  CSV_FILE;
	Face_recognition() {};
	static Face_recognition* fr;
public:
	static Face_recognition* getInstance();
	int prediction;
	double Threshold;
	vector<Mat> images;
	vector<int> labels;

	Ptr<FaceRecognizer> model;

	int Face_recog_start(unsigned char* Bdata);

	void Traing_ready();
};

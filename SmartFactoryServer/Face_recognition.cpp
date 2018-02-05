#include "Face_recognition.h"

using namespace cv;
using namespace std;

//String Face_recognition::CSV_FILE = ".\\OPCV\\csv.ext";
String Recog_Pic_Path = "C:\\PC_att_faces";
String Csv_File = ".\\OPCV\\csv.ext";
Face_recognition* Face_recognition::fr = NULL;

Face_recognition* Face_recognition::getInstance() {
	if (!fr) {
		fr = new Face_recognition();
	}
	return fr;
}

void create_csv() {//csv���� ����     ����: python 'create_csv.py���' 'att_faces���' > 'csv.ext���������� ���'
	String temp = "python .\\OPCV\\create_csv.py ";
	temp+= Recog_Pic_Path + " > " + Csv_File;

	system(temp.c_str());
	//printf("%s\n", temp.c_str());
}

static void read_csv(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';') {
	std::ifstream file(filename.c_str(), ifstream::in);
	if (!file) {
		string error_message = "No valid input file was given, please check the given filename.";
		CV_Error(CV_StsBadArg, error_message);
	}
	string line, path, classlabel;
	images.clear();
	labels.clear();
	while (getline(file, line)) {
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, classlabel);
		if (!path.empty() && !classlabel.empty()) {
			images.push_back(imread(path, 0));
			labels.push_back(atoi(classlabel.c_str()));
		}
	}
}


void Face_recognition::Traing_ready() {

	create_csv(); //training �� ���� ������� ���� ����

	Threshold = 3300.0; //unknown person �Ӱ谪 (���� ������� �Ǻ��� ��������)
	
	try {
		read_csv(Csv_File, images, labels);
	}
	catch (cv::Exception& e) {
		cerr << "Error opening file \"" << Csv_File << "\". Reason: " << e.msg << endl;
		exit(1);
	}

	model = createEigenFaceRecognizer(80, Threshold);  // threshold ĸ����ǿ��� 3300�߾���  //2500���Ͽ����� �νķ� ������
	//model = createEigenFaceRecognizer();
	
	model->train(images, labels);
	cout << "trainning complete" << endl;
}

int Face_recognition::Face_recog_start(unsigned char* Bdata) {

	//////////////////////////////////////////////////////////

	Mat Detected_face(Size(92, 112), CV_8UC1);
	Detected_face.data = Bdata;

	equalizeHist(Detected_face, Detected_face);
	prediction = model->predict(Detected_face);

	return prediction;
}
// CS 510 Intro to Computer Vision (Winter 2016)
// Student: Jong Seong Lee
// Project 2 Face Detection
// 02/21/2016

// *** Description:
// This program will demonstrate Viola Jones face detection and
// Eigenfaces, Fisherfaces, Local Binary Patterns Histograms face recognitions.

// *** Notice:
// Please run this program in release mode!
// Please add the environment variable "OPENCV_DIR" with the value of "opencv directory"\build\x86\vc12
// (For example, C:\Opencv\build\x86\vc12)

// *** Prerequisites:
// 1. Opencv 2.4.10

// *** Develpment Environment:
// 1. Visual Studio 2013
// 2. Windows 7 64bit

// *** Code References:
// 1. Viola Jones Face Detection
// http://study.marearts.com/2014/09/opencv-face-detection-using-adaboost.html
// 2. Face Recognition (Eigenfaces / Fisherfaces / Local Binary Patterns Histograms)
// http://docs.opencv.org/2.4/modules/contrib/doc/facerec/tutorial/facerec_video_recognition.html
// https://github.com/gihantharanga/ComputerVision

#include <iostream>
#include <string>
#include <fstream>
#include <sstream>

#include "opencv2\core\core.hpp"
#include "opencv2\contrib\contrib.hpp"
#include "opencv2\highgui\highgui.hpp"
#include "opencv2\objdetect\objdetect.hpp"
#include "opencv2\opencv.hpp"

using namespace std;
using namespace cv;

// constants
string const XMLFILE = "haarcascade_frontalface_default.xml";
string const CSV = "at.txt";
string const EIGEN = "eigenface.yml";
string const FISHER = "fisherface.yml";
string const LBPH = "LBPHface.yml";
string const IMAGE = "group.jpg";
string const NAME = "Jong";

int const LABEL = 39;

enum FaceRec { FisherRec, EigenRec, LBPHRec };

void violaJones(string imageFile) {
	Mat img = imread(imageFile, CV_LOAD_IMAGE_COLOR);

	Mat grayImg(img.size(), CV_8UC1);
	cvtColor(img, grayImg, COLOR_BGR2GRAY);
	equalizeHist(grayImg, grayImg);

	CascadeClassifier face_cascade;
	bool test = face_cascade.load(XMLFILE);

	vector<Rect> faces;

	if (!grayImg.empty() && test) {
		face_cascade.detectMultiScale(grayImg, faces, 1.1, 3, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));
	}

	if (faces.size() >= 1)
	{
		for (int ji = 0; ji < faces.size(); ++ji)
		{
			rectangle(img, faces[ji], CV_RGB(0, 0, 255), 4);
		}
	}

	imshow("result", img);
	waitKey(0);
}

static Mat MatNorm(InputArray _src) {
	Mat src = _src.getMat();
	// Create and return normalized image:
	Mat dst;
	switch (src.channels()) {
	case 1:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
		break;
	case 3:
		cv::normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
		break;
	default:
		src.copyTo(dst);
		break;
	}
	return dst;
}

static void dbread(const string& filename, vector<Mat>& images, vector<int>& labels, char separator = ';'){
	std::ifstream file(filename.c_str(), ifstream::in);

	if (!file){
		string error = "no valid input file";
		CV_Error(CV_StsBadArg, error);
	}

	string line, path, label;
	while (getline(file, line))
	{
		stringstream liness(line);
		getline(liness, path, separator);
		getline(liness, label);
		if (!path.empty() && !label.empty()){
			images.push_back(imread(path, 0));
			labels.push_back(atoi(label.c_str()));
		}
	}
}

void eigenFaceTrainer(){
	vector<Mat> images;
	vector<int> labels;

	try{
		string filename = CSV;
		dbread(filename, images, labels);

		cout << "size of the images is " << images.size() << endl;
		cout << "size of the labes is " << labels.size() << endl;
		cout << "Training begins...." << endl;
	}
	catch (cv::Exception& e){
		cerr << " Error opening the file " << e.msg << endl;
		exit(1);
	}

	//create algorithm eigenface recognizer
	Ptr<FaceRecognizer>  model = createEigenFaceRecognizer();
	//train data
	model->train(images, labels);

	model->save(EIGEN);

	cout << "Training finished...." << endl;

	waitKey(10000);
}

void fisherFaceTrainer(){
	/*in this two vector we put the images and labes for training*/
	vector<Mat> images;
	vector<int> labels;

	try{
		string filename = CSV;
		dbread(filename, images, labels);

		cout << "size of the images is " << images.size() << endl;
		cout << "size of the labes is " << labels.size() << endl;
		cout << "Training begins...." << endl;
	}
	catch (cv::Exception& e){
		cerr << " Error opening the file " << e.msg << endl;
		exit(1);
	}


	Ptr<FaceRecognizer> model = createFisherFaceRecognizer();

	model->train(images, labels);

	int height = images[0].rows;

	model->save(FISHER);

	cout << "Training finished...." << endl;

	Mat eigenvalues = model->getMat("eigenvalues");
	// And we can do the same to display the Eigenvectors (read Eigenfaces):
	Mat W = model->getMat("eigenvectors");
	// Get the sample mean from the training data
	Mat mean = model->getMat("mean");
	
	waitKey(10000);
}

void LBPHFaceTrainer(){

	vector<Mat> images;
	vector<int> labels;

	try{
		string filename = CSV;
		dbread(filename, images, labels);

		cout << "size of the images is " << images.size() << endl;
		cout << "size of the labes is " << labels.size() << endl;
		cout << "Training begins...." << endl;
	}
	catch (cv::Exception& e){
		cerr << " Error opening the file " << e.msg << endl;
		exit(1);
	}

	//lbph face recognier model
	Ptr<FaceRecognizer> model = createLBPHFaceRecognizer();

	//training images with relevant labels 
	model->train(images, labels);

	//save the data in yaml file
	model->save(LBPH);

	cout << "training finished...." << endl;

	waitKey(10000);
}

//lbpcascades works in lbphrecognier as fast as haarcascades 
int  FaceRecognition(string ymlFilename, FaceRec facerec){

	cout << "start recognizing..." << endl;

	//load pre-trained data sets
	Ptr<FaceRecognizer> model;
	
	if (facerec == FisherRec) {
		model = createFisherFaceRecognizer();
	}
	else if (facerec == EigenRec) {
		model = createEigenFaceRecognizer();
	}
	else {
		model = createLBPHFaceRecognizer();
	}
	
	model->load(ymlFilename);

	Mat testSample = imread("att_faces/s40/5.pgm", 0);

	int img_width = testSample.cols;
	int img_height = testSample.rows;


	//lbpcascades/lbpcascade_frontalface.xml
	string classifier = XMLFILE;

	CascadeClassifier face_cascade;
	string window = "Capture - face detection";

	if (!face_cascade.load(classifier)){
		cout << " Error loading file" << endl;
		return -1;
	}

	//VideoCapture cap(0);
	VideoCapture cap("video.mp4");

	if (!cap.isOpened())
	{
		cout << "exit" << endl;
		return -1;
	}

	namedWindow(window, 1);
	long count = 0;

	//recognition rate
	int rec_rate = 0;
	int det_rate = 0;

	while (true)
	{	
		vector<Rect> faces;
		Mat frame;
		Mat graySacleFrame;
		Mat original;

		cap >> frame;
		count = count + 1;

		if (!frame.empty()){

			//clone from original frame
			original = frame.clone();

			//convert image to gray scale and equalize
			cvtColor(original, graySacleFrame, CV_BGR2GRAY);

			//detect face in gray image
			face_cascade.detectMultiScale(graySacleFrame, faces, 1.1, 3, 0, cv::Size(90, 90));

			//number of faces detected
			cout << faces.size() << " faces detected" << endl;
			std::string frameset = std::to_string(count);
			std::string faceset = std::to_string(faces.size());

			int width = 0, height = 0;

			//person name
			string Pname = "";

			for (int i = 0; i < faces.size(); i++)
			{
				det_rate++;

				//region of interest
				Rect face_i = faces[i];

				//crop the roi from grya image
				Mat face = graySacleFrame(face_i);

				//resizing the cropped image to suit to database image sizes
				Mat face_resized;
				cv::resize(face, face_resized, Size(img_width, img_height), 1.0, 1.0, INTER_CUBIC);

				//recognizing what faces detected
				int label = -1; double confidence = 0;
				model->predict(face_resized, label, confidence);

				cout << " confidencde " << confidence << ", label " << label << endl;

				//drawing green rectagle in recognize face
				rectangle(original, face_i, CV_RGB(0, 255, 0), 1);
				string text = "Detected";
				if (label == LABEL){
					Pname = NAME;
					rec_rate++;
				}
				else{
					Pname = "unknown";
				}


				int pos_x = std::max(face_i.tl().x - 10, 0);
				int pos_y = std::max(face_i.tl().y - 10, 0);

				//name the person who is in the image
				putText(original, text, Point(pos_x, pos_y), FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 255, 0), 1.0);
			}


			putText(original, "Frames: " + frameset, Point(30, 60), CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 255, 0), 1.0);
			putText(original, "Person: " + Pname, Point(30, 90), CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 255, 0), 1.0);
			putText(original, "Detection: " + std::to_string(double(det_rate) / double(count)), Point(30, 120), CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 255, 0), 1.0);
			putText(original, "Recognition: " + std::to_string(double(rec_rate) / double(count)), Point(30, 150), CV_FONT_HERSHEY_COMPLEX_SMALL, 1.0, CV_RGB(0, 255, 0), 1.0);
			//display to the winodw
			cv::imshow(window, original);

		}

		if (waitKey(30) >= 0) break;
	}
}

int main() {
	cout << "Please select among following:\n"
		<< "1. Single Image Face Detection\n"
		<< "2. Fisher Face Trainer\n"
		<< "3. Video Face Recognition with Fisherfaces\n"
		<< "4. Eigen Face Trainer\n"
		<< "5. Video Face Recognition with Eigenfaces\n"
		<< "6. LBPH Face Trainer\n"
		<< "7. Video Face Recognition with LBPH\n";

	int selection;
	cin >> selection;

	if (!cin) {
		cout << "Wrong input, please try again...";
	}
	else {
		switch (selection) {
		case 1:
			violaJones(IMAGE);
			break;
		case 2:
			fisherFaceTrainer();
			break;
		case 3:
			FaceRecognition(FISHER, FisherRec);
			break;
		case 4:
			eigenFaceTrainer();
			break;
		case 5:
			FaceRecognition(EIGEN, EigenRec);
			break;
		case 6:
			LBPHFaceTrainer();
			break;
		case 7:
			FaceRecognition(LBPH, LBPHRec);
			break;
		default:
			cout << "Wrong input, please try again...";
			break;
		}
	}

	system("pause");
	return 0;
}
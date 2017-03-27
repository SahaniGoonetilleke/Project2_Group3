#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/features2d.hpp"
//#include "opencv2/xfeatures2d.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>

using namespace cv;
using namespace std;
//using namespace cv::xfeatures2d;

//Global variables
Mat monkey, man, morphed, dst;
int alpha = 0; int const max_elem = 100;
string face_cascade_name = "../../haarcascades/haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "../../haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
vector<Point2f> manPoints;
vector<Point2f> monkeyPoints;

// Linear blend/ Adding - simplest ;)
void Blend(int, void*)
{
	double A = alpha / 100.0; double beta = (1.0 - A);
	addWeighted(monkey, A, man, beta, 0.0, dst);
	imshow("Morphed", dst);
}

// Facial feature detection - face, eyes, mouth - Or select points
void face(Mat image)
{
	std::vector<Rect> faces;
	Mat imageGray, gray;
	cvtColor(image, imageGray, COLOR_BGR2GRAY);
	equalizeHist(imageGray, imageGray);			// Histogram equalization

												// Detect faces
	face_cascade.detectMultiScale(imageGray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	// Set Region of Interest
	cv::Rect roi_b;
	cv::Rect roi_c;
	size_t ic = 0;								// current face index
	int ac = 0;									// current face area
	size_t ib = 0;								// biggest face index
	int ab = 0;									// biggest face area
	for (ic = 0; ic < faces.size(); ic++)		// Iterate through all current faces (detected faces)
	{
		roi_c.x = faces[ic].x;
		roi_c.y = faces[ic].y;
		roi_c.width = (faces[ic].width);
		roi_c.height = (faces[ic].height);
		ac = roi_c.width * roi_c.height;		// area of current face (detected face)
		roi_b.x = faces[ib].x;
		roi_b.y = faces[ib].y;
		roi_b.width = (faces[ib].width);
		roi_b.height = (faces[ib].height);
		ab = roi_b.width * roi_b.height;		// area of biggest face(initially same as current face)
		if (ac > ab)							// Check if current larger than biggest face
		{
			ib = ic;
			roi_b.x = faces[ib].x;
			roi_b.y = faces[ib].y;
			roi_b.width = (faces[ib].width);
			roi_b.height = (faces[ib].height);
		}
		Point pt1(faces[ic].x, faces[ic].y);		// Display detected face
		Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));
		rectangle(image, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
	}
	imshow("face", image);
	waitKey(0);
}

// Mouse clicks
void MouseClick(int event, int x, int y, int flags, void* userdata)
{
	//char coordinate[16];
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << endl;
		manPoints.push_back(Point(x, y));
		//vector<Point>* ptPtr = (vector<Point>*)userdata;
		//ptPtr->push_back(cv::Point(x, y));
		cout << "Mouse clicked position (" << x << " " << y << ")" << endl;
	}
}
void MonkeyMouseClick(int event, int x, int y, int flags, void* userdata)
{
	//char coordinate[16];
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << endl;
		monkeyPoints.push_back(Point(x, y));
		cout << "Mouse clicked position (" << x << " " << y << ")" << endl;
	}
}

// Warping


// Main function
int main(int argc, char** argv)
{
	// Load the cascade
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -2; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return -3; };

	// Read the images
	monkey = imread("../../Images/monkey1.jpg");
	man = imread("../../Images/man1.jpg");
	
	//convert Mat to float data type
	//monkey.convertTo(monkey, CV_32F);
	//man.convertTo(man, CV_32F);
	//empty average image
	//Mat imgMorph = Mat::zeros(img1.size(), CV_32FC3);

	resize(monkey, monkey, man.size());// Resize monkey to be the same size as man
	morphed = man.clone();

	namedWindow("Man", 1);
	//set the callback function for mouse event
	setMouseCallback("Man", MouseClick, NULL);
	imshow("Man", man);
	waitKey(0);
	//save coordinates to txt file
	ofstream file("../../../Images/Man.txt");			
	vector<Point2f>::iterator it = manPoints.begin();
	for (; it != manPoints.end(); ++it)
	{
		file << it->x << " " << it->y << endl;
	}
	file.close();
	cout << "Man Points\n" << manPoints << endl;

	namedWindow("Monkey", 1);
	//set the callback function for mouse event
	setMouseCallback("Monkey", MonkeyMouseClick, NULL);
	imshow("Monkey", monkey);
	waitKey(0);
	//save coordinates to txt file
	ofstream file2("../../../Images/Monkey.txt");			
	vector<Point2f>::iterator mt = monkeyPoints.begin();
	for (; mt != monkeyPoints.end(); ++mt)
	{
		file2 << mt->x << " " << mt->y << endl;	
	}
	file2.close();
	cout << "Monkey Points\n" << monkeyPoints << endl;

	vector<Point2f> points;
	alpha = 0.5;
	//compute weighted average point coordinates
	for (int i = 0; i < manPoints.size(); i++)
	{
		float x, y;
		x = (1 - alpha) * manPoints[i].x + alpha * monkeyPoints[i].x;
		y = (1 - alpha) * manPoints[i].y + alpha * monkeyPoints[i].y;
		points.push_back(Point2f(x, y));
	}
	cout << "Weighted Points\n" << points << endl;

	// Triangulation




	// Classifier
//	face(man); face(monkey);


	namedWindow("Morphed", 1);
	imshow("Morphed", morphed);
	createTrackbar("Alpha", "Morphed", &alpha, max_elem, Blend);
	waitKey(0);
}
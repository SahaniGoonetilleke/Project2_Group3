#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <stdio.h>

using namespace cv;
using namespace std;

//Global variables
Mat monkey, man, dst;
int alpha = 0; int const max_elem = 100;
string face_cascade_name = "../../haarcascades/haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "../../haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;

// Linear blend/ Adding - simplest ;)
void Blend(int, void*)
{
	double A = alpha / 100.0; double beta = (1.0 - A);
	addWeighted(monkey, A, man, beta, 0.0, dst);
	imshow("image", dst);
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

// Warping



int main(int argc, char** argv)
{
	// Load the cascade
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -2; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return -3; };

	// Read the images
	monkey = imread("../../Images/monkey1.jpg");
	man = imread("../../Images/man2.jpg");

	// Apply the classifier
	if (!man.empty()) { face(man); }
	face(monkey);

	resize(monkey, monkey, man.size());// Resize monkey to be the same size as man
	namedWindow("image");
	imshow("image", man);
	createTrackbar("Alpha", "image", &alpha, max_elem, Blend);
	waitKey(0);
}

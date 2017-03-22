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
	resize(monkey, monkey, man.size()); // Resize monkey to be the same size as man
	namedWindow("image");
	imshow("image", man);
	createTrackbar("Alpha", "image", &alpha, max_elem, Blend);	// Linear

	waitKey(0);
	return 0;
}
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
Mat morphedMan, morphedMonkey;
int a = 0; int const max_elem = 100;
string face_cascade_name = "../../haarcascades/haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "../../haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
vector<Point2f> manPoints;
vector<Point2f> monkeyPoints;

// Linear blend/ Adding - simplest ;)
void Blend(int, void*)
{
	double A = a / 100.0; double beta = (1.0 - A);
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
	destroyAllWindows();
}

// Mouse clicks
void MouseClick(int event, int x, int y, int flags, void* userdata)
{
	//char coordinate[16];
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << endl;
		manPoints.push_back(Point2f(x, y));
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
		monkeyPoints.push_back(Point2f(x, y));
		cout << "Mouse clicked position (" << x << " " << y << ")" << endl;
	}
}




// Warping
//Triangulation
void draw_subdivide(Mat &img, Subdiv2D& subdiv, Scalar delaunay_color)
{
	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point> pt(3);
	for (size_t i = 0; i < triangleList.size(); ++i)
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
		line(img, pt[0], pt[1], delaunay_color, 1);
		line(img, pt[1], pt[2], delaunay_color, 1);
		line(img, pt[2], pt[0], delaunay_color, 1);
	}
	//destroyAllWindows();
}




static void draw_subdiv_point(Mat& img, Point2f fp, Scalar color)
{
	circle(img, fp, 3, color, FILLED, LINE_8, 0);
}

static void draw_subdiv(Mat& img, Subdiv2D& subdiv, Scalar delaunay_color)
{
#if 1
	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point> pt(3);

	for (size_t i = 0; i < triangleList.size(); i++)
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
		line(img, pt[0], pt[1], delaunay_color, 1, LINE_AA, 0);
		line(img, pt[1], pt[2], delaunay_color, 1, LINE_AA, 0);
		line(img, pt[2], pt[0], delaunay_color, 1, LINE_AA, 0);
	}
#else
	vector<Vec4f> edgeList;
	subdiv.getEdgeList(edgeList);
	for (size_t i = 0; i < edgeList.size(); i++)
	{
		Vec4f e = edgeList[i];
		Point pt0 = Point(cvRound(e[0]), cvRound(e[1]));
		Point pt1 = Point(cvRound(e[2]), cvRound(e[3]));
		line(img, pt0, pt1, delaunay_color, 1, LINE_AA, 0);
	}
#endif
}

static void locate_point(Mat& img, Subdiv2D& subdiv, Point2f fp, Scalar active_color)
{
	int e0 = 0, vertex = 0;

	subdiv.locate(fp, e0, vertex);

	if (e0 > 0)
	{
		int e = e0;
		do
		{
			Point2f org, dst;
			if (subdiv.edgeOrg(e, &org) > 0 && subdiv.edgeDst(e, &dst) > 0)
				line(img, org, dst, active_color, 3, LINE_AA, 0);

			e = subdiv.getEdge(e, Subdiv2D::NEXT_AROUND_LEFT);
		} while (e != e0);
	}

	draw_subdiv_point(img, fp, active_color);
}
































// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(Mat &warpImage, Mat &src, vector<Point2f> &srcTri, vector<Point2f> &dstTri)
{
	// Given a pair of triangles, find the affine transform.
	Mat warpMat = getAffineTransform(srcTri, dstTri);
	// Apply the Affine Transform just found to the src image
	warpAffine(src, warpImage, warpMat, warpImage.size(), INTER_LINEAR, BORDER_REFLECT_101);
}

// Warps and alpha blends triangular regions from img1 and img2 to img
void morphTriangle(Mat &img1, Mat &img2, Mat &img, vector<Point2f> &t1, vector<Point2f> &t2, vector<Point2f> &t, double alpha)
{
	// Find bounding rectangle for each triangle
	Rect r = boundingRect(t);
	Rect r1 = boundingRect(t1);
	Rect r2 = boundingRect(t2);
	// Offset points by left top corner of the respective rectangles
	vector<Point2f> t1Rect, t2Rect, tRect;
	vector<Point> tRectInt;
	for (int i = 0; i < 3; i++)
	{
		tRect.push_back(Point2f(t[i].x - r.x, t[i].y - r.y));
		tRectInt.push_back(Point(t[i].x - r.x, t[i].y - r.y)); // for fillConvexPoly
		t1Rect.push_back(Point2f(t1[i].x - r1.x, t1[i].y - r1.y));
		t2Rect.push_back(Point2f(t2[i].x - r2.x, t2[i].y - r2.y));
	}
	// Get mask by filling triangle
	Mat mask = Mat::zeros(r.height, r.width, CV_32FC3);
	fillConvexPoly(mask, tRectInt, Scalar(1.0, 1.0, 1.0), 16, 0);
	// Apply warpImage to small rectangular patches
	Mat img1Rect, img2Rect;
	img1(r1).copyTo(img1Rect);
	img2(r2).copyTo(img2Rect);
	Mat warpImage1 = Mat::zeros(r.height, r.width, img1Rect.type());
	Mat warpImage2 = Mat::zeros(r.height, r.width, img2Rect.type());
	applyAffineTransform(warpImage1, img1Rect, t1Rect, tRect);
	applyAffineTransform(warpImage2, img2Rect, t2Rect, tRect);
	// Alpha blend rectangular patches
	Mat imgRect = (1.0 - alpha) * warpImage1 + alpha * warpImage2;
	// Copy triangular region of the rectangular patch to the output image
	multiply(imgRect, mask, imgRect);
	multiply(img(r), Scalar(1.0, 1.0, 1.0) - mask, img(r));
	img(r) = img(r) + imgRect;
}














// Draw delaunay triangles
static void draw_delaunay(Mat& img, Subdiv2D& subdiv, Scalar delaunay_color)
{
	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point> pt(3);
	Size size = img.size();
	Rect rect(0, 0, size.width, size.height);
	for (size_t i = 0; i < triangleList.size(); i++)
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
		// Draw rectangles completely inside the image.
		if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
		{
			line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
			line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
			line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
		}
	}
	//---------------
	//save triangle coordinates to txt file
	//ofstream fileTriMan("../../../Images/ManTri.txt");
	//vector<Vec6f>::iterator t = triangleList.begin();
	//for (int t = 0; t < triangleList.size(); ++t)
	//{
	//	fileTriMan << triangleList[t] << endl;
	//}
	//fileTriMan.close();
	//------------------
}

// Main function
int main(int argc, char** argv)
{
	// Load the cascade
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -2; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return -3; };

	// Read the images
	monkey = imread("../../Images/monkey1.jpg");
	man = imread("../../Images/man1.jpg");
	
	resize(monkey, monkey, man.size());// Resize monkey to be the same size as man
	morphed = man.clone();
	morphedMan = man.clone();
	morphedMonkey = monkey.clone();

	namedWindow("Morphed", 1);
	imshow("Morphed", morphed);
	createTrackbar("Alpha", "Morphed", &a, max_elem, Blend);
	waitKey(0);

	// Classifier
	face(morphedMan); face(morphedMonkey);

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
	double alpha = 0.5;			//alpha controls the degree of morph
	//compute weighted average point coordinates
	for (int i = 0; i < manPoints.size(); i++)
	{
		float x, y;
		x = (1 - alpha) * manPoints[i].x + alpha * monkeyPoints[i].x;
		y = (1 - alpha) * manPoints[i].y + alpha * monkeyPoints[i].y;
		points.push_back(Point2f(x, y));
	}
	cout << "Weighted Points\n" << points << endl;

	//convert Mat to float data type
	//monkey.convertTo(monkey, CV_32F);
	//man.convertTo(man, CV_32F);
	//empty average image
	//Mat imgMorph = Mat::zeros(img1.size(), CV_32FC3);

	//...........
	Scalar delaunay_color(255, 255, 255), points_color(0, 0, 255); // Define colors for drawing.
	bool animate = true;					// Turn on animation while drawing triangles
	//............
	
	// Triangulation points *******
	Rect rectMan(0, 0, man.cols, man.rows);	// Outer bounding box
	Subdiv2D subdivMan(rectMan);			// Create initial subdivision
	Point2f fpMan;							// Point holder						
	
	// Insert points into subdiv
	//for (int ip = 0; ip < manPoints.size(); ip++)
	for (vector<Point2f>::iterator ip = manPoints.begin(); ip != manPoints.end(); ip++)
	{
		//fpMan = manPoints[ip];			// 32f points
		subdivMan.insert(*ip);
		if (animate)
		{
			Mat img_copy = man.clone();
			// Draw delaunay triangles
			draw_subdiv(img_copy, subdivMan, delaunay_color);
			imshow("Delaunay Triangulation", img_copy);
			waitKey(0);
		}
	}
	// Draw delaunay triangles
	draw_subdivide(man, subdivMan, delaunay_color);

	// Draw points
	for (vector<Point2f>::iterator it = points.begin(); it != points.end(); it++)
	{
		circle(man, *it, 2, points_color, CV_FILLED, CV_AA, 0);
	}

	imshow("Delaunay Triangulation", man);
	waitKey(0);







	Rect rectMonkey(0, 0, monkey.cols, monkey.rows);	// Outer bounding box - Same as man
	Subdiv2D subdivMonkey(rectMonkey);		// Create initial subdivision
	Point2f fpMonkey;					// Point holder
	for (int i = 0; i < monkeyPoints.size(); i++)
	{
		fpMonkey = monkeyPoints[i];		// 32f points
		subdivMonkey.insert(fpMonkey);
	}
	vector<Vec6f> trianglesMonkey;
	subdivMonkey.getTriangleList(trianglesMonkey);







	//Read triangle indices
	ifstream ifs("../../../Images/tri.txt");
	double x, y, z;
	while (ifs >> x >> y >> z)
	{
		// Triangles
		vector<Point2f> t1, t2, t;
		// Triangle corners for man
		cout << manPoints[x] << endl;
		t1.push_back(manPoints[x]);
		t1.push_back(manPoints[y]);
		t1.push_back(manPoints[z]);
		// Triangle corners for monkey
		t2.push_back(monkeyPoints[x]);
		t2.push_back(monkeyPoints[y]);
		t2.push_back(monkeyPoints[z]);
		// Triangle corners for morphed image.
		t.push_back(points[x]);
		t.push_back(points[y]);
		t.push_back(points[z]);
		morphTriangle(man, monkey, morphed, t1, t2, t, alpha);
	}

	// Display Result
	imshow("Morphed Face", morphed / 255.0);
	waitKey(0);


	namedWindow("Morphed", 1);
	imshow("Morphed", morphed);
	createTrackbar("Alpha", "Morphed", &a, max_elem, Blend);
	waitKey(0);
}
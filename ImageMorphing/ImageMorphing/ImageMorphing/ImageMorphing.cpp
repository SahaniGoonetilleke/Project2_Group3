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
Mat manWarp, monkeyWarp, morphedManWarp, morphedMonkeyWarp;
int a = 0; int const max_elem = 100;
string face_cascade_name = "../../haarcascades/haarcascade_frontalface_alt.xml";
string eyes_cascade_name = "../../haarcascades/haarcascade_eye_tree_eyeglasses.xml";
CascadeClassifier face_cascade;
CascadeClassifier eyes_cascade;
vector<Point2f> manPoints;
vector<Point2f> monkeyPoints;
vector<Point2f> points;
vector<Vec6f> Mantri, Monkeytri, Morphedtri;

/// Linear blend/ Adding
void Blend(int, void*)
{
	double A = a / 100.0; double beta = (1.0 - A);
	addWeighted(monkey, A, man, beta, 0.0, dst);
	imshow("Morphed", dst);
}

/// Facial feature detection - face, eyes, mouth - Or select points
void face(Mat image)
{
	std::vector<Rect> faces;
	Mat imageGray, gray;
	cvtColor(image, imageGray, COLOR_BGR2GRAY);
	equalizeHist(imageGray, imageGray);			// Histogram equalization
	// Detect faces
	face_cascade.detectMultiScale(imageGray, faces, 1.1, 2, 0 | CASCADE_SCALE_IMAGE, Size(30, 30));
	// Set Region of Interest
	cv::Rect roi_b;								// Biggest face ROI
	cv::Rect roi_c;								// Current face ROI
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
		// Display detected face
		Point pt1(faces[ic].x, faces[ic].y);	
		Point pt2((faces[ic].x + faces[ic].height), (faces[ic].y + faces[ic].width));
		rectangle(image, pt1, pt2, Scalar(0, 255, 0), 2, 8, 0);
	}
	imshow("face", image);
	waitKey(0);
	destroyAllWindows();
}

/// Mouse clicks to select points
void MouseClick(int event, int x, int y, int flags, void* userdata)
{
	//char coordinate[16];
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << endl;
		manPoints.push_back(Point2f(x, y));
		//vector<Point>* ptPtr = (vector<Point>*)userdata;
		//ptPtr->push_back(cv::Point(x, y));
	}
}
void MonkeyMouseClick(int event, int x, int y, int flags, void* userdata)
{
	//char coordinate[16];
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << endl;
		monkeyPoints.push_back(Point2f(x, y));
	}
}

///Triangulation - Draws Delaunay triangles
static void draw_delaunay(Mat& img, Subdiv2D& subdiv, Scalar delaunay_color, string name)
{
	vector<Vec6f> triangleList;				// (x1,y1,x2,y2,x3,y3)
	subdiv.getTriangleList(triangleList);
	vector<Point> pt(3);
	Size size = img.size();
	Rect rect(0, 0, size.width, size.height);
	cout << name << "Triangle List" << endl;
	for (size_t i = 0; i < triangleList.size(); i++)
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
		cout << t << endl;
		// Draw rectangles only if completely inside the image.
		if (rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
		{
			line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
			line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
			line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);
		}
		//else
		//{
		//	triangleList.erase(triangleList.begin() + i);
		//}
	}
	cout << "Wait 1 min..." << endl;
	waitKey(60000);	// Wait 60s
	//save triangle coordinates to txt file
	ofstream fileTriMan;
	fileTriMan.open("../../../Images/" + name + "Tri.txt");
	vector<Vec6f>::iterator t = triangleList.begin();
	for (int t = 0; t < triangleList.size(); ++t)
	{
		fileTriMan << triangleList[t] << endl;
	}
	fileTriMan.close();
	if (name == "Morphed") { Morphedtri = triangleList; }
	else if (name == "Man") { Mantri = triangleList; }
	else { Monkeytri = triangleList; }
}






//*********************************************************
// Warping


//xxxxxxxxxxxx
void draw_subdivide(Mat &img, Subdiv2D& subdiv, Scalar delaunay_color)
{
	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);
	vector<Point> pt(3);
	for (size_t i = 0; i < triangleList.size(); ++i)
	{
		Vec6f t = triangleList[i];
		cout << t << endl;
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));
		line(img, pt[0], pt[1], delaunay_color, 1);
		line(img, pt[1], pt[2], delaunay_color, 1);
		line(img, pt[2], pt[0], delaunay_color, 1);
	}
	//destroyAllWindows();
}



//xxxxxxxxxxxxxxx
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
		cout << t << endl;
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
//xxxxxxxxxxxxxxx
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


// *********************************************************4
// WARP CODE 1
// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(Mat &warpImage, Mat &src, vector<Point2f> &srcTri, vector<Point2f> &dstTri)
{
	// Given a pair of triangles, find the affine transform.
	Mat warpMat = getAffineTransform(srcTri, dstTri);
	// Apply the Affine Transform just found to the src image
	warpAffine(src, warpImage, warpMat, warpImage.size(), INTER_LINEAR, BORDER_REFLECT_101);
}
// Warps and alpha blends triangular regions from img1 and img2 to img **********
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

//WARP CODE 2
// Warps and alpha blends triangular regions from img1 and img2 to img
void warpTriangle(Mat &img1, Mat &img2, vector<Vec6f> tri1, vector<Vec6f> tri2)
{
	vector<Vec6f>::iterator t = tri1.begin();
	for (int t = 0; t < tri1.size(); ++t)
	{
		// Find bounding rectangle for each triangle
		Rect r1 = boundingRect(tri1[t]);
		Rect r2 = boundingRect(tri2[t]);

		// Offset points by left top corner of the respective rectangles
		vector<Point2f> tri1Cropped, tri2Cropped;
		vector<Point> tri2CroppedInt;
		for (int i = 0; i < 3; i++)
		{
			tri1Cropped.push_back(Point2f(tri1[t][i] - r1.x, tri1[t][i] - r1.y));
			tri2Cropped.push_back(Point2f(tri2[t][i] - r2.x, tri2[t][i] - r2.y));

			// fillConvexPoly needs a vector of Point and not Point2f
			tri2CroppedInt.push_back(Point((int)(tri2[t][i] - r2.x), (int)(tri2[t][i] - r2.y)));

		}

		// Apply warpImage to small rectangular patches
		Mat img1Cropped;
		img1(r1).copyTo(img1Cropped);

		// Given a pair of triangles, find the affine transform.
		Mat warpMat = getAffineTransform(tri1Cropped, tri2Cropped);

		// Apply the Affine Transform just found to the src image
		Mat img2Cropped = Mat::zeros(r2.height, r2.width, img1Cropped.type());
		warpAffine(img1Cropped, img2Cropped, warpMat, img2Cropped.size(), INTER_LINEAR, BORDER_REFLECT_101);

		// Get mask by filling triangle
		Mat mask = Mat::zeros(r2.height, r2.width, CV_32FC3);
		fillConvexPoly(mask, tri2CroppedInt, Scalar(1.0, 1.0, 1.0), 16, 0);

		// Copy triangular region of the rectangular patch to the output image
		multiply(img2Cropped, mask, img2Cropped);
		multiply(img2(r2), Scalar(1.0, 1.0, 1.0) - mask, img2(r2));
		img2(r2) = img2(r2) + img2Cropped;
	}
}

//// Main function
int main(int argc, char** argv)
{
	// Load the cascade
	if (!face_cascade.load(face_cascade_name)) { printf("--(!)Error loading face cascade\n"); return -2; };
	if (!eyes_cascade.load(eyes_cascade_name)) { printf("--(!)Error loading eyes cascade\n"); return -3; };

	//// Read the images
	monkey = imread("../../Images/monkey1.jpg");
	man = imread("../../Images/man1.jpg");
	
	resize(monkey, monkey, man.size());			// Resize monkey to be the same size as man
	morphed = man.clone(); morphedMan = man.clone(); morphedMonkey = monkey.clone();
	morphedMonkeyWarp = monkey.clone();  morphedManWarp = man.clone(); manWarp = man.clone(); monkeyWarp = monkey.clone();
	//// Linear Blending
	namedWindow("Morphed", 1);
	imshow("Morphed", morphed);
	createTrackbar("Alpha", "Morphed", &a, max_elem, Blend);
	cout << "Press a key once done..." << endl;
	waitKey(0);

	//// Classifier - face detection
	face(morphedMan); face(morphedMonkey);

	//// Detecting Feature Points
	namedWindow("Select Feature Points on Man", 1);
	//set the callback function for mouse event
	setMouseCallback("Select Feature Points on Man", MouseClick, NULL);
	imshow("Select Feature Points on Man", man);
	cout << "Press a key once done..." << endl;
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

	namedWindow("Select Corresponding Feature Points on Monkey", 1);
	//set the callback function for mouse event
	setMouseCallback("Select Corresponding Feature Points on Monkey", MonkeyMouseClick, NULL);
	imshow("Select Corresponding Feature Points on Monkey", monkey);
	cout << "Press a key once done..." << endl;
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

	//// Weighted average point coordinates
	vector<Point2f> points;
	double alpha = 0.5;			//alpha controls the degree of morph
	for (int i = 0; i < manPoints.size(); i++)
	{
		float x, y;
		x = (1 - alpha) * manPoints[i].x + alpha * monkeyPoints[i].x;
		y = (1 - alpha) * manPoints[i].y + alpha * monkeyPoints[i].y;
		points.push_back(Point2f(x, y));
	}
	cout << "Weighted Average Points\n" << points << endl;

	//convert Mat to float data type
	monkey.convertTo(monkey, CV_32F);
	man.convertTo(man, CV_32F);
	morphed.convertTo(morphed, CV_32F);
	//man.convertTo(man, CV_32FC3, 1 / 255.0);
	//empty average image
	//Mat imgMorph = Mat::zeros(img1.size(), CV_32FC3);


	//// Delaunay Triangulation
	Scalar delaunay_color(255, 255, 255), points_color(0, 0, 255); // colors for drawing.
	bool animate = true;					// Turn on animation while drawing triangles
	
	// Triangualtion of Average Points
	Size size = man.size();
	//Rect rectMan(0, 0, size.width, size.height);
	Rect rect(0, 0, 600, 800);					// Rectangle to be used with Subdiv2D
	//Rect rect(Point(0, 0), Point(600, 800));
	//Rect rect(0, 0, man.cols, man.rows);		// Outer bounding box
	Subdiv2D subdiv(rect);						// Create initial subdivision
	Point2f fp;							// Point holder						
	Mat img(rect.size(), CV_8UC3);
	img = Scalar::all(0);
	// Insert points into subdiv
	for (int ip = 0; ip < points.size(); ip++)
	//for (vector<Point2f>::iterator ip = manPoints.begin(); ip != manPoints.end(); ip++)
	{
		fp = points[ip];						// 32f points
		if (points[ip].x >= 0 && points[ip].y >= 0)
		{
			subdiv.insert(fp);
			//subdivMan.insert(*ip);
			if (!animate)	//
			{
				Mat img_copy = morphedMan.clone();
				// Draw delaunay triangles
				draw_delaunay(img_copy, subdiv, delaunay_color, "Morphed");
				imshow("Delaunay Triangulation", img_copy);
				waitKey(10000);					// Wait 10s
			}
		}
	}
	// Draw delaunay triangles
	draw_delaunay(morphed, subdiv, delaunay_color, "Morphed");
	morphed.convertTo(morphed, CV_8UC3);
	// Draw points
	for (vector<Point2f>::iterator ipp = points.begin(); ipp != points.end(); ipp++)
	{
		circle(morphed, *ipp, 2, points_color, CV_FILLED, CV_AA, 0);
	}
	imshow("Delaunay Triangulation - Average", morphed);
	cout << "Waiting 10 s" << endl;
	waitKey(10000);

	// Triangulation - MAN
	Size sizeMan = man.size();
	//Rect rectMan(0, 0, sizeMan.width, sizeMan.height);
	Rect rectMan(0, 0, 600, 800);
	//Rect rectMan(0, 0, man.cols, man.rows);	// Outer bounding box
	Subdiv2D subdivMan(rectMan);			// Create initial subdivision
	Point2f fpMan;							// Point holder						
	//Mat img(rectMan.size(), CV_8UC3);
	//img = Scalar::all(0);	
	// Insert points into subdiv
	for (int ip = 0; ip < manPoints.size(); ip++)
	//for (vector<Point2f>::iterator ip = manPoints.begin(); ip != manPoints.end(); ip++)
	{
		fpMan = manPoints[ip];			// 32f points
		if (manPoints[ip].x > 0 && manPoints[ip].y > 0)
		{
			subdivMan.insert(fpMan);
			//subdivMan.insert(*ip);
			if (!animate)
			{
				Mat img_copy = man.clone();
				// Draw delaunay triangles
				draw_delaunay(img_copy, subdivMan, delaunay_color, "Man");
				imshow("Delaunay Triangulation", img_copy);
				waitKey(30000);
			}
		}
	}
	// Draw delaunay triangles
	draw_delaunay(man, subdivMan, delaunay_color, "Man");
	man.convertTo(man, CV_8UC3);
	// Draw points
	for (vector<Point2f>::iterator itt = manPoints.begin(); itt != manPoints.end(); itt++)
	{
		circle(man, *itt, 2, points_color, CV_FILLED, CV_AA, 0);
	}
	imshow("Delaunay Triangulation Man", man);
	cout << "Waiting 10 s" << endl;
	waitKey(10000);

	// Monkey Triangulation
	Rect rectMonkey(0, 0, 600, 800);
	//Rect rectMonkey(0, 0, monkey.cols, monkey.rows);	// Outer bounding box - Same as man
	Subdiv2D subdivMonkey(rectMonkey);		// Create initial subdivision
	Point2f fpMonkey;					// Point holder
	for (int im = 0; im < monkeyPoints.size(); im++)
	{
		fpMonkey = monkeyPoints[im];		// 32f points
		if (monkeyPoints[im].x > 0 && monkeyPoints[im].y >0)
		{
			subdivMonkey.insert(fpMonkey);
		}
	}
	// Draw delaunay triangles
	draw_delaunay(monkey, subdivMonkey, delaunay_color, "Monkey");
	monkey.convertTo(monkey, CV_8UC3);
	//vector<Vec6f> trianglesMonkey;
	//subdivMonkey.getTriangleList(trianglesMonkey);
	// Draw points
	for (vector<Point2f>::iterator imm = monkeyPoints.begin(); imm != monkeyPoints.end(); imm++)
	{
		circle(monkey, *imm, 2, points_color, CV_FILLED, CV_AA, 0);
	}
	imshow("Delaunay Triangulation Monkey", monkey);
	cout << "Waiting 10 s" << endl;
	waitKey(10000);



	/// Warping*************************************************
	// convert image to float
	manWarp.convertTo(manWarp, CV_32FC3, 1 / 255.0);
	monkeyWarp.convertTo(monkeyWarp, CV_32FC3, 1 / 255.0);
	morphedManWarp.convertTo(morphedManWarp, CV_32FC3, 1 / 255.0);

	// I/P - Mantri/Monkeytri
	// O/P - Morphedtri
	warpTriangle(manWarp, morphedManWarp, Mantri, Morphedtri);



//	for (int x; x < Mantri.size(); x++)
//	{
//		morphTriangle(man, monkey, morphed, Mantri[x], Monkeytri[x], Morphedtri[x], alpha);
//	}

	
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
		//for (int p; p < Mantri.size(); p++)
		//{
			//morphTriangle(man, monkey, morphed, Mantri[p], Monkeytri[p], Morphedtri[p], alpha);
		//}
	}




	//**********************
	// Display Result
	imshow("Morphed Face", morphed / 255.0);
	waitKey(0);


	namedWindow("Morphed", 1);
	imshow("Morphed", morphed);
	createTrackbar("Alpha", "Morphed", &a, max_elem, Blend);
	waitKey(0);
}
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/opencv.hpp>
#include "opencv2/features2d.hpp"
#include <iostream>
#include <stdio.h>
#include <vector>
#include <fstream>


//#define SHOW_DELAUNAY 
//#define SHOW_VORONOI 
//#define SAVE_POINTS
#define READ_POINTS

using namespace cv;
using namespace std;
//using namespace cv::xfeatures2d;

//Global variables
Mat monkey, man;
Mat morphedMonkey;
Mat clickMan,clickMonkey;

int a = 0; int const max_elem = 100;
vector<Point2f> manPoints;
vector<Point2f> monkeyPoints;
vector<Point2f> averagedPoints;
vector<Vec6f> manTri, monkeyTri, morphedTri;

bool manClick = true;
int imageFlag = 1;

double alpha = 0.5;			//alpha controls the degree of morph

// Draw a single point
static void draw_point( Mat& img, Point2f fp, Scalar color )
{
	circle( img, fp, 2, color, CV_FILLED, CV_AA, 0 );
}

/// Mouse clicks to select points
void mouseClick(int event, int x, int y, int flags, void* userdata)
{
	//char coordinate[16];
	if (event == EVENT_LBUTTONDOWN)
	{
		cout << x << " " << y << endl;
		if(manClick){
			manPoints.push_back(Point2f(x, y));
			draw_point(clickMan, Point2f(x, y), Scalar(0,0,255));
			imshow("Select Feature Points on Man", clickMan);
		}
		else {
			monkeyPoints.push_back(Point2f(x, y));
			draw_point(clickMonkey, Point2f(x, y), Scalar(0,0,255));
			imshow("Select Corresponding Feature Points on Monkey", clickMonkey);
		}
	}
}

void getFeaturePointsMan(){
#ifndef READ_POINTS
	namedWindow("Select Feature Points on Man", 1);
	//set the callback function for mouse event
	setMouseCallback("Select Feature Points on Man", mouseClick, NULL);
	imshow("Select Feature Points on Man", clickMan);
	cout << "Press a key once done..." << endl;
	waitKey(0);

#ifdef SAVE_POINTS
	//save coordinates to txt file
	ofstream file("../../Images/ManFeaturePoints.txt");			
	vector<Point2f>::iterator it = manPoints.begin();
	for (; it != manPoints.end(); ++it)
	{
		file << it->x << " " << it->y << endl;
	}
	file.close();
	manClick = false; // change the flag for monkey mouseclick
	cout << "Man Points\n" << manPoints << endl;
#endif

#endif
#ifdef READ_POINTS
	ifstream ifs("../../Images/ManFeaturePoints.txt");
	double x, y;
	while (ifs >> x >> y ){
		manPoints.push_back(Point2f(x, y));
	}

#endif

}

void getFeaturePointsMonkey(){
#ifndef READ_POINTS
	namedWindow("Select Corresponding Feature Points on Monkey", 1);
	//set the callback function for mouse event
	setMouseCallback("Select Corresponding Feature Points on Monkey", mouseClick, NULL);
	imshow("Select Corresponding Feature Points on Monkey", monkey);
	cout << "Press a key once done..." << endl;
	waitKey(0);

#ifdef SAVE_POINTS
	//save coordinates to txt file
	ofstream file2("../../Images/MonkeyFeaturePoints.txt");			
	vector<Point2f>::iterator mt = monkeyPoints.begin();
	for (; mt != monkeyPoints.end(); ++mt)
	{
		file2 << mt->x << " " << mt->y << endl;	
	}
	file2.close();
	cout << "Monkey Points\n" << monkeyPoints << endl;
#endif
#endif
#ifdef READ_POINTS
	ifstream ifs("../../Images/MonkeyFeaturePoints.txt");
	double x, y;
	while (ifs >> x >> y ){
		monkeyPoints.push_back(Point2f(x, y));
	}

#endif

}

void averageFeaturePoints(){

	
	for (int i = 0; i < manPoints.size(); i++)
	{
		float x, y;
		x = (1 - alpha) * manPoints[i].x + alpha * monkeyPoints[i].x;
		y = (1 - alpha) * manPoints[i].y + alpha * monkeyPoints[i].y;
		averagedPoints.push_back(Point2f(x, y));
	}
	//cout << "Weighted Average Points\n" << averagedPoints << endl;

}


// Draw delaunay triangles
static void draw_delaunay( Mat& img, Subdiv2D& subdiv, Scalar delaunay_color )
{

	vector<Vec6f> triangleList;
	subdiv.getTriangleList(triangleList);

	vector<Point> pt(3);
	Size size = img.size();
	Rect rect(0,0, size.width, size.height);

	for( size_t i = 0; i < triangleList.size(); i++ )
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));

		// Draw rectangles completely inside the image.
		if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
		{
			line(img, pt[0], pt[1], delaunay_color, 1, CV_AA, 0);
			line(img, pt[1], pt[2], delaunay_color, 1, CV_AA, 0);
			line(img, pt[2], pt[0], delaunay_color, 1, CV_AA, 0);


		}
	}
}

//Draw voronoi diagrams
static void draw_voronoi( Mat& img, Subdiv2D& subdiv )
{
	vector<vector<Point2f> > facets;
	vector<Point2f> centers;
	subdiv.getVoronoiFacetList(vector<int>(), facets, centers);

	vector<Point> ifacet;
	vector<vector<Point> > ifacets(1);

	for( size_t i = 0; i < facets.size(); i++ )
	{
		ifacet.resize(facets[i].size());
		for( size_t j = 0; j < facets[i].size(); j++ )
			ifacet[j] = facets[i][j];

		Scalar color;
		color[0] = rand() & 255;
		color[1] = rand() & 255;
		color[2] = rand() & 255;
		fillConvexPoly(img, ifacet, color, 8, 0);

		ifacets[0] = ifacet;
		polylines(img, ifacets, true, Scalar(), 1, CV_AA, 0);
		circle(img, centers[i], 3, Scalar(), CV_FILLED, CV_AA, 0);
	}
}

void drawDelaunayMan(){


	// Rectangle to be used with Subdiv2D
	Size size = man.size();
	Rect rect(0, 0, size.width, size.height);

	// Create an instance of Subdiv2D
	Subdiv2D subdivMan(rect);

	for( vector<Point2f>::iterator it = manPoints.begin(); it != manPoints.end(); it++)
	{
		subdivMan.insert(*it);
	}

	// Define colors for drawing.
	Scalar delaunay_color(255,255,255), points_color(0, 0, 255);
	string manWindow = "Traingulation_man";
	imshow(manWindow,1);

	Mat img_copy_man = man.clone();
	draw_delaunay( img_copy_man, subdivMan, delaunay_color );
	for( vector<Point2f>::iterator it = manPoints.begin(); it != manPoints.end(); it++)
	{
		draw_point(img_copy_man, *it, points_color);
	}
	imshow(manWindow, img_copy_man);

#ifdef SHOW_VORONOI
	cvWaitKey(0);
	// Allocate space for voronoi Diagram
	Mat man_voronoi = Mat::zeros(man.rows, man.cols, CV_8UC3);

	// Draw voronoi diagram
	draw_voronoi( man_voronoi, subdivMan );
	imshow(manWindow, man_voronoi);
	cvWaitKey(0);
#endif
}

void drawDelaunayMonkey(){

	Scalar delaunay_color(255,255,255), points_color(0, 0, 255);

	// Rectangle to be used with Subdiv2D
	Size size = man.size();
	Rect rect(0, 0, size.width, size.height);

	Subdiv2D subdivMonkey(rect);
	string monkeyWindow = "Traingulation_monkey";
	imshow(monkeyWindow,1);

	for( vector<Point2f>::iterator it = monkeyPoints.begin(); it != monkeyPoints.end(); it++)
	{
		subdivMonkey.insert(*it);
	}
	Mat img_copy_monkey = monkey.clone();
	// Draw delaunay triangles

	draw_delaunay( img_copy_monkey, subdivMonkey, delaunay_color );
	for( vector<Point2f>::iterator it = monkeyPoints.begin(); it != monkeyPoints.end(); it++)
	{
		draw_point(img_copy_monkey, *it, points_color);
	}
	imshow(monkeyWindow, img_copy_monkey);
#ifdef SHOW_VORONOI
	cvWaitKey(0);

	// Allocate space for voronoi Diagram
	Mat monkey_voronoi = Mat::zeros(man.rows, man.cols, CV_8UC3);

	// Draw voronoi diagram
	draw_voronoi( monkey_voronoi, subdivMonkey );
	imshow(monkeyWindow, monkey_voronoi);

	cvWaitKey(0);
#endif
}


void getMonkeyTriangles(){

	Size size = man.size();
	Rect rect(0, 0, size.width, size.height);
	vector<Vec6f> triangleList;
	vector<Point2f> pt(3);

	Subdiv2D subdivMonkey(rect);

	for( vector<Point2f>::iterator it = monkeyPoints.begin(); it != monkeyPoints.end(); it++)
	{
		subdivMonkey.insert(*it);
	}

	subdivMonkey.getTriangleList(triangleList);

	for( size_t i = 0; i < triangleList.size(); i++ )
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));

		// Draw rectangles completely inside the image.
		if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
		{
			monkeyTri.push_back(t);

		}
	}
	ofstream file2("../../Images/MonkeyTriangles.txt");			
	vector<Vec6f>::iterator mt = monkeyTri.begin();
	for (; mt != monkeyTri.end(); ++mt)
	{
		file2 << mt[0] << endl;


	}
	file2.close();

}

void getManTriangles(){

	Size size = man.size();
	Rect rect(0, 0, size.width, size.height);
	vector<Vec6f> triangleList;
	vector<Point2f> pt(3);

	Subdiv2D subdivMan(rect);
	for( vector<Point2f>::iterator it = manPoints.begin(); it != manPoints.end(); it++)
	{
		subdivMan.insert(*it);

	}
	subdivMan.getTriangleList(triangleList);

	for( size_t i = 0; i < triangleList.size(); i++ )
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));

		// Draw rectangles completely inside the image.
		if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
		{
			manTri.push_back(t);

		}
	}

	ofstream file2("../../Images/ManTriangles.txt");			
	vector<Vec6f>::iterator mt = manTri.begin();
	for (; mt != manTri.end(); ++mt)
	{
		file2 << mt[0]<< endl;	
	}
	file2.close();
}

void getAveragedTriangles(){

	Size size = man.size();
	Rect rect(0, 0, size.width, size.height);
	vector<Vec6f> triangleList;
	vector<Point2f> pt(3);

	Subdiv2D subdivMorph(rect);

	for( vector<Point2f>::iterator it = averagedPoints.begin(); it != averagedPoints.end(); it++)
	{
		subdivMorph.insert(*it);
	}

	subdivMorph.getTriangleList(triangleList);
	for( size_t i = 0; i < triangleList.size(); i++ )
	{
		Vec6f t = triangleList[i];
		pt[0] = Point(cvRound(t[0]), cvRound(t[1]));
		pt[1] = Point(cvRound(t[2]), cvRound(t[3]));
		pt[2] = Point(cvRound(t[4]), cvRound(t[5]));

		// Draw rectangles completely inside the image.
		if ( rect.contains(pt[0]) && rect.contains(pt[1]) && rect.contains(pt[2]))
		{
			morphedTri.push_back(t);

		}
	}

}

// Apply affine transform calculated using srcTri and dstTri to src
void applyAffineTransform(Mat &warpImage, Mat &src, vector<Point2f> &srcTri, vector<Point2f> &dstTri)
{

	// Given a pair of triangles, find the affine transform.
	Mat warpMat = getAffineTransform( srcTri, dstTri );

	// Apply the Affine Transform just found to the src image
	warpAffine( src, warpImage, warpMat, warpImage.size(), INTER_LINEAR, BORDER_REFLECT_101);
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
	for(int i = 0; i < 3; i++)
	{
		tRect.push_back( Point2f( t[i].x - r.x, t[i].y -  r.y) );
		tRectInt.push_back( Point(t[i].x - r.x, t[i].y - r.y) ); // for fillConvexPoly

		t1Rect.push_back( Point2f( t1[i].x - r1.x, t1[i].y -  r1.y) );
		t2Rect.push_back( Point2f( t2[i].x - r2.x, t2[i].y - r2.y) );
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
	//Mat imgRect(warpImage1.rows, warpImage1.cols, CV_32FC3);
	Mat imgRect = (1.0 - alpha) * warpImage1 + alpha * warpImage2;
	imgRect.convertTo(imgRect,CV_32FC3);

	// Copy triangular region of the rectangular patch to the output image
	multiply(imgRect, mask,imgRect);
	img.convertTo(img,CV_32FC3);
	multiply(img(r),Scalar(1.0,1.0,1.0) - mask, img(r));
	img(r) = img(r) + imgRect;
	//img.convertTo(img,CV_8UC3);    
}

void applyWarp(){
	
	map <pair<int,int>,int> pointsMap;

	for(int i =0 ; i < manPoints.size(); i++){
		pointsMap.insert(pair<pair<int,int>,int>(pair<int,int>(manPoints[i].x,manPoints[i].y),i));	
	}
	
	Size size = man.size();
	Rect rect(0, 0, size.width, size.height);
	for( size_t i = 0; i < manTri.size(); i++ )
	{

		Vec6f manTri_t = manTri[i];
		vector<Point2f> t1, t2, t;
		int i1,i2,i3;

		t1.push_back(Point2f((manTri_t[0]), (manTri_t[1])));
		t1.push_back(Point2f((manTri_t[2]), (manTri_t[3])));
		t1.push_back(Point2f((manTri_t[4]), (manTri_t[5])));

		i1 = pointsMap.find(pair<int,int>(manTri_t[0],manTri_t[1]))->second;
		i2 = pointsMap.find(pair<int,int>(manTri_t[2],manTri_t[3]))->second;
		i3 = pointsMap.find(pair<int,int>(manTri_t[4],manTri_t[5]))->second;

		t2.push_back(monkeyPoints[i1]);
		t2.push_back(monkeyPoints[i2]);
		t2.push_back(monkeyPoints[i3]);

		t.push_back(averagedPoints[i1]);
		t.push_back(averagedPoints[i2]);
		t.push_back(averagedPoints[i3]);

		morphTriangle(man,monkey,morphedMonkey,t1,t2,t,alpha);

	}


}
int main(int argc, char** argv)
{
	monkey = imread("../../Images/monkey1.jpg");
	man = imread("../../Images/man1.jpg");
	resize(monkey, monkey, man.size());			// Resize monkey to be the same size as man

	morphedMonkey = Mat::zeros(man.size(), CV_32FC3);
	clickMan = man.clone();
	clickMonkey = monkey.clone();

	alpha = 0.25;	

	//getting Feature Points from User
	getFeaturePointsMan();
	getFeaturePointsMonkey();
	averageFeaturePoints();

	cvDestroyAllWindows();

#ifdef SHOW_DELAUNAY
	// Delaunay Triangulation
	drawDelaunayMan();
	drawDelaunayMonkey();
#endif	

	getManTriangles();
	getMonkeyTriangles();
	getAveragedTriangles();

	applyWarp();

	imshow("Morphed",morphedMonkey/255.0);
	cvWaitKey(0);
}
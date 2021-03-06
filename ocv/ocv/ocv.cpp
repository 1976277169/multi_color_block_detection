#include "stdafx.h"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <sstream>
#include <string>
#include <iostream>
#include <vector>
#include "Block.h"

using namespace std;
using namespace cv;

Mat img1;
Mat threshold;
Mat hsv;
//C lH  ,hH  - lS  ,hS  -lV  ,hV
//R 0   ,4   - 65  ,254 -67  ,252
//Y 23  ,46  - 123 ,254 -149 ,252
//B 82  ,255 - 99  ,255 -126 ,255

VideoCapture capture(0);

int iLowH = 82;
int iHighH = 255;

int iLowS = 99;
int iHighS = 255;

int iLowV = 126;
int iHighV = 255;

bool trackObjects = true;
bool useMorphOps = true;

int FRAME_WIDTH = 640;
int FRAME_HEIGHT = 480;
int MIN_OBJECT_AREA = 20 * 20;
int MAX_OBJECT_AREA = 204800;
int MAX_NUM_OBJECTS = 50;

void createTrackbars() {
	createTrackbar("LowH", "Control", &iLowH, 255); //Hue (0 - 179)
	createTrackbar("HighH", "Control", &iHighH, 255);

	createTrackbar("LowS", "Control", &iLowS, 255); //Saturation (0 - 255)
	createTrackbar("HighS", "Control", &iHighS, 255);

	createTrackbar("LowV", "Control", &iLowV, 255);//Value (0 - 255)
	createTrackbar("HighV", "Control", &iHighV, 255);
}

string intToString(int number) {


	std::stringstream ss;
	ss << number;
	return ss.str();
}

void drawObject(vector <Block> Blocks, Mat &frame) {
	for (int i = 0; i < Blocks.size(); i++) {

		circle(frame, Point(Blocks.at(i).getxPos(), Blocks.at(i).getyPos()), 20, Blocks.at(i).getcolor(), 2);

		putText(frame, intToString(Blocks.at(i).getxPos()) + "," + intToString(Blocks.at(i).getyPos()), Point(Blocks.at(i).getxPos(), Blocks.at(i).getyPos() + 30), 1, 1, Blocks.at(i).getcolor(), 2);
		putText(frame, Blocks.at(i).getname(), Point(Blocks.at(i).getxPos(), Blocks.at(i).getyPos() - 20), 1, 1, Blocks.at(i).getcolor(), 2);
	}
}
void morphOps(Mat &thresh) {

	//create structuring element that will be used to "dilate" and "erode" image.
	//the element chosen here is a 3px by 3px rectangle

	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	//dilate with larger element so make sure object is nicely visible
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(8, 8));

	erode(thresh, thresh, erodeElement);
	erode(thresh, thresh, erodeElement);


	dilate(thresh, thresh, dilateElement);
	dilate(thresh, thresh, dilateElement);



}
void trackFilteredObject(Mat threshold, Mat &cameraFeed) {

	Mat temp;
	vector <Block> Blocks;

	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
					Block B;
					B.setxPos(moment.m10 / area);
					B.setyPos(moment.m01 / area);
					Blocks.push_back(B);
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;
			}
			//let user know you found an object
			if (objectFound == true) {
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//draw object location on screen
				drawObject(Blocks, cameraFeed);
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}
void trackFilteredObject(Block Bl, Mat threshold, Mat &cameraFeed) {

	Mat temp;
	vector <Block> Blocks;

	threshold.copyTo(temp);
	//these two vectors needed for output of findContours
	vector< vector<Point> > contours;
	vector<Vec4i> hierarchy;
	//find contours of filtered image using openCV findContours function
	findContours(temp, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//use moments method to find our filtered object
	double refArea = 0;
	bool objectFound = false;
	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();
		//if number of objects greater than MAX_NUM_OBJECTS we have a noisy filter
		if (numObjects<MAX_NUM_OBJECTS) {
			for (int index = 0; index >= 0; index = hierarchy[index][0]) {

				Moments moment = moments((cv::Mat)contours[index]);
				double area = moment.m00;

				//if the area is less than 20 px by 20px then it is probably just noise
				//if the area is the same as the 3/2 of the image size, probably just a bad filter
				//we only want the object with the largest area so we safe a reference area each
				//iteration and compare it to the area in the next iteration.
				if (area>MIN_OBJECT_AREA && area<MAX_OBJECT_AREA && area>refArea) {
					Block B;
					B.setxPos(moment.m10 / area);
					B.setyPos(moment.m01 / area);
					B.setname(Bl.getname());
					B.setcolor(Bl.getcolor());
					Blocks.push_back(B);
					objectFound = true;
					refArea = area;
				}
				else objectFound = false;
			}
			//let user know you found an object
			if (objectFound == true) {
				putText(cameraFeed, "Tracking Object", Point(0, 50), 2, 1, Scalar(0, 255, 0), 2);
				//draw object location on screen
				drawObject(Blocks, cameraFeed);
			}

		}
		else putText(cameraFeed, "TOO MUCH NOISE! ADJUST FILTER", Point(0, 50), 1, 2, Scalar(0, 0, 255), 2);
	}
}

int main(int argc, char* argv[])
{

	namedWindow("Control", CV_WINDOW_AUTOSIZE);
	int x = 0, y = 0;

	Block blue("Blue"), red("Red"), yellow("Yellow");

	blue.setHSVmin(Scalar(82, 99, 126));
	blue.setHSVmax(Scalar(255, 255, 255));
	blue.setcolor(Scalar(255, 0, 0));

	red.setHSVmin(Scalar(0, 75, 91));
	red.setHSVmax(Scalar(14, 255, 255));
	red.setcolor(Scalar(0, 0, 255));

	yellow.setHSVmin(Scalar(23, 123, 149));
	yellow.setHSVmax(Scalar(46, 254, 252));
	yellow.setcolor(Scalar(0, 255, 255));

	while (1) {
		capture.read(img1);
		flip(img1, img1, 1);

		cvtColor(img1, hsv, COLOR_BGR2HSV);
		inRange(hsv, blue.getHSVmin(), blue.getHSVmax(), hsv);
		morphOps(hsv);
		trackFilteredObject(blue, hsv, img1);
		
		cvtColor(img1, hsv, COLOR_BGR2HSV);
		inRange(hsv, red.getHSVmin(), red.getHSVmax(), hsv);
		morphOps(hsv);
		trackFilteredObject(red, hsv, img1);

		cvtColor(img1, hsv, COLOR_BGR2HSV);
		inRange(hsv, yellow.getHSVmin(), yellow.getHSVmax(), hsv);
		morphOps(hsv);
		trackFilteredObject(yellow, hsv, img1);

		imshow("camera", img1);
		//imshow("hsv", hsv);
		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}
	return 0;
}

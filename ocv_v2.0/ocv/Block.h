#pragma once
#include<string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

using namespace std;
using namespace cv;

class Block
{
public:
	Block();
	~Block();
	Block(string name);

	int getxPos();
	void setxPos(int x);

	int getyPos();
	void setyPos(int x);

	Scalar getHSVmin();
	Scalar getHSVmax();

	void setHSVmin(Scalar min);
	void setHSVmax(Scalar max);

	string getname();
	void setname(string name);

	Scalar getcolor();
	void setcolor(Scalar color);

private:
	int xPos, yPos;
	string type;
	Scalar HSVmin, HSVmax;
	Scalar Color;
	
};


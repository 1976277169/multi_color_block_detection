#include "stdafx.h"
#include "Block.h"


Block::Block()
{
}

Block::Block(string name)
{
	type = name;
}

Block::~Block()
{
}

int Block::getxPos()
{
	return xPos;
}

void Block::setxPos(int x)
{
	Block::xPos = x;

}
int Block::getyPos()
{
	return yPos;
}

void Block::setyPos(int y)
{
	Block::yPos = y;

}

Scalar Block::getHSVmin()
{
	return HSVmin;
}
Scalar Block::getHSVmax()
{
	return HSVmax;
}


void Block::setHSVmax(Scalar max)
{
	HSVmax = max;
}

void Block::setHSVmin(Scalar min)
{
	HSVmin = min;
}

string Block::getname()
{
	return type;
}
void Block::setname(string name)
{
	type = name;
}

Scalar Block::getcolor()
{
	return Color;
}
void Block::setcolor(Scalar color)
{
	Color = color;
}
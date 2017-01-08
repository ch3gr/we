#pragma once
#include "ofMain.h"


class person
{
	int id;
	int x;
	int y;
	ofImage face;

public:
	person();
	person(int _id, int _x, int _y);
	person(int _id, ofImage _face, int _x, int _y);


	//void person::track();
	//void person::train();

	void person::setFace( ofImage _face);
	void person::setPos(int _x, int _y);
	void person::update();
	void person::draw();
	void person::info();
};


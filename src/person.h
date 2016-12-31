#pragma once
#include "ofMain.h"


class person
{

	int x;
	int y;

public:
	person();
	person(int _x, int _y);


	//void person::track();
	//void person::train();

	void person::snap();
	void person::draw();
};


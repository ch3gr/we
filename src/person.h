#pragma once
#include "ofApp.h"



class person
{

	int x;
	int y;
	ofImage a;

public:
	person();
	person(int _x, int _y);
	~person();

	void person::draw();
};


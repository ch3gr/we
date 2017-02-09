#pragma once
#include "ofMain.h"
#include "ofxCv.h"

using namespace cv;

class person
{
public:
	int id;
	int x;
	int y;
	ofImage face;
	vector<ofImage> snapshots;
	vector<Mat> snapshotsCV;
	vector<ofImage> ofSnapshotsCV;


	person();
	person(int _id, int _x, int _y);
	person(int _id, ofImage _face);
	person(int _id, ofImage _face, vector<ofImage> _snapshots);
	person(int _id, ofImage _face, int _x, int _y);


	//void person::track();
	//void person::train();

	void person::setFace( ofImage _face);
	void person::setPos(int _x, int _y);
	void person::update();
	void person::draw();
	void person::drawDebug();
	void person::info();
};


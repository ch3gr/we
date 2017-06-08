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
	int f;
	float nextTime;
	ofImage face;
	vector<ofImage> frames;
	vector<ofImage> snapshots;
	vector<Mat> snapshotsCV;
	vector<ofImage> ofSnapshotsCV;
	vector<float> scale;
		
	person(int _id, vector<ofImage> & _frames, vector<ofImage> & _snapshots);
	
	

	void person::setPos(int _x, int _y);
	void person::draw();
	void person::drawDebug();
	void person::info();
};


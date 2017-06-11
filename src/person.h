#pragma once
#include "ofMain.h"
#include "ofxCv.h"


using namespace cv;

class person
{
public:
	int id;

	int iFrame;
	float nextTime;
	ofImage face;
	vector<ofImage> frames;
	vector<ofImage> snapshots;
	vector<Mat> snapshotsCV;
	vector<ofImage> ofSnapshotsCV;
	float scale;
	float rotate;
	ofVec2f translate;
		
	person(int _id, vector<ofImage> & _frames, vector<ofImage> & _snapshots);
	
	

	void person::setPos(float _x, float _y);
	void person::draw();
	void person::drawDebug();
	void person::info();
};


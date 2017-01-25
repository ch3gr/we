#pragma once
#include "ofMain.h"
#include "ofxCv.h"


class candidate : public ofxCv::RectFollower
{
public:
	ofRectangle faceBounds;
	ofRectangle faceBoundsOld;
	
	vector<ofImage> snapshots;

	int activeTimer ;

	float birthTime;

	bool active;
	bool trigger;
	bool captured;

	candidate();
	~candidate();

	void candidate::info();


	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
	bool isSnapshot();
	void takeSnapshot(ofImage snapshot);
};


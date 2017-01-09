#pragma once
#include "ofMain.h"
#include "ofxCv.h"


class candidate : public ofxCv::RectFollower
{
public:
	ofColor color;
	ofRectangle faceBounds;
	float startedDying;
	float dyingTime;


	candidate();
	~candidate();

	void candidate::info();

	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
};


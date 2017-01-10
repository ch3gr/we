#pragma once
#include "ofMain.h"
#include "ofxCv.h"


class candidate : public ofxCv::RectFollower
{
public:
	ofRectangle faceBounds;
	
	float rememberPeriod ;
	float evaluatePeriod ;

	float birthTime;
	float lostTime;

	bool trigger;
	bool captured;

	candidate();
	~candidate();

	void candidate::info();

	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
};


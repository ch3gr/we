#pragma once
#include "ofMain.h"
#include "ofxCv.h"


class candidate : public ofxCv::RectFollower
{
public:
	ofRectangle faceBounds;
	ofVec2f pos;
	ofVec2f pos0;
	ofVec2f vel;

	float rememberPeriod ;
	float evaluatePeriod ;

	float birthTime;
	float lostTime;

	bool tracking;
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


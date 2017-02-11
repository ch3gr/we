#pragma once
#include "ofMain.h"
#include "ofxCv.h"

class candidate : public ofxCv::RectFollower
{
public:
	ofRectangle faceBounds;
	ofRectangle faceBoundsOld;
	ofVec2f vel;
	
	vector<ofImage> snapshots;
	cv::Mat cv_evidence;
	bool evidenceIsSet;
	


	float birthTime;
	float lastSnapshotTime;
	float snapshotInterval;
	int snapshotSum;


	bool active;
	bool trigger;
	bool ignore;

	int lastMatch;
	double lastConfidence;

	candidate();
	~candidate();

	void candidate::info();


	void setup(const cv::Rect& track);
	void update(const cv::Rect& track);
	void kill();
	void draw();
	bool isPhotoTime();
	void takeSnapshot(ofImage snapshot);
};


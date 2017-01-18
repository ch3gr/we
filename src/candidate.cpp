#include "candidate.h"

using namespace ofxCv;
using namespace cv;

void candidate::setup(const cv::Rect& track) {
	rememberPeriod = 2;
	evaluatePeriod = 2;
	birthTime = ofGetElapsedTimef();
	trigger = false;
	captured = false;
	faceBounds = toOf(track);
	cout << "New Label : " << label << endl;
}

void candidate::update(const cv::Rect& track) {
	faceBounds = toOf(track);
	lostTime = -1;

	if (trigger) {
		trigger = false;
		captured = true;
	}
	else if (!captured && ofGetElapsedTimef() > birthTime + evaluatePeriod)
		trigger = true;
	
}

void candidate::kill() {
	if (lostTime == -1)
		lostTime = ofGetElapsedTimef();
	else if (ofGetElapsedTimef() - lostTime > rememberPeriod)
		dead = true;
}

void candidate::draw() {
	ofPushStyle();
	ofColor c = ofColor::blue;
	if (captured)
		c = ofColor::green;
	

	float fade = 0;
	if (lostTime != -1) {
		fade = ofMap(ofGetElapsedTimef(), lostTime, lostTime+rememberPeriod, 1, 0, true);
		c.a = fade * 255;
	}
	if (trigger) {
		ofFill();
		ofSetColor(ofColor::white);
	}
	else {
		ofSetColor(c);
		ofNoFill();
		ofSetLineWidth(2);
	}
	ofDrawRectangle(faceBounds);
	ofDrawBitmapString(label, faceBounds.x+5, faceBounds.y + 20);
	
	ofPopStyle();
}


candidate::candidate()
{
}


candidate::~candidate()
{
}



void candidate::info() {
	cout << "I am a candidate" << endl;
}


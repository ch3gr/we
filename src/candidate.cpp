#include "candidate.h"

using namespace ofxCv;
using namespace cv;

void candidate::setup(const cv::Rect& track) {
	rememberPeriod = 2;
	evaluatePeriod = 2;
	lostTime = -1;
	birthTime = ofGetElapsedTimef();

	trigger = false;
	captured = false;
	tracking = true;

	faceBounds = toOf(track);
	pos = pos0 = faceBounds.getCenter();
	vel = ofVec2f(0, 0);

	cout << "New Candidate : " << label << endl;
}

void candidate::update(const cv::Rect& track) {
	tracking = true;
	lostTime = -1;

	faceBounds = toOf(track);
	pos = faceBounds.getCenter();
	vel = pos - pos0;
	pos0 = pos;

	if (trigger) {
		trigger = false;
		captured = true;
	}
	else if (!captured && ofGetElapsedTimef() > birthTime + evaluatePeriod)
		trigger = true;
	
}


void candidate::kill() {
	if (lostTime == -1){
		lostTime = ofGetElapsedTimef();
		tracking = false;
		birthTime = ofGetElapsedTimef();
	}
	else if (ofGetElapsedTimef() > lostTime + rememberPeriod)
		dead = true;
}


void candidate::draw() {
	ofPushStyle();
	ofColor c = ofColor::blue;
	if (captured)
		c = ofColor::green;

	if( tracking )
		ofSetLineWidth(3);
	else
		ofSetLineWidth(1);

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
		
	}
	ofDrawRectangle(faceBounds);
	ofDrawBitmapString(label, faceBounds.x+5, faceBounds.y + 20);
	ofPopStyle();

	ofPushStyle();
	ofSetColor(ofColor::lightGrey);
	ofDrawLine(pos, pos + vel);
	ofSetColor(ofColor::darkGrey);
	ofDrawCircle(pos, 3);
	ofPopStyle();

	tracking = false;
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


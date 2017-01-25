#include "candidate.h"

using namespace ofxCv;
using namespace cv;

void candidate::setup(const cv::Rect& track) {

	activeTimer = -10;
	birthTime = ofGetElapsedTimef();
	active = false;
	trigger = false;
	captured = false;
	faceBounds = toOf(track);
	cout << "New Label : " << label << endl;
}

void candidate::update(const cv::Rect& track) {
	faceBounds = toOf(track);

	// we assume that if the track bound changes, we still see an actual face
	if (faceBounds.getCenter().x != faceBoundsOld.getCenter().x ||
		faceBounds.getCenter().y != faceBoundsOld.getCenter().y ||
		faceBounds.getWidth() != faceBoundsOld.getWidth() ||
		faceBounds.getHeight() != faceBoundsOld.getHeight()) {

		active = true;
		activeTimer += 1;
	}
	else
		active = false;

	faceBoundsOld = faceBounds;


	lostTime = -1;

	if (trigger) {
		trigger = false;
		captured = true;
	}
	else if (!captured && activeTimer >= 0)
		trigger = true;
	
}

void candidate::kill() {
	dead = true;
}

void candidate::draw() {
	ofPushStyle();
	ofColor c = ofColor::white;
	if (captured)
		c = ofColor::green;
	


	if (trigger) {
		ofFill();
		ofSetColor(ofColor::white);
	}
	else if (active) {
		ofNoFill();
		ofSetLineWidth(2);
		ofSetColor(ofColor::red);
	}
	else {
		ofSetColor(c);
		ofNoFill();
		ofSetLineWidth(1);
		ofSetColor(ofColor::grey);
	}


	ofDrawRectangle(faceBounds);
	ofDrawBitmapString(label, faceBounds.x+5, faceBounds.y + 20);
	ofDrawBitmapString(activeTimer, faceBounds.x + 5, faceBounds.y + 30);
	
	ofPopStyle();
}


candidate::candidate()
{
}


candidate::~candidate()
{
}



void candidate::info() {
	cout << "Am I active : " << active << endl;
}


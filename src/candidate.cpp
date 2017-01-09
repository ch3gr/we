#include "candidate.h"

using namespace ofxCv;
using namespace cv;

void candidate::setup(const cv::Rect& track) {
	color.set(ofColor::orange);
	dyingTime = 1;
	faceBounds = toOf(track);
}

void candidate::update(const cv::Rect& track) {
	faceBounds = toOf(track);
}

void candidate::kill() {
	cout << "Die" << endl;

	float curTime = ofGetElapsedTimef();
	if (startedDying == 0) {
		startedDying = curTime;
	}
	else if (curTime - startedDying > dyingTime) {
		dead = true;
	}

}

void candidate::draw() {
	ofPushStyle();
	ofColor c = ofColor::blue;
	float fade = 0;
	if (startedDying) {
		fade = ofMap(ofGetElapsedTimef() - startedDying, 0, dyingTime, 0, 1, true);
		ofColor a = ofColor::green;
		ofColor b = ofColor::red;
		c = a*(1-fade) + b*(fade);

	}
	ofSetColor(c);
	ofNoFill();
	faceBounds.scaleFromCenter(0.9, 0.9);
	ofDrawRectangle(faceBounds);
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
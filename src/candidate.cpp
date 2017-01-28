#include "candidate.h"

using namespace ofxCv;
using namespace cv;

void candidate::setup(const cv::Rect& track) {

	activeTimer = -30;
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


	if (trigger) {
		ofFill();
		ofSetColor(ofColor::white);
	}
	else if (active && !captured) {
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(ofColor::red);
	}
	else if (captured) {
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(ofColor::green);
	}

	if( !active) {
		ofSetColor(c);
		ofNoFill();
		ofSetLineWidth(1);
		ofSetColor(ofColor::grey);
	}

	if( isSnapshot() )
		ofSetLineWidth(10);

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


bool candidate::isSnapshot() {
	int intervals = 3;
	bool out = active;

	if ( activeTimer% intervals != 0) // Set every how many frames to take a snapshot
		out = false;

	if (activeTimer > 0)
		out = false;

	return out;
}

void candidate::takeSnapshot(ofImage snapshot)
{
	snapshot.crop(faceBounds.x, faceBounds.y, faceBounds.width, faceBounds.height );
	snapshot.resize(75, 75);
	snapshots.push_back(snapshot);
}
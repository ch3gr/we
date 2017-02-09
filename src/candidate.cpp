#include "candidate.h"

using namespace ofxCv;
using namespace cv;

void candidate::setup(const cv::Rect& track) {

	snapshotIntervals = 4;
	activeTimer = -snapshotIntervals * 10 -1;
	birthTime = ofGetElapsedTimef();
	active = false;
	trigger = false;
	captured = false;
	exist = false;
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




	if (trigger)
		trigger = false;
		
	else if (!exist && !captured && activeTimer >= 0)
		trigger = true;
	
	if (exist)
	{
		snapshots.clear();
		captured = true;
	}
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
	else if (exist) {
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(ofColor::red);
	}
	else if (active && !captured) {
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(ofColor::green);
	}
	else if (captured) {
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(ofColor::blue);
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
	bool out = active;

	if (exist)
		return false;

	if ( activeTimer% snapshotIntervals != 0) // Set every how many frames to take a snapshot
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

	// <----------- EDW
	//set the mat to ID against
	Mat snapToIdCvColor = ofxCv::toCv(snapshot);
	cvtColor(snapToIdCvColor, snapToIdCv, CV_RGB2GRAY);
}



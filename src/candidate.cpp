#include "candidate.h"
#include "ofApp.h"

using namespace ofxCv;
using namespace cv;

void candidate::setup(const cv::Rect& track) {

	birthTime = ofGetElapsedTimef();
	lastSnapshotTime = birthTime;
	snapshotInterval = 0.001;
	snapshotSum = 4;



	active = false;
	trigger = false;
	ignore = false;
	evidenceIsSet = false;

	lastMatch = -1;
	lastConfidence = -1;

	faceBounds = toOf(track);
	vel = ofVec2f(0, 0);

}

void candidate::update(const cv::Rect& track) {
	faceBounds = toOf(track);

	// we assume that if the track bound changes, we still see an actual face
	if (faceBounds.getCenter().x != faceBoundsOld.getCenter().x ||
		faceBounds.getCenter().y != faceBoundsOld.getCenter().y ||
		faceBounds.getWidth() != faceBoundsOld.getWidth() ||
		faceBounds.getHeight() != faceBoundsOld.getHeight()) {

		active = true;
	}
	else
		active = false;

	vel = faceBounds.getCenter() - faceBoundsOld.getCenter();
	faceBoundsOld = faceBounds;



	if (trigger)
		trigger = false;
		
	else if (!ignore && snapshots.size() >= snapshotSum)
		trigger = true;
	
}

void candidate::kill() {
	dead = true;
}

void candidate::draw() {

	ofPushStyle();

	// default
	ofNoFill();
	ofSetLineWidth(1);
	ofSetColor(ofColor::grey);

	if (active) {
		ofSetColor(ofColor::green);
		if (isPhotoTime()) {
			ofSetColor(ofColor::white);
			ofSetLineWidth(100);
		}
	}
	if (trigger && active && isPhotoTime()) {
		ofFill();
		ofSetColor(ofColor::white);
	}
	if (ignore && active) {
		ofNoFill();
		ofSetLineWidth(3);
		ofSetColor(ofColor::red);
	}



	ofDrawRectangle(faceBounds);
	


	ofSetLineWidth(3);
	ofSetColor(ofColor::darkGreen);
	ofDrawLine(faceBounds.getCenter(), faceBounds.getCenter() + vel);
	
	ofDrawBitmapStringHighlight("id  :" + ofToString( label ), faceBounds.x + 5, faceBounds.y - 5, ofColor::black, ofColor::white);
	ofDrawBitmapStringHighlight("snap:" + ofToString(snapshots.size()) +"/"+ ofToString(snapshotSum), faceBounds.getCenter().x, faceBounds.y - 5, ofColor::black, ofColor::white);
	ofDrawBitmapStringHighlight("v:" + ofToString(int(vel.length()*100)/100.0), faceBounds.getCenter().x , faceBounds.getBottom()+15, ofColor::black, ofColor::white);

	ofNoFill();
	ofSetColor(ofColor::lightGrey);
	ofRectangle framing = adjustFaceBounds(faceBounds);
	ofDrawRectangle(framing.x, framing.y, framing.width, framing.height);

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


bool candidate::isPhotoTime() {
	bool out = active;

	if (ignore)
		return false;

	//If it's time to take another snapshot
	if (ofGetElapsedTimef() > lastSnapshotTime + snapshotInterval)
		out = true;
	else
		out = false;
	

	
	// prevent taking blurry snapshot
	if (vel.length() > 5)
		out = false;

	return out;
}

void candidate::takeSnapshot(ofImage & snapshot)
{
	if (snapshots.size() < snapshotSum) {

		snapshot.crop(faceBounds.x, faceBounds.y, faceBounds.width, faceBounds.height );
		snapshot.resize(75, 75);
		snapshots.push_back(snapshot);

		//set the mat to ID against
		Mat snapToIdCvColor = ofxCv::toCv(snapshot);
		cvtColor(snapToIdCvColor, cv_evidence, CV_RGB2GRAY);
		evidenceIsSet = true;

		lastSnapshotTime = ofGetElapsedTimef();
	}
}



#include "person.h"
#include "ofApp.h"

using namespace cv;




person::person(int _id, vector<ofImage> & _frames, vector<ofImage> & _snapshots) {

	id = _id;
	f = 0;

	frames = _frames;
	face = frames[ 0 ];
	snapshots = _snapshots;

	ofSeedRandom(ofGetSystemTimeMicros());
	float o = ofRandomf() * ((ofApp*)ofGetAppPtr())->guiAnimationInterval;
	nextTime = ofGetElapsedTimef() + o;


	// convert snapshots to CV Mat
	for (int s = 0; s < snapshots.size(); s++) {
		//snapshots[s].getPixelsRef()
		ofPixels toMatPixels = snapshots[s].getPixels();

		Mat color = ofxCv::toCv(toMatPixels);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);
		snapshotsCV.push_back(grey);

	}


	cout << "New Person added to the list. ID :" << id << endl;
}




void person::setPos(int _x, int _y) {
	x = _x;
	y = _y;
}



void person::draw() {
	ofPushMatrix();
	ofTranslate(x, y);

	// incremenet frame
	if (frames.size() != 0) {
		if (ofGetElapsedTimef() > nextTime) {
			f = (f + 1) % frames.size();
			float p = ((ofApp*)ofGetAppPtr())->guiAnimationInterval;
			nextTime = ofGetElapsedTimef() + p;
		}
		if (frames[f].isAllocated())
			frames[f].draw(-face.getWidth()/2, -face.getHeight());
	}



	ofPopMatrix();
	
}

void person::drawDebug() {
	ofPushMatrix();
	ofTranslate(x, y);
	ofTranslate(-face.getWidth()/2, -face.getHeight());

	ofPushStyle();
	ofNoFill();
	ofSetColor(ofColor::blue);
	ofDrawRectangle(0, 0, face.getWidth(), face.getHeight());
	ofSetColor(ofColor::white);

	ofDrawBitmapString(id, x, y + 20);

	// draw snapshots
	if (false && snapshots.size() > 0) {
		ofPushMatrix();
		//ofScale(0.5, 0.5);
		//for (int s = 0; s < snapshots.size(); s++) {
		//	snapshots[s].draw(0, s * snapshots[s].getHeight());
		//}
		for (int s = 0; s < snapshotsCV.size(); s++) {
			// Draw mat
			ofImage ofMat;
			ofxCv::toOf(snapshotsCV[s], ofMat);
			if (ofMat.isAllocated()) {
				ofMat.update();
				ofMat.draw(snapshots[s].getWidth() * 0, s * snapshots[s].getHeight());
			}
		}
		ofPopMatrix();

	}


	ofDrawBitmapStringHighlight(ofToString(id), face.getWidth()/2+5, -5, ofColor::black, ofColor::white);


	ofPopStyle();
	ofPopMatrix();
}


void person::info() {

	cout << "I'm person: " << id << "\t\tX: " << x << "\tY: " << y <<endl;
}
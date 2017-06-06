#include "person.h"
#include "ofApp.h"

using namespace cv;


person::person() {
	x = ofRandom(1000);
	y = ofRandom(1000);
}

person::person(int _id, int _x, int _y) {
	id = _id;
	x = _x;
	y = _y;
}

person::person(int _id, ofImage & _face) {

	id = _id;

	face = _face;

	cout << "New Person added to the list. ID :" << id << endl;
}

person::person(int _id, ofImage & _face, vector<ofImage> & _snapshots) {

	id = _id;

	face = _face;
	snapshots =_snapshots;

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

person::person(int _id, vector<ofImage> & _frames, vector<ofImage> & _snapshots) {

	id = _id;
	frameNo = 0;

	frames = _frames;
	face = frames[ 0 ];
	snapshots = _snapshots;

	ofSeedRandom(ofGetSystemTimeMicros());
	float p = ofRandomf() * ((ofApp*)ofGetAppPtr())->guiAnimationInterval;
	nextKeyFrame = ofGetElapsedTimef() + 1;


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

person::person( int _id, ofImage & _face, int _x, int _y) {

	id = _id;

	face = _face;
	x = _x;
	y = _y;
	

	cout << "New Person added to the list. ID :" << id << endl;
}


void person::setFace( ofImage & _face){
	face = _face;
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
		if (ofGetElapsedTimef() > nextKeyFrame) {
			frameNo = (frameNo + 1) % frames.size();
			setFace( frames[ frameNo ]);

			//ofSeedRandom(ofGetSystemTimeMicros());
			//float p = ofRandom(0.5) ;
			float p = ((ofApp*)ofGetAppPtr())->guiAnimationInterval;
			nextKeyFrame = ofGetElapsedTimef() + p;
		}
	}

	if (face.isAllocated())
		face.draw(-face.getWidth()/2, -face.getHeight());

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


	if (snapshots.size() > 0) {
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
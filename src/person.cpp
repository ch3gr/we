#include "person.h"

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

person::person(int _id, ofImage _face) {

	id = _id;

	face = _face;

	cout << "New Person added to the list. ID :" << id << endl;
}

person::person(int _id, ofImage _face, vector<ofImage> _snapshots) {

	id = _id;

	face = _face;
	snapshots =_snapshots;

	for (int s = 0; s < snapshots.size(); s++) {
		Mat color = ofxCv::toCv(face);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);
	}


	cout << "New Person added to the list. ID :" << id << endl;
}

person::person( int _id, ofImage _face, int _x, int _y) {

	id = _id;

	face = _face;
	x = _x;
	y = _y;
	

	cout << "New Person added to the list. ID :" << id << endl;
}


void person::setFace( ofImage _face){
	face = _face;
}

void person::setPos(int _x, int _y) {
	x = _x;
	y = _y;
}


void person::update() {
	//ofSeedRandom(id);
	//x += sin((ofGetElapsedTimef() + ofRandom(10)) * 5) * 5;
	//y += cos((ofGetElapsedTimef() + ofRandom(10)) * 5) * 5;
}

void person::draw() {
	ofPushMatrix();
	ofTranslate(x, y);
	ofPushStyle();
	ofNoFill();
	ofSetColor(ofColor::blue);
	ofDrawRectangle(0, 0, face.getWidth(), face.getHeight());
	ofSetColor(ofColor::white);
	if (face.isAllocated())
		face.draw(0, 0);
	ofDrawBitmapString(id, x, y+20);

	// Debuging
	if (snapshots.size() > 0) {
		//ofScale(0.5, 0.5);
		for (int s = 0; s < snapshots.size(); s++) {
			snapshots[s].draw(0, s * snapshots[s].getHeight());
		}
	}

	ofPopStyle();
	ofPopMatrix();
}

void person::info() {

	cout << "I am person : " << id << "X: " << x << endl;
}
#include "person.h"


person::person() {
	x = ofRandom(1000);
	y = ofRandom(1000);
}

person::person(int _id, int _x, int _y) {
	id = _id;
	x = _x;
	y = _y;
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

void person::update() {
	ofSeedRandom(id);
	x += sin((ofGetElapsedTimef() + ofRandom(10)) * 5) * 5;
	y += cos((ofGetElapsedTimef() + ofRandom(10)) * 5) * 5;
}

void person::draw() {
	if (face.isAllocated())
		face.draw(x, y);
	ofDrawBitmapString(id, x, y);
}
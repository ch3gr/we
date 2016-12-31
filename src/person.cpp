#include "person.h"



person::person() {
	x = ofRandom(1000);
	y = ofRandom(1000);
}

person::person(int _x, int _y) {
	x = _x;
	y = _y;
}

void person::draw() {
	ofDrawBitmapString("I am a person", x, y);
}
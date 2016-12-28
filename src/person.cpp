#include "person.h"



person::person() {
	x = 100;
	y = 100;
}

person::person(int _x, int _y) {
	x = _x;
	y = _y;
}


void person::draw() {

	ofDrawBitmapString("I am a person", x, y);

}

person::~person() {
}

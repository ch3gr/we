#include "person.h"



person::person() {
	x = 10;
	y = 10;
}


void person::draw() {

	ofDrawBitmapString("I am a person", x, y);

}

person::~person() {
}

#include "person.h"



person::person() {
}


void person::draw(int x, int y) {
	ofDrawBitmapString("I am a person", x, y);
}
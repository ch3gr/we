#include "manager.h"


manager::manager(){
}

manager::manager(int _camW, int _camH)
{
	nextPersonId = 0;
	camW = _camW;
	camH = _camH;
	
	canvas.allocate(ofGetWindowWidth(), ofGetWindowHeight());
}


manager::~manager()
{
}

void manager::addPerson(ofImage _face, int _x, int _y) {
	person someoneNew = person(nextPersonId++, _face, _x, _y);
	we.push_back(someoneNew);
}

void manager::update() {
	for (int p = 0; p < we.size(); ++p)
		we[p].update();
}

void manager::draw() {
	update();

	canvas.begin();
	ofClear(0);

	for (int p = 0; p < we.size(); ++p)
		we[p].draw();

	canvas.end();

	canvas.draw(0, 0);
}

void manager::info() {
	cout << "I am a manager and these are my people :" << endl;
	for (int p = 0; p < we.size(); ++p)
		we[p].info();
}

void manager::clearPeople() {
	we.clear();
}
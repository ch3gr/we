#pragma once
#include "ofMain.h"
#include "person.h"


class manager
{
public:
	manager();
	manager(int _camW, int _camH);
	~manager();


	void manager::addPerson(ofImage _face, int _x, int _y);
	void manager::update();
	void manager::draw();
	void manager::info();
	void manager::clearPeople();


	int camW, camH;

	vector<person> we;
	int nextPersonId;
	ofFbo canvas;
};


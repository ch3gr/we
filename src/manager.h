#pragma once
#include "ofMain.h"
#include "person.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"

class manager
{
public:
	manager();
	manager(int _camW, int _camH);
	~manager();


	void manager::addPerson(ofImage _face, int _x, int _y);
	void manager::update();
	void manager::draw();
	void manager::drawDebug();
	void manager::info();
	void manager::clearPeople();

	ofImage manager::makePortrait(ofImage cam, ofImage bg);


	int camW, camH;
	bool debugMode;

	vector<person> we;
	int nextPersonId;
	ofFbo canvas;
	ofFbo debug;

	ofShader shdPrep, shdComp;

	ofxCv::ObjectFinder faceFinder;

};


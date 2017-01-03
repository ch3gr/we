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
	void manager::setBg(ofImage _bg);
	void manager::setBg(ofVideoGrabber _cam);
	void manager::update();
	void manager::draw();
	void manager::drawDebug();
	void manager::info();
	void manager::clearPeople();


	void manager::detectFaces(ofImage cam);
	void manager::detectFaces(ofVideoGrabber cam);
	ofImage manager::makePortrait(ofImage cam, float shdThress);


	int camW, camH;

	vector<person> we;
	int nextPersonId;
	ofFbo canvas;
	ofFbo debug;
	

	ofShader shdPrep, shdComp;

	ofxCv::ObjectFinder managerFFinder;

private:
	ofImage bg;

};


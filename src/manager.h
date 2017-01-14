#pragma once
#include "ofMain.h"
#include "candidate.h"
#include "person.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"


using namespace ofxCv;

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
	void manager::drawDebug(ofImage camFrame);
	void manager::drawDebugTrackers();
	void manager::info();
	void manager::clearPeople();
	void manager::curate();


	void manager::detectFaces(ofImage cam);
	void manager::detectFaces(ofVideoGrabber cam);
	ofImage manager::makePortrait(ofImage cam, ofRectangle crop, float shdThress);


	int camW, camH;
	float previewScale;

	vector<person> we;
	int nextPersonId;
	ofFbo canvas;
	ofFbo debugPortrait;
	ofFbo debugTrackers;
	

	ofShader shdPrep, shdComp;

	ObjectFinder scout;
	RectTrackerFollower<candidate> candidates;
	

	
private:
	ofImage bg;

};


ofRectangle adjustFaceBounds(ofRectangle _faceBounds, int camW, int camH);
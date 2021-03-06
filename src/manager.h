#pragma once
#include "ofMain.h"
#include "candidate.h"
#include "person.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "modelThread.h"



using namespace ofxCv;
using namespace cv;





class manager
{
public:

	manager();
	~manager();


	void manager::addPerson(vector<ofImage> &_frames, vector<ofImage> & _snapshots);
	void manager::setBg(ofImage & _bg);
	void manager::setBg(ofVideoGrabber & _cam);
	void manager::draw();
	void manager::drawDebugPortrait(ofImage & camFrame);
	void manager::drawDebugTrackers();
	void manager::info();
	void manager::remove();
	void manager::forgetUs();
	void manager::saveUs(bool);
	void manager::loadUs();
	void manager::curate();

	void manager::addMonkey();

	void manager::detectFaces(ofImage & cam);
	void manager::detectFaces(ofVideoGrabber & cam);
	void manager::captureFrame(ofImage & cam, candidate & follower);
	person manager::getPerson(int id);

	

	float debugTrackersScale;
	float debugPortraitScale;


	bool animation;
	bool portraitWithAlpha;


	bool debugPeople;
	bool debugUpdateEvidence;
	bool debugPortrait;
	bool debugTrackers;

	vector<person> we;
	int nextPersonId;
	ofFbo FBO_debugPortrait;
	ofFbo FBO_debugTrackers;
	int selection;
	string session;

	ofShader shdPrep, shdComp;

	ObjectFinder scout;
	RectTrackerFollower<candidate> candidates;
	
	// Threaded face recognition model !!!
	modelThread model;
	
	
	

private:
	ofImage bg;
	float flash;

};






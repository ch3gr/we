#pragma once
#include "ofMain.h"
#include "candidate.h"
#include "person.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"


using namespace ofxCv;
using namespace cv;


class manager
{
public:
	manager();
	manager(int _camW, int _camH);
	~manager();


	void manager::addPerson(ofImage _face);
	void manager::addPerson(ofImage _face, vector<ofImage> _snapshots);
	void manager::setBg(ofImage _bg);
	void manager::setBg(ofVideoGrabber _cam);
	void manager::draw();
	void manager::drawDebug(ofImage camFrame);
	void manager::drawDebugTrackers();
	void manager::info();
	void manager::clearPeople();
	void manager::curate();


	void manager::detectFaces(ofImage cam);
	void manager::detectFaces(ofVideoGrabber cam);
	ofImage manager::makePortrait(ofImage cam, ofRectangle crop);
	person manager::getPerson(int id);

	int camW, camH;
	int faceDetectW;
	int contourDetectW;
	float debugPortraitScale;

	bool portraitWithAlpha;


	bool debugPeople;
	bool debugUpdateEvidence;
	bool debugPortrait;
	bool debugTrackers;
	bool debugTest;

	vector<person> we;
	int nextPersonId;
	ofFbo FBO_debugPortrait;
	ofFbo FBO_debugTrackers;
	

	ofShader shdPrep, shdComp;
	float shdPrepThress;

	ObjectFinder scout;
	RectTrackerFollower<candidate> candidates;
	
	// face recognition model
	Ptr<FaceRecognizer> model;
	vector<Mat> modelFaces;
	vector<ofImage> modelOfFaces;
	vector<int> modelLabels;
	
private:
	ofImage bg;

};


ofRectangle adjustFaceBounds(ofRectangle _faceBounds, int camW, int camH);

float getTimeDiff(float refTime);


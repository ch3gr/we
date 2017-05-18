#pragma once

#include "ofMain.h"
#include "manager.h"
#include "person.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
#include "ofxGui.h"


using namespace cv;

class ofApp : public ofBaseApp {

public:

	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);



	

	ofVideoGrabber		cam;
	ofFbo				camHacked;
	float lastUpdate;


	// Sample people faces for DEBUGING
	int uCount;
	vector< vector<ofImage> > samplePeople;
	int selectSamplePerson;
	float fScale;


	// color picker
	bool mouseLB_Pressed;





	manager manage;


	bool showGui;
	ofxPanel gui;
	ofxLabel winSize;
	ofxLabel camSize;

	ofxFloatSlider guiSpeedThress;
	ofxFloatSlider guiTrackerWidth;
	ofxFloatSlider guiTrackerMinSize;
	ofxFloatSlider guiTrackerMaxSize;

	ofxFloatSlider guiLumaKey;
	ofxFloatSlider guiContourImgScale;
	ofxIntSlider guiContourImgSimplify;
	ofxIntSlider guiContourSmooth;

	ofxFloatSlider guiDebugSplit;

	ofxToggle guiDebugTrackers;
	ofxToggle guiDebugPortrait;
	ofxToggle guiDebugPeople;
	ofxToggle guiDebugUpdateEvidence;
	
};



// Video stream resolution
const int camW = 640;
const int camH = 480;
//const int camW = 640;
//const int camH = 360;
//const int camW = 1280;
//const int camH = 720;
//const int camW = 1920;
//const int camH = 1080;






ofRectangle adjustFaceBounds(ofRectangle _faceBounds);
float getTimeDiff(float refTime);
string padThis(int pads, int number);
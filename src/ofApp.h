#pragma once

#include "ofMain.h"
#include "manager.h"
#include "person.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"


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



	void ofApp::faceDetectSetup();
	void ofApp::faceDetectUpdate();
	void ofApp::faceDetectDraw();

	void ofApp::drawWithNormals(const ofPolyline& polyline);

	bool debugMode;
	ofFbo debugView;


	int camW, camH;
	float				camProxySize;
	ofVideoGrabber		cam;
	ofImage frame;						// cam + comps
	ofImage frameCompute;				// reduced frame for computation

	ofImage background;
	ofFbo faceMask;						// face mask from faceTrack
	ofFbo contourMask;					// calculated contours

	ofFbo faceDetectMask;				// buffer for face detection square masks
	ofFbo prep;							// buffer for image before contour detection
	ofFbo comp;							// buffer for final person layer




	ofShader shdPrep, shdComp;
	ofImage img1;
	ofImage img2;

	float shdPrepThress;

	// Contour detection openCV
	ofxCvColorImage cvImgColor, cvImgColor2;
	ofxCvGrayscaleImage cvImgGrayscale;
	ofxCvContourFinder contourFinder;
	vector<ofPath> controurSurfaces;

	int threshold;
	int smooth;


	ofxCv::ObjectFinder faceFinder;

	//	face detection and recognition
	vector<Rect> objects;
	CascadeClassifier classifier;
	Ptr<FaceRecognizer> model;

	vector<Mat> images, faces;
	vector<ofImage> ofFaces;
	vector<int> labels;

	ofImage mouseFace;
	ofImage unknownFace;

	int match[10];
	double confidence[10];

	int mousePic;




	manager manage;


};

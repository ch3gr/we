#pragma once

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxCv.h"
using namespace cv;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
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




		int 				camW;
		int 				camH;
		float				camProxySize;
		ofVideoGrabber		cam;
		ofImage frame;						// cam + comps
		ofImage frameCompute;				// reduced frame for computation

		ofFbo personCanvas;					// extracted face



		//	face detection and recognition
		vector<cv::Rect> objects;
		CascadeClassifier classifier;
		Ptr<FaceRecognizer> model;

		vector<cv::Mat> images, faces;
		vector<ofImage> ofFaces;
		vector<int> labels;
		
		ofImage mouseFace;
		ofImage unknownFace;

		int match[10];
		double confidence[10];

		int mousePic;



		// Contour detection
		ofxCvColorImage cvImgColor, cvImgColor2;
		ofxCvGrayscaleImage cvImgGrayscale;
		ofxCvContourFinder contourFinder;
		vector<ofPolyline> polylines, smoothed, resampled;
		vector<ofPath> paths;
		vector<ofRectangle> boundingBoxes;
		int threshold;
		int smooth;

		
		

		ofShader shader;
		ofImage img1;
		ofImage img2;
};

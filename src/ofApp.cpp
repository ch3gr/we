#include "ofApp.h"


using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
	CamW = 640;
	CamH = 480;
	CvScale = 0.25;
	Cam.setup(CamW, CamH);
	CvImg.allocate(CamW, CamH);

	Finder.setup("haarcascade_frontalface_default.xml");
	//finder.setup("haarcascade_frontalface_alt.xml");
	//finder.setup("HS.xml");
	
	Finder2.setup("haarcascade_frontalface_default.xml");
	Finder2.setPreset(ObjectFinder::Fast);
}


//--------------------------------------------------------------
void ofApp::update(){

	Cam.update();
	
	if (Cam.isFrameNew()) {
		
		CvImg.setFromPixels(Cam.getPixels());
		CvImg.resize(CvImg.width*CvScale, CvImg.height*CvScale);

		Finder.findHaarObjects(CvImg.getPixels());

		Finder2.update(Cam);
	}


	// FPS on title bar
	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	Cam.draw(0, 0);
	Finder2.draw();

	CvImg.draw(CamW+50, 0);


	ofNoFill();
	ofSetColor(0, 0, 255);
	for (unsigned int i = 0; i < Finder.blobs.size(); i++) {
		ofRectangle cur = Finder.blobs[i].boundingRect;
		ofDrawRectangle(cur.x/CvScale, cur.y/CvScale, cur.width/CvScale, cur.height/CvScale);
	}

	ofSetColor(255, 255, 255);
	if (Finder.blobs.size() > 0) {
		ofImage face = Cam.getPixels();
		ofRectangle cur = Finder.blobs[0].boundingRect;
		face.crop(cur.x / CvScale, cur.y / CvScale, cur.width / CvScale, cur.height / CvScale);
		face.resize(100, 100);
		face.draw(0, CamH + 25);
	}

	ofDrawBitmapString(Finder.blobs.size(), 100, 100);
	
	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

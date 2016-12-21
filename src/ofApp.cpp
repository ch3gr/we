#include "ofApp.h"


using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
	CamW = 640;
	CamH = 480;
	Cam.setup(CamW, CamH);

	
	Finder.setup("haarcascade_frontalface_default.xml");
	Finder.setPreset(ObjectFinder::Fast);
}


//--------------------------------------------------------------
void ofApp::update(){

	Cam.update();
	
	if (Cam.isFrameNew()) {
		Finder.update(Cam);
	}


	// FPS on title bar
	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());
	
}

//--------------------------------------------------------------
void ofApp::draw(){
	Cam.draw(0, 0);
	Finder.draw();

	
	
	if (Finder.size() > 0) {
		ofImage face = Cam.getPixels();
		
		face.crop(Finder.getObject(0).getMinX(), Finder.getObject(0).getMinY(), Finder.getObject(0).getWidth(), Finder.getObject(0).getHeight());
		face.resize(100, 100);
		face.draw(0, CamH + 25);
	}

	
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

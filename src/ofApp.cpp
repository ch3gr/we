#include "ofApp.h"


using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
	/*CamW = 640;
	CamH = 480;
	Cam.setup(CamW, CamH);

	
	Finder.setup("haarcascade_frontalface_default.xml");
	Finder.setPreset(ObjectFinder::Fast);*/







	// setup face detection
	classifier.load(ofToDataPath("haarcascade_frontalface_alt.xml"));

	ofSetFrameRate(10);
	cam.initGrabber(320, 240);

	// load images from data directory
	ofDirectory dir;
	int num = dir.listDir("faces1");
	for (int i = 0; i<num; i++) {
		// load into OF
		ofImage img;
		img.loadImage(dir.getPath(i));

		// convert into openCV, grayscale

		Mat color = ofxCv::toCv(img);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);

		images.push_back(grey);

		// labels == who you are
		labels.push_back(i);
	}

	num = dir.listDir("faces2");
	for (int i = 0; i < num; i++) {
		// load into OF
		ofImage img;
		img.loadImage(dir.getPath(i));
		// convert into openCV, grayscale
		Mat color = ofxCv::toCv(img);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);
		images.push_back(grey);
		// labels == who you are
		labels.push_back(6);
	}


	// use haar classifier to detect faces
	for (int i = 0; i<images.size(); i++) {

		// detect faces in image
		classifier.detectMultiScale(images[i], objects, 1.06, 1, 0);

		// if there are some faces
		if (objects.size() > 0) {
			// crops image to face rect
			Mat roi(images[i], objects[0]);

			// get everybody to the same size
			Mat smallMat;
			smallMat.create(100, 100, ofxCv::getCvImageType(roi));

			resize(roi, smallMat, smallMat.size(), 0, 0, INTER_LINEAR);

			faces.push_back(smallMat);

			ofImage toSave;
			ofxCv::toOf(smallMat, toSave);
			toSave.update();

			ofFaces.push_back(toSave);
		}

		objects.clear();
	}




	// load faces into trainer
	// The following lines create an Eigenfaces model for
	// face recognition and train it with the images and labels
	model = createEigenFaceRecognizer();
	model->train(faces, labels);

	currentTest = 0;
	currentResult = -1;

}




















//--------------------------------------------------------------
void ofApp::update(){

	//Cam.update();
	//
	//if (Cam.isFrameNew()) {
	//	Finder.update(Cam);
	//}


	//// FPS on title bar
	//std::stringstream strm;
	//strm << "fps: " << ofGetFrameRate();
	//ofSetWindowTitle(strm.str());










	cam.update();
	if (cam.isFrameNew()) {
		//ofxCv::convertColor(cam, gray, CV_RGB2GRAY);
		Mat color = ofxCv::toCv(cam);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);

		classifier.detectMultiScale(grey, objects, 1.06, 1, 0);

		if (objects.size() > 0) {
			// crops image to face rect
			Mat roi(grey, objects[0]);

			// get everybody to the same size
			Mat smallMat;
			smallMat.create(100, 100, ofxCv::getCvImageType(roi));

			resize(roi, smallMat, smallMat.size(), 0, 0, INTER_LINEAR);

			currentResult = model->predict(smallMat);
		}

	}
	//currentResult = model->predict(faces[currentTest]);




}













//--------------------------------------------------------------
void ofApp::draw(){
	/*Cam.draw(0, 0);
	Finder.draw();

	
	
	if (Finder.size() > 0) {
		ofImage face = Cam.getPixels();
		
		face.crop(Finder.getObject(0).getMinX(), Finder.getObject(0).getMinY(), Finder.getObject(0).getWidth(), Finder.getObject(0).getHeight());
		face.resize(100, 100);
		face.draw(0, CamH + 25);
	}
	if (Finder.size() > 1) {
		ofImage face = Cam.getPixels();
		face.crop(Finder.getObject(1).getMinX(), Finder.getObject(1).getMinY(), Finder.getObject(1).getWidth(), Finder.getObject(1).getHeight());
		face.resize(100, 100);
		face.draw(120, CamH + 25);
	}*/



	ofSetColor(255, 255, 255);
	cam.draw(0, 300);

	int x = 0;
	for (int i = 0; i<ofFaces.size(); i++) {
		if (currentResult == i) {
			ofSetColor(150, 0, 0);
		}
		else {
			ofSetColor(255);
		}
		ofFaces[i].draw(x, 0);
		ofDrawBitmapString(i, x, 30);
		if (currentTest == i) {
			ofSetColor(255, 0, 0);
			//ofDrawBitmapString("Test", x, ofFaces[i].height + 20);
			ofDrawBitmapString("Test", x, 120);
		}
		//x += ofFaces[i].width;
		x += 120;
	}

	ofDrawBitmapString("Testing image " + ofToString(currentTest + 1) + ", which I, the computer, think is image " + ofToString(currentResult + 1), 20, 200);

	//ofDrawBitmapString("result: "+ ofToString(result), 20,220);





	
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){





	if (key == '+') {
		currentTest++;
		if (currentTest >= faces.size()) {
			currentTest = 0;
		}
	}
	else if (key == '-') {
		currentTest--;
		if (currentTest < 0) {
			currentTest = faces.size() - 1;
		}
	}




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

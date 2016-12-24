#include "ofApp.h"


using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
void ofApp::setup(){
	//Cam.setup(CamW, CamH);
	//Finder.setup("haarcascade_frontalface_default.xml");
	//Finder.setPreset(ObjectFinder::Fast); */
	

	camW = 640;
	camH = 480;
	camProxySize = 0.25;
	
	finder.setup("haarcascade_frontalface_default.xml");
	//finder.setPreset(ObjectFinder::Fast);

	// setup face detection
	
	classifier.load(ofToDataPath("haarcascade_frontalface_default.xml"));
	//ofSetFrameRate(10);
	cam.initGrabber(camW, camH);

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

	testFace.loadImage("testFace.jpg");
	testFace.resize(testFace.getWidth()*2, testFace.getWidth()*2);


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

	
	cam.update();
	if (cam.isFrameNew()) {

		//ofxCv::convertColor(cam, gray, CV_RGB2GRAY);
		frame = cam.getPixels();

		testFace.getPixels().pasteInto(frame.getPixelsRef(), ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY);
		frame.update();

		// prep frameCompute and run the classifiers
		frameCompute.clone(frame);
		frameCompute.resize(camW*camProxySize, camH*camProxySize);


		finder.update(frameCompute);

		Mat color = ofxCv::toCv(frameCompute);
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
	


	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());


}













//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255, 255, 255);
	ofSetLineWidth(3);
	cam.draw(0, 0);
	ofDrawBitmapString("Camera feed", 0,10);

	frameCompute.draw(camW, camH-frameCompute.getHeight());
	ofDrawBitmapString("Frame to compute", camW, camH - frameCompute.getHeight()+10);

	//testFace.draw(ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY);
	

	// finder Stuff
	//finder.draw();
	for (int i = 0; i < finder.size(); ++i) {
		ofImage face;
		face.clone(frame);
		ofRectangle crop = finder.getObject(i);
		crop.x /= camProxySize;
		crop.y /= camProxySize;
		crop.scale(1 / camProxySize);

		face.crop(crop.x, crop.y, crop.width, crop.width);
		face.resize(100, 100);
		face.draw(camW, i*face.getHeight());
		ofDrawBitmapString("finder", camW, i*face.getHeight()+10);

		ofNoFill();
		ofSetColor(255, 0, 0);
		ofDrawRectangle(crop);
		ofSetColor(255, 255, 255);
	}

	// classifier
	for (int i = 0; i < objects.size(); ++i) {
		ofImage face;
		face.clone(frame);
		ofRectangle crop = ofRectangle(objects[i].x, objects[i].y, objects[i].width, objects[i].height);
		crop.x /= camProxySize;
		crop.y /= camProxySize;
		crop.scale(1 / camProxySize);

		face.crop(crop.x, crop.y, crop.width, crop.height);
		face.resize(100, 100);
		face.draw(i*100, camH);
		ofDrawBitmapString("classifier", i * 100, camH+10);

		ofNoFill();
		ofSetColor(0, 0, 255);
		ofDrawRectangle(crop);
		ofSetColor(255, 255, 255);
	}
	ofSetLineWidth(1);


	int x = 0;
	for (int i = 0; i<ofFaces.size(); i++) {
		if (currentResult == i) {
			ofSetColor(150, 0, 0);
		}
		else {
			ofSetColor(255);
		}
		ofFaces[i].draw(x, camH + 100);
		ofDrawBitmapString(i, x, camH + 120);
		if (currentTest == i) {
			ofSetColor(255, 0, 0);
			ofDrawBitmapString("Test", x, camH+200);
		}
		//x += ofFaces[i].width;
		x += 120;
	}





	ofDrawBitmapString("Testing image " + ofToString(currentTest + 1) + ", which I, the computer, think is image " + ofToString(currentResult + 1), 20, camH + 230);

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

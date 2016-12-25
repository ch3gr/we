#include "ofApp.h"


using namespace ofxCv;
using namespace cv;

//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setup(){

	camW = 640;
	camH = 480;
	//ofSetFrameRate(10);
	camProxySize = 0.25;
	cam.initGrabber(camW, camH);


	// setup face detection
	classifier.load(ofToDataPath("haarcascade_frontalface_default.xml"));

	

	string trainPath = "toTrain";
	cout << "training directory: " << trainPath << endl;
	
	ofDirectory trainDir;
	int personNum = trainDir.listDir(trainPath);
	for (int p = 0; p < personNum; p++) {
		string personDirPath = trainDir.getPath(p);
		cout << "Person dir: " << personDirPath << endl;

		ofDirectory personDir;
		int sampleNum = personDir.listDir(personDirPath);;
		for (int s = 0; s < sampleNum; s++) {
			string samplePath = personDir.getPath(s);
			cout << "Sample: " << samplePath << endl;


			// load photo into OF
			ofImage img;
			img.loadImage(samplePath);
			// Load one sample per person
			if( s==0 )
				ofFaces.push_back(img);

			// convert into openCV, grayscale
			Mat color = ofxCv::toCv(img);
			Mat grey;
			cvtColor(color, grey, CV_RGB2GRAY);
			images.push_back(grey);
			// labels == who you are
			labels.push_back(p);

		}
	}

	cout << " ___ Loaded ____" << endl;
	cout << "images : " << images.size() << endl;
	cout << "labels : " << labels.size() << endl;
	cout << "ofImages : " << ofFaces.size() << endl;
	
	
	


	mousePic = ofFaces.size(); // just to select the unknowPerson who is always the group+1
	unknownFace.loadImage("testFace2.jpg");




	//// use haar classifier to detect faces
	//for (int i = 0; i<images.size(); i++) {

	//	// detect faces in image
	//	classifier.detectMultiScale(images[i], objects, 1.06, 1, 0);

	//	// if there are some faces
	//	if (objects.size() > 0) {
	//		// crops image to face rect
	//		Mat roi(images[i], objects[0]);

	//		// get everybody to the same size
	//		Mat smallMat;
	//		smallMat.create(100, 100, ofxCv::getCvImageType(roi));
	//		resize(roi, smallMat, smallMat.size(), 0, 0, INTER_LINEAR);
	//		faces.push_back(smallMat);
	//		ofImage toSave;
	//		ofxCv::toOf(smallMat, toSave);
	//		toSave.update();

	//		ofFaces.push_back(toSave);
	//	}

	//	objects.clear();
	//}




	// load faces into trainer
	// The following lines create an Eigenfaces model for
	// face recognition and train it with the images and labels
	model = createEigenFaceRecognizer(80, 5000);
	//model = createFisherFaceRecognizer(0, 5000);
	//model = createLBPHFaceRecognizer();
	model->train(images, labels);

	
	



	personCanvas.allocate(512, 512, GL_RGBA);
	personCanvas.begin();
	ofClear(255, 255, 255, 0);
	personCanvas.end();
}















//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::update(){

	
	cam.update();
	if (cam.isFrameNew()) {

		//ofxCv::convertColor(cam, frame, CV_RGB2GRAY);
		frame = cam.getPixels();
		
		if (mousePic < ofFaces.size())
			mouseFace = ofFaces[mousePic];
		else
			mouseFace = unknownFace;

		/*mouseFace.resize(200, 200);
		mouseFace.getPixels().pasteInto(frame.getPixelsRef(), ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY);
		frame.update();*/

		// prep frameCompute and run the classifiers
		frameCompute.clone(frame);
		frameCompute.resize(camW*camProxySize, camH*camProxySize);


		Mat color = ofxCv::toCv(frameCompute);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);

		classifier.detectMultiScale(grey, objects, 1.16, 1, 0);

		for(int i = 0; i < objects.size(); ++i ){
			// crops image to face rect
			Mat roi(grey, objects[i]);

			// get everybody to the same size
			Mat smallMat;
			smallMat.create(100, 100, ofxCv::getCvImageType(roi));
			resize(roi, smallMat, smallMat.size(), 0, 0, INTER_LINEAR);

			model->predict(smallMat, match[i], confidence[i]);
			


			if (i == 0) {

				ofImage face;
				face.clone(frame);

				ofRectangle crop = ofRectangle(objects[i].x, objects[i].y, objects[i].width, objects[i].height);
				// adjust found area to original resolution
				crop.x /= camProxySize;
				crop.y /= camProxySize;
				crop.scale(1 / camProxySize);
				// adjust
				//crop.translateY(-50);
				crop.setFromCenter( crop.getCenter(), 200,250);
				face.crop(crop.x, crop.y, crop.width, crop.height);
				
				personCanvas.begin();
				ofClear(200, 255, 255, 255);
				//face.draw(face.getWidth()/2, face.getHeight()/2);
				face.draw(0,0);
				personCanvas.end();
			}
		}
	}
	


	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());


	
	
	
}













//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255, 255, 255);
	ofSetLineWidth(3);
	frame.draw(0, 0);
	ofDrawBitmapString("Frame comped", 0,10);

	frameCompute.draw(camW, camH-frameCompute.getHeight());
	ofDrawBitmapString("Frame to compute", camW, camH - frameCompute.getHeight()+10);


	// classifier
	for (int i = 0; i < objects.size(); ++i) {
		ofImage face;
		face.clone(frame);
		ofRectangle crop = ofRectangle(objects[i].x, objects[i].y, objects[i].width, objects[i].height);
		crop.x /= camProxySize;
		crop.y /= camProxySize;
		crop.scale(1 / camProxySize);

		ofSetColor(0, 0, 0);
		ofDrawBitmapString(match[i], i * 100+1, camH+20+1);
		ofDrawBitmapString(confidence[i], i * 100+1, camH + 40+1);

		ofSetColor(255, 0, 0);
		ofDrawBitmapString(match[i], i * 100, camH + 20);
		ofDrawBitmapString(confidence[i], i * 100, camH + 40);
		ofSetColor(255, 255, 255);

		face.crop(crop.x, crop.y, crop.width, crop.height);
		face.resize(100, 100);
		face.draw(i*100, camH+50);

		ofNoFill();
		ofSetColor(0, 0, 255);
		ofDrawRectangle(crop);
		ofSetColor(255, 255, 255);
	}
	ofSetLineWidth(1);


	for (int i = 0; i<ofFaces.size(); i++) {
		ofFaces[i].draw(i*100, camH + 150);
		ofDrawBitmapString(i, i*100, camH + 170);
	}


	
	
	personCanvas.draw(camW + 150, 0);
}










//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	if (key == 'd') {
//		cout << model->getAlgorithm() << endl;
	}

	if (key == '+') {
		mousePic++;
		// include the unknown face
		if (mousePic > ofFaces.size())
			mousePic = ofFaces.size()-0;
	}
	if (key == '-') {
		mousePic--;
		if (mousePic < 0)
			mousePic = 0;
	}

	if (key == ']') {
		double thress = model->getDouble("threshold");
		thress += 250;
		model->set("threshold", thress);
		cout << "Model thress: " << thress << endl;
	}
	if (key == '[') {
		double thress = model->getDouble("threshold");
		if( thress >= 250 )
			thress -= 250;
		model->set("threshold", thress);
		cout << "Model thress: " << thress << endl;
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

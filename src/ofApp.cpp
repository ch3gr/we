#include "ofApp.h"




using namespace ofxCv;
using namespace cv;







//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setup() {




	shdPrepThress = 0.1;

	camW = 640;
	camH = 480;

	manage = manager(camW, camH);

	//ofSetFrameRate(10);

	camProxySize = 0.25;
	cam.initGrabber(camW, camH);

	threshold = 90;


	cvImgColor.allocate(camW, camH);			
	cvImgGrayscale.allocate(camW, camH);

	smooth = 2;

	//faceDetectSetup();



	debugMode = false;


	img1.allocate(camW, camH, OF_IMAGE_COLOR);

	img1.loadImage("testFace1.jpg");
	img2.loadImage("testFace2.jpg");
	img1.resize(camW, camH);
	img2.resize(camW, camH);


	// Doesn't grab anything, cam is still empty, kind of works
	//manage.setBg(cam);
}















//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::update() {


	cam.update();
	if (cam.isFrameNew()) {



		//ofxCv::convertColor(cam, frame, CV_RGB2GRAY);
		//frame = cam.getPixels();

		/*
		if (mousePic < ofFaces.size())
		mouseFace = ofFaces[mousePic];
		else
		mouseFace = unknownFace;

		mouseFace.resize(200, 200);
		mouseFace.getPixels().pasteInto(frame.getPixelsRef(), ofGetAppPtr()->mouseX, ofGetAppPtr()->mouseY);
		frame.update();
		*/


		//frameCompute.clone(frame);
		//frameCompute.resize(camW*camProxySize, camH*camProxySize);

		//faceDetectUpdate();




		//ofImage bridge;
		//personCanvas.readToPixels(bridge.getPixelsRef());

		//personCanvas.readToPixels(img1.getPixelsRef());
		//cvImgColor2.setFromPixels( bridge.getPixels() );
		//cvImgColor2.setFromPixels(cam.getPixels());

		//cvImgTmp.setFromPixels(cvImgColor.getPixels());
		//cvImgTmp.setFromPixels( personCanvas.)
		//cvImgColor *= cvImgColor2;



	}




}











//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::draw() {

	ofClear(ofColor::grey);
	ofDrawBitmapString("start draw", 0, 20);



	if (debugMode)
	{
		manage.drawDebug();
		// direct draw portrait
		ofImage camFrame;
		camFrame.setFromPixels(cam.getPixels());
		manage.makePortrait(camFrame, shdPrepThress).draw(0,camH);
	}
	manage.draw();




	//cam.draw(0,0);
	//ofImage cameraFrame ;
	//cameraFrame.setFromPixels(cam.getPixels());
	//cameraFrame.draw(400, 400);


	//Mat colorMat = ofxCv::toCv(cameraFrame.getPixelsRef());
	//Mat colorMat = ofxCv::toCv(cam.getPixels().getData(), camW, camH);
	//Mat colorMat;
	//colorMat.setFromPixels(cam.getPixels().getData(), camW, camH);

	//Mat colorMat = ofxCv::toCv(cam.getPixelsRef());
	//ofImage colorOf;
	//toOf(colorMat, colorOf);
	//colorOf.draw(600, 100);

	//ofImage bridge;
	//personCanvas.readToPixels(bridge.getPixelsRef());

	//personCanvas.readToPixels(img1.getPixelsRef());
	//cvImgColor2.setFromPixels( bridge.getPixels() );
	//cvImgColor2.setFromPixels(cam.getPixels());

	//cvImgTmp.setFromPixels(cvImgColor.getPixels());
	//cvImgTmp.setFromPixels( personCanvas.)
	//cvImgColor *= cvImgColor2;


	ofImage camFrame;
	camFrame.setFromPixels(cam.getPixels());





	// OF Time
	ofDrawBitmapString(ofGetElapsedTimef(), 5, ofGetWindowHeight() - 40);

	//	Framerate
	std::stringstream strm;
	strm << "fps: " << ofGetFrameRate();
	ofSetWindowTitle(strm.str());

}


























//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'b')
	{
		manage.setBg(cam);
	}

	if (key == 'c')
	{
		manage.clearPeople();
	}

	if (key == 'd') {
		debugMode = !debugMode;
	}

	// info
	if (key == 'i') {
		manage.info();
	}

	/////// temp keys





	if (key == '+') {
		mousePic++;
		// include the unknown face
		if (mousePic > ofFaces.size())
			mousePic = ofFaces.size() - 0;
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
		if (thress >= 250)
			thress -= 250;
		model->set("threshold", thress);
		cout << "Model thress: " << thress << endl;
	}

	//		shdPrepThress
	if (key == ',') {
		shdPrepThress = ofClamp(shdPrepThress - 0.01, 0, 1);
		cout << "shdPrepThress : " << shdPrepThress << endl;
	}
	if (key == '.') {
		shdPrepThress = ofClamp(shdPrepThress + 0.01, 0, 1);
		cout << "shdPrepThress : " << shdPrepThress << endl;
	}



	if (key == 'w')
	{
		threshold = ofClamp(threshold + 5, 0, 255);
		cout << "Threshold : " << threshold << endl;
	}
	if (key == 'q')
	{
		threshold = ofClamp(threshold - 5, 0, 255);
		cout << "Threshold : " << threshold << endl;
	}
	if (key == 'x')
	{
		smooth = ofClamp(smooth + 1, 0, 50);
		cout << "Smooth : " << smooth << endl;
	}
	if (key == 'z')
	{
		smooth = ofClamp(smooth - 1, 0, 50);
		cout << "Smooth : " << smooth << endl;
	}




}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

	ofImage camFrame;
	camFrame.setFromPixels(cam.getPixels());
	ofImage portraint = manage.makePortrait(camFrame, shdPrepThress);
	manage.addPerson(portraint, mouseX, mouseY);


}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}









//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//	Various functions









//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------



void ofApp::faceDetectSetup() {
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
			if (s == 0)
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


}






void ofApp::faceDetectUpdate() {

	Mat color = ofxCv::toCv(frameCompute);
	Mat grey;
	cvtColor(color, grey, CV_RGB2GRAY);

	classifier.detectMultiScale(grey, objects, 1.16, 1, 0);

	for (int i = 0; i < objects.size(); ++i) {
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
			crop.setFromCenter(crop.getCenter(), 200, 250);
			face.crop(crop.x, crop.y, crop.width, crop.height);

			contourMask.begin();
			ofClear(200, 255, 255, 255);
			//face.draw(face.getWidth()/2, face.getHeight()/2);
			face.draw(0, 0);
			contourMask.end();
		}
	}
}





void ofApp::faceDetectDraw() {

	// classifier
	for (int i = 0; i < objects.size(); ++i) {
		ofImage face;
		face.clone(frame);
		ofRectangle crop = ofRectangle(objects[i].x, objects[i].y, objects[i].width, objects[i].height);
		crop.x /= camProxySize;
		crop.y /= camProxySize;
		crop.scale(1 / camProxySize);

		ofSetColor(0, 0, 0);
		ofDrawBitmapString(match[i], i * 100 + 1, camH + 20 + 1);
		ofDrawBitmapString(confidence[i], i * 100 + 1, camH + 40 + 1);

		ofSetColor(255, 0, 0);
		ofDrawBitmapString(match[i], i * 100, camH + 20);
		ofDrawBitmapString(confidence[i], i * 100, camH + 40);
		ofSetColor(255, 255, 255);

		face.crop(crop.x, crop.y, crop.width, crop.height);
		face.resize(100, 100);
		face.draw(i * 100, camH + 50);

		ofNoFill();
		ofSetColor(0, 0, 255);
		ofDrawRectangle(crop);
		ofSetColor(255, 255, 255);
	}
	ofSetLineWidth(1);


	for (int i = 0; i<ofFaces.size(); i++) {
		ofFaces[i].draw(i * 100, camH + 150);
		ofDrawBitmapString(i, i * 100, camH + 170);
	}

}






//--------------------------------------------------------------
void ofApp::drawWithNormals(const ofPolyline& polyline) {
	for (int i = 0; i< (int)polyline.size(); i++) {
		bool repeatNext = i == (int)polyline.size() - 1;

		const ofPoint& cur = polyline[i];
		const ofPoint& next = repeatNext ? polyline[0] : polyline[i + 1];

		float angle = atan2f(next.y - cur.y, next.x - cur.x) * RAD_TO_DEG;
		float distance = cur.distance(next);

		if (repeatNext) {
			ofSetColor(255, 0, 255);
		}
		ofPushMatrix();
		glTranslatef(cur.x, cur.y, 0);
		ofRotate(angle);
		ofDrawLine(0, 0, 0, distance);
		ofDrawLine(0, 0, distance, 0);
		glPopMatrix();
	}
}

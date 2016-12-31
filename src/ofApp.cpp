#include "ofApp.h"





using namespace ofxCv;
using namespace cv;







//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setup() {
#ifdef TARGET_OPENGLES
	shader.load("shadersES2/shader");
#else
	if (ofIsGLProgrammableRenderer()) {
		shader.load("shadersGL3/shader");
	}
	else {
		shader.load("shadersGL2/shader");
	}
#endif


	camW = 640;
	camH = 480;
	//ofSetFrameRate(10);
	camProxySize = 0.25;
	cam.initGrabber(camW, camH);

	threshold = 90;
	cvImgColor.allocate(camW, camH);
	cvImgGrayscale.allocate(camW, camH);

	smooth = 2;

	//faceDetectSetup();



	personCanvas.allocate(camW, camH, GL_RGBA);
	img1.allocate(camW, camH, OF_IMAGE_COLOR);

	img1.loadImage("testFace1.jpg");
	img2.loadImage("testFace2.jpg");
	img1.resize(camW, camH);
	img2.resize(camW, camH);


	//person someone = person();
}















//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::update() {


	cam.update();
	if (cam.isFrameNew()) {

		//ofxCv::convertColor(cam, frame, CV_RGB2GRAY);
		frame = cam.getPixels();

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



		// contours
		cvImgColor.setFromPixels(cam.getPixels().getData(), camW, camH);
		cvImgGrayscale.setFromColorImage(cvImgColor);
		cvImgGrayscale.blur(11);

		cvImgGrayscale.threshold(threshold, true);
		//cvImgGrayscale.adaptiveThreshold(64, threshold, 0);
		cvImgGrayscale.dilate();
		cvImgGrayscale.erode();


		contourFinder.findContours(cvImgGrayscale, 64 * 64, camW * camH, 2, false, true);










		polylines.clear();
		smoothed.clear();
		resampled.clear();
		boundingBoxes.clear();
		paths.clear();

		for (unsigned int i = 0; i < contourFinder.blobs.size(); i++) {
			ofPolyline cur;
			// add all the current vertices to cur polyline
			cur.addVertices(contourFinder.blobs[i].pts);
			cur.setClosed(true);
			cur = cur.getSmoothed(smooth);

			// add the cur polyline to all these vector<ofPolyline>
			polylines.push_back(cur);
			//smoothed.push_back(cur.getSmoothed(8));
			//resampled.push_back(cur.getResampledByCount(100).getSmoothed(8));
			//boundingBoxes.push_back(cur.getBoundingBox());


			ofPath newPath;
			for (int i = 0; i < cur.getVertices().size(); i++) {
				if (i == 0) {
					newPath.newSubPath();
					newPath.moveTo(cur.getVertices()[i]);
				}
				else {
					newPath.lineTo(cur.getVertices()[i]);
				}
			}

			newPath.close();
			newPath.simplify();
			newPath.setFillColor(ofColor(0, 0, 100, 255));

			paths.push_back(newPath);
		}



		// update personCanvas

		personCanvas.begin();
		ofClear(0);

		ofSetColor(ofColor::black);
		for (unsigned int i = 0; i < paths.size(); i++) {
			//paths[i].setColor(ofColor(0, 100, 0, 255));
			paths[i].setFillColor(ofColor(255));
			paths[i].draw();
		}

		//ofSetColor(ofColor::red);
		//ofFill();
		//for (unsigned int i = 0; i < polylines.size(); i++) {
		//	polylines[i].draw();
		//}


		personCanvas.end();



		//		super slow
		//		ofPixels personCanvasPxl;
		//		personCanvas.readToPixels(personCanvasPxl,0);
		//		for (int p = 0; p < personCanvasPxl.size(); ++p) {
		//			ofColor pixel = personCanvasPxl.getColor(p);
		////			pixel.invert();
		////			personCanvasPxl.setColor(p, pixel);
		//		}



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
	//ofClear(150, 150, 150, 0);
	ofSetColor(ofColor::white);
	ofSetLineWidth(3);
	if (frame.isAllocated())
	{
		frame.draw(0, 0);
		ofDrawBitmapString("Frame comped", 0, 10);
	}

	if (frameCompute.isAllocated())
	{
		frameCompute.draw(camW, camH - frameCompute.getHeight());
		ofDrawBitmapString("Frame to compute", camW, camH - frameCompute.getHeight() + 10);
	}


	//faceDetectDraw();


	//personCanvas.draw(camW + 150, 0);



	// move to the right
	ofPushMatrix();
	ofTranslate(camW, 0);
	ofSetColor(ofColor::darkGreen);
	cvImgGrayscale.draw(0, 0);
	//	contourFinder.draw();

	for (unsigned int i = 0; i < polylines.size(); i++) {
		ofNoFill();

		ofSetColor(255, 255, 255, 255);
		polylines[i].draw();
		/*drawWithNormals(polylines[i]);

		ofSetColor(0, 255, 0);
		drawWithNormals(smoothed[i]);

		ofSetColor(0, 0, 255);
		drawWithNormals(resampled[i]);

		ofSetColor(0, 255, 255);
		ofDrawRectangle(boundingBoxes[i]);*/

	}

	ofPopMatrix();

	//personCanvas.draw(0,camH);
	// draw CV image
	//ofPushMatrix();
	//ofTranslate(camW, camH);
	//cvImgColor.draw(camW, camH);
	//cvImgColor2.draw(0, camH);
	//img1.draw(0, camW * 2);





	ofSetColor(255);
	ofFill();
	ofPushMatrix();
	ofScale(2, 2);
	shader.begin();
	// use as a tex0 the same image you are drawing below
	shader.setUniformTexture("tex0", personCanvas.getTexture(), 0);
	shader.setUniformTexture("tex1", cam.getTexture(), 1);
	shader.setUniformTexture("tex2", img2.getTexture(), 2);

	personCanvas.draw(0, 0);


	shader.end();
	ofPopMatrix();



	
	for (int p = 0; p < we.size(); ++p) {
		we[p].draw();
	}


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

	if (key == 'd') {
		//		cout << model->getAlgorithm() << endl;
	}

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

	person someoneNew = person(mouseX, mouseY);
	we.push_back(someoneNew);

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
//	Various functions






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

			personCanvas.begin();
			ofClear(200, 255, 255, 255);
			//face.draw(face.getWidth()/2, face.getHeight()/2);
			face.draw(0, 0);
			personCanvas.end();
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

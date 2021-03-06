#include "ofApp.h"



using namespace ofxCv;
using namespace cv;







//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::setup() {


	

	// Create Manager
	//manage = manager();
	// Load background from disk if exists
	if (ofFile::doesFileExist("background.jpg", true)) {
		ofImage background;
		background.loadImage("background.jpg");
		background.resize(camW, camH);
		manage.setBg(background);
		cout << "Background loaded from disk" << endl;
	}

	//ofSetFrameRate(10);

	
	//cam.setDeviceID(2);
	cam.initGrabber(camW, camH);
	camHacked.allocate(camW, camH);




	//samplePeople
	selectSamplePerson = -1;
	uCount = 0;
	fScale = 0.75;
	ofDirectory sampleFaceDir("");
	sampleFaceDir.listDir();
	for (int i = 0; i < sampleFaceDir.size(); i++) {
		string path = sampleFaceDir.getPath(i);
		// Use only directories starting with f_
		if (path.substr(0, 2).compare("f_") == 0) {
			cout << "Loading sample images from : "<< path << endl;

			vector<ofImage> onePerson;
			ofDirectory dir(path);
			//dir.allowExt("tif");
			dir.listDir();
			for (int p = 0; p < dir.size(); p++) {
				ofImage newPhoto;
				cout << "Loading sample image : " << dir.getPath(p) << endl;
				newPhoto.loadImage(dir.getPath(p));
				onePerson.push_back(newPhoto);
			}
			samplePeople.push_back(onePerson);
		}
	}


	// Doesn't grab anything, cam is still empty, kind of works
	//manage.setBg(cam);

	mouseLB_Pressed = false;



	////////////
	// GUI

	showGui = false;
	gui.setup(); // most of the time you don't need a name
	//gui.setDefaultWidth(500);
	gui.add( winSize.setup("Win size", ofToString(ofGetWidth()) + "x" + ofToString(ofGetHeight())));
	gui.add( camSize.setup("Cam size", ofToString(camW) + "x" + ofToString(camH)));
	gui.add( crowdSize.setup("Crowd size", ofToString(-1) ));
	

	
	guiGrpTrackers.setup();
	guiGrpTrackers.setName("Trackers");
	guiGrpTrackers.setBorderColor(ofColor::lightGreen);
	guiGrpTrackers.add( guiTrackerWidth.setup("Trkr Img Width", 128, 8, 512));
	guiGrpTrackers.add( guiSpeedThress.setup("Trkr Thress", 0.01, 0, 0.1));
	guiGrpTrackers.add( guiTrackerMaxSize.setup("Trkr Max Size", 0.8, 0, 1));
	guiGrpTrackers.add( guiTrackerMinSize.setup("Trkr Min Size", 0.1, 0, 1));
	guiGrpTrackers.add( guiConfidenceThress.setup("Confidence Thress", 1000, 0, 2000));

	gui.add( &guiGrpTrackers );
	//guiGrpTrackers.minimize();


	
	guiGrpPortrain.setup();
	guiGrpPortrain.setName("Portrait");
	guiGrpPortrain.setBorderColor(ofColor::lightBlue);
	guiGrpPortrain.add( guiLumaKey.setup("key", 0.2, 0, 1));
	guiGrpPortrain.add( guiContourImgScale.setup("Contour Img scale", 0.25, 0.01, 1));
	guiGrpPortrain.add( guiContourImgSimplify.setup("Simplify Contour Img", 0, 0, 10));
	guiGrpPortrain.add( guiContourSmooth.setup("Contour smooth", 2, 0, 100));
	
	gui.add(&guiGrpPortrain);
	//guiGrpPortrain.minimize();


	guiGrpDebug.setup();
	guiGrpDebug.setName("Debuging");
	guiGrpDebug.setBorderColor(ofColor::salmon);
	guiGrpDebug.add( guiDebugSplit.setup("Split screen", 0.5, 0, 1));
	guiGrpDebug.add( guiDebugTrackers.setup("Debug Trackers", true));
	guiGrpDebug.add( guiDebugPortrait.setup("Debug Portrait", false));
	guiGrpDebug.add( guiDebugPeople.setup("Debug Portrait", false));
	guiGrpDebug.add( guiDebugUpdateEvidence.setup("Update Evidence", false));

	gui.add(&guiGrpDebug);
	//guiGrpDebug.minimize();
	

	guiGrpCurate.setup();
	guiGrpCurate.setName("Curate");
	guiGrpCurate.setBorderColor(ofColor::green);
	guiGrpCurate.add(guiPersonSizeMin.setup("Min size div", 4, 1, 10));
	guiGrpCurate.add(guiPersonSizeMax.setup("Max size div", 6, 1, 10));
	guiGrpCurate.add( guiAnimationInterval.setup("Animation", 0.1, 0, 1));

	gui.add(&guiGrpCurate);
	//guiGrpCurate.minimize();


	gui.saveToFile("gui.xml");
	
	

	cout << "Initialised" << endl;
}















//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::update() {

	// gui
	manage.debugTrackers = guiDebugTrackers;
	manage.debugPortrait = guiDebugPortrait;
	manage.debugPeople = guiDebugPeople;
	manage.debugUpdateEvidence = guiDebugUpdateEvidence;


	cam.update();
	if (cam.isFrameNew()) {

		// Hack the camera and add test faces, only during debugging or test face
		if (manage.debugTrackers && selectSamplePerson!=-1) {
			camHacked.begin();
			ofClear(0);
			cam.draw(0, 0);
		
			// Add a sample person on the mouse cursor
			if (selectSamplePerson >= 0) {
				ofImage extraFace = samplePeople[selectSamplePerson][uCount++ % samplePeople[selectSamplePerson].size()];
				ofPushMatrix();
				// move to mouse but counteract the scaling of the debug draw
				ofTranslate(mouseX/manage.debugTrackersScale , mouseY/manage.debugTrackersScale);
				ofScale(fScale, fScale);
				extraFace.draw(-extraFace.getWidth()*0.5, -extraFace.getHeight()*0.35);
				ofPopMatrix();
			}
			camHacked.end();

			ofImage bridge;
			camHacked.readToPixels(bridge.getPixelsRef());
			bridge.update();

			manage.detectFaces( bridge );
		}
		else {
			manage.detectFaces(cam);
		}



		//	Framerates
		std::stringstream strm;
		strm << "Draw FPS: " << int(ofGetFrameRate()) << "   | Update FPS: " << int(1/(ofGetElapsedTimef()- lastUpdate)) << "   | clock: " << int(ofGetElapsedTimef());
		ofSetWindowTitle(strm.str());
		lastUpdate = ofGetElapsedTimef();
	}
}










//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::draw() {
	
	manage.curate();
	manage.draw();



	if (manage.debugPortrait)
	{
		ofImage camFrame;
		camFrame.setFromPixels(cam.getPixels());
		manage.drawDebugPortrait(camFrame);

		// direct draw portrait
		//ofImage camFrame;
		//camFrame.setFromPixels(cam.getPixels());
		//ofRectangle crop = ofRectangle(0, 0, camW, camH);
		//manage.makePortrait(camFrame, crop, shdPrepThress).draw(0,camH);
	}

	if (manage.debugTrackers){
		manage.drawDebugTrackers();
	}



	// display RGB values below the mouse
	if (mouseLB_Pressed) {
		unsigned char color[3];
		glReadPixels(mouseX, ofGetHeight() - mouseY, 1, 1, GL_RGB, GL_UNSIGNED_BYTE, color);
		
		std::stringstream rgbValues;
		rgbValues << int(color[0]) << " " << int(color[1]) << " " << int(color[2]) ;
		ofDrawBitmapStringHighlight(rgbValues.str(), mouseX+10, mouseY+30, ofColor::black, ofColor::white);
	}


	// REFERENCE
	// From ofImage to Mat and back
	if( false ){
		ofImage camFrame;
		camFrame.draw(750, 0);
		ofPixels toMatPixels = camFrame.getPixels();
		Mat color = ofxCv::toCv(toMatPixels);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);

		ofImage ofMat;
		ofxCv::toOf(grey, ofMat);
		ofMat.update();
		ofMat.draw(1200, 0);
	}
	/////////////





	// GUI
	if( showGui )
		gui.draw();


	
}

























//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

	if (key == 'b')
	{
		ofImage background;
		background.setFromPixels(cam.getPixels());
		background.saveImage("background.jpg");
		manage.setBg(background);
		cout << "Background Saved to disk" << endl;
	}

	if (key == OF_KEY_LEFT) {
		manage.selection = ofClamp(manage.selection - 1, -1, manage.we.size() - 1);
		cout << "Selection " << manage.selection << endl;
	}

	if (key == OF_KEY_RIGHT) {
		manage.selection = ofClamp(manage.selection + 1, -1, manage.we.size() - 1);
		cout << "Selection " << manage.selection << endl;
	}

	if (key == 'd')
	{
		manage.remove();
		cout << "Removed person " << manage.selection << endl;
	}

	if (key == 'c')
	{
		manage.forgetUs();
		cout << "People cleared" << endl;
	}
	if (key == 'C')
	{
		manage.candidates.setPersistence(0);
		cout << "Forget trackers" << endl;
	}

	if (key == '`') {
		showGui = !showGui;
	}


	// Debug face Trackers
	if (key == '1') {
		manage.debugTrackers = !manage.debugTrackers;
		guiDebugTrackers = !guiDebugTrackers;
		cout << "Debug tracking :" << manage.debugTrackers << endl;
	}
	// Debug Portrait creation
	if (key == '2') {
		manage.debugPortrait = !manage.debugPortrait;
		guiDebugPortrait = !guiDebugPortrait;
		cout << "Debug Mode" << manage.debugPortrait << endl;
	}
	// Debug People
	if (key == '3') {
		manage.debugPeople = !manage.debugPeople;
		guiDebugPeople = !guiDebugPeople;
		cout << "debugPeople :" << manage.debugPeople << endl;
	}
	// Debug People
	if (key == '4') {
		manage.debugUpdateEvidence = !manage.debugUpdateEvidence;
		guiDebugUpdateEvidence = !guiDebugUpdateEvidence;
		cout << "debugUpdateEvidence :" << manage.debugUpdateEvidence << endl;
	}


	// info
	if (key == 'i') {
		manage.info();
	}

	// camera settings
	if (key == 'v' || key == 'V') {
		cam.videoSettings();
	}

	// saveUs
	if (key == 's' || key == 'S') {
//		manage.saveUs(key == 's');
		manage.saveUs(true);
	}


	// loadUs
	if (key == 'l' || key == 'L') {
		manage.loadUs();
		if(key == 'l')
			manage.model.train();
	}

	// Make portraits with Alpha channel
	if (key == 'a') {
		manage.portraitWithAlpha = !manage.portraitWithAlpha;
		cout << "portraitWithAlpha :" << manage.portraitWithAlpha << endl;
	}


	if (key == OF_KEY_F8) {
		manage.model.save();
	}
	if (key == OF_KEY_F5) {
		manage.model.load();
	}


	if (key == 'F')
		ofToggleFullscreen();


	/////// temp keys

	if (key == ',' ) {
		selectSamplePerson = ofClamp(--selectSamplePerson, -1, samplePeople.size() - 1);
		cout << "Select Sample Person : " << selectSamplePerson << endl;
	}
	if (key == '.' ) {
		selectSamplePerson = ofClamp(++selectSamplePerson, -1, samplePeople.size() - 1);
		cout << "Select Sample Person : " << selectSamplePerson << endl;
	}


	if (key == '+') {
		fScale = ofClamp(fScale+0.1, 0, 2);
		cout << "fScale :" << fScale << endl;
	}
	if (key == '-') {
		fScale = ofClamp(fScale-0.1, 0, 2);
		cout << "fScale :" << fScale << endl;
	}

	if (key == 'X') {
		manage.model.test();
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
	
	mouseLB_Pressed = true;

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
	mouseLB_Pressed = false;

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	winSize = ofToString(w) + "x" + ofToString(h);
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





// Make the faceBounds bigger for better cropping
ofRectangle adjustFaceBounds(ofRectangle faceBounds) {

	faceBounds.scaleFromCenter(1.5, 2.2);
	faceBounds.translateY(faceBounds.getHeight()*0.05);

	// make sure the new bounds are not getting outside the camera image
	faceBounds.setX(ofClamp(faceBounds.x, 0, camW));
	faceBounds.setY(ofClamp(faceBounds.y, 0, camH));
	faceBounds.setWidth(ofClamp(faceBounds.getWidth(), 0, camW - faceBounds.x));
	faceBounds.setHeight(ofClamp(faceBounds.getHeight(), 0, camH - faceBounds.y));

	return faceBounds;
}

// remap the coordinates of the small snapshot crop to the bigger frame crop
ofRectangle remapSnapshotCrop(ofRectangle faceBound, ofRectangle newRef) {
	faceBound.setX(faceBound.getX() - newRef.getX());
	faceBound.setY(faceBound.getY() - newRef.getY());
	return faceBound;
}



// Get the time difference between the refTime and current time with set precision
float getTimeDiff(float refTime) {
	return roundf((ofGetElapsedTimef() - refTime) * 100000) / 100;
}




string padThis(int pads, int number) {
	string out;
	string strNum = ofToString(number);

	for (int i = 0; i < pads - strNum.length(); i++) {
		out += "0";
	}
	return out + strNum;
}











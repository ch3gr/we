#include "manager.h"

using namespace ofxCv;


manager::manager(){
}

manager::manager(int _camW, int _camH)
{
#ifdef TARGET_OPENGLES
	shdPrep.load("shaders/ES2/prep");
	shdComp.load("shaders/ES2/comp");
#else
	if (ofIsGLProgrammableRenderer()) {
		shdPrep.load("shaders/GL3/prep");
		shdComp.load("shaders/GL3/comp");
	}
	else {
		shdPrep.load("shaders/GL2/prep");
		shdComp.load("shaders/GL2/comp");
	}
#endif



	camW = _camW;
	camH = _camH;
	fDetectW = 256;
	cDetectW = camW*0.5;
	
	previewScale = 0.333;

	nextPersonId = 0;
	canvas.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugPortrait.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugTrackers.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugTimers.allocate(ofGetWindowWidth(), ofGetWindowHeight());

	scout.setup("haarcascade_frontalface_default.xml");
//	scout.setup("haarcascade_eye.xml");
	scout.setPreset(ObjectFinder::Fast);
	//scout.setPreset(ObjectFinder::Accurate);
	//scout.setPreset(ObjectFinder::Sensitive);
	scout.setRescale( fDetectW/float(camW) );
	scout.setMinSizeScale(0.1);
	scout.setMaxSizeScale(0.8);

	candidates.setMaximumDistance(camW / 5);
	//candidates.setPersistence(10);

}


manager::~manager()
{
}


void manager::setBg(ofImage _bg) {
	bg = _bg;
}

void manager::setBg(ofVideoGrabber _cam) {
	bg.setFromPixels(_cam.getPixels());
}


void manager::addPerson(ofImage _face, int _x, int _y) {
	person someoneNew = person(nextPersonId++, _face, _x, _y);
	we.push_back(someoneNew);
}


void manager::curate() {
	int size = we.size();

	// spread them at the bottom of the half window
	for (int p = 0; p < size; ++p) {
		int x = (1-(float(p+1) / float(size))) * ofGetWidth();
		
		int yMax = ofGetHeight()*0.5;
		int yMin = ofGetHeight() - we[p].face.getHeight();

		ofSeedRandom(we[p].id);
		float wave = cos(ofGetElapsedTimef() + ofRandom(1000))*0.5 +0.5;
		int y = ofLerp(yMin, yMax, wave);

		we[p].setPos(x, y);
		
		// debug some values
		//if (p == 0)
		//	cout << we[p].id << endl;
	}
	
	
	
}


void manager::update() {
	curate();
	for (int p = 0; p < we.size(); ++p)
		we[p].update();
}



void manager::draw() {
	update();

	canvas.begin();
	ofClear(0);

	for (int p = 0; p < we.size(); ++p)
		we[p].draw();

	canvas.end();
	canvas.draw(0, 0);

	//debugTimers.draw(0, 0);

}









void manager::drawDebug(ofImage camFrame) {

	vector<candidate> & followers = candidates.getFollowers();
	if (followers.size() > 0) {
		ofRectangle someone = followers[0].faceBounds;
		makePortrait(camFrame, someone, 0.1);
	}

	debugPortrait.draw(ofGetWidth()-camW, 0);
}

void manager::drawDebugTrackers(){
	debugTrackers.draw(0, 0);
}

void manager::info() {
	// print people
	cout << "I am a manager and these are my people :" << endl;
	for (int p = 0; p < we.size(); ++p)
		we[p].info();


	// candidates
	vector<candidate> & followers = candidates.getFollowers();
	for (int i = 0; i < followers.size(); i++) {
		cout << "I am follower : " << followers[i].getLabel()  << endl;
	}

	
}


void manager::clearPeople() {
	we.clear();
}












void manager::detectFaces(ofImage cam) {
	
	// Timer
	std::stringstream ss;
	ss << "detectFaces" << endl;
	float timerDetectFaces = ofGetElapsedTimef();

	if (!bg.isAllocated())
		setBg(cam);


	cam.resize(cam.getWidth(), cam.getHeight());

	// timer
	ss << getTimeDiff(timerDetectFaces) << " start" << endl;


	scout.update(cam);

	// timer
	ss << getTimeDiff(timerDetectFaces) << " update" << endl;



	// Get the tracker
	//RectTracker tracker = scout.getTracker();
	//tracker.setMaximumDistance(100);
	//tracker.setPersistence(30);
	//candidates.setPersistence( ofGetFrameRate() * 0.5 );

	candidates.track(scout.getObjects());

	vector <candidate> candidatesList = candidates.getFollowers();
	for (int c = 0; c < candidatesList.size(); c++) {
		if (candidatesList[c].trigger) {
			ofImage portrait = makePortrait(cam, candidatesList[c].faceBounds, 0.1);
			addPerson(portrait, ofRandom(ofGetWidth()), ofRandom(ofGetHeight()));
		}
	}

	// timer
	ss << getTimeDiff(timerDetectFaces) << " get candidates" << endl;



	ofPushMatrix();
	debugTrackers.begin();
	ofClear(0);
	cam.draw(0, 0);

	// Draw the face trackers


	// timer
	ss << getTimeDiff(timerDetectFaces) << " Start drawing trackers" << endl;

	//// Only needed for debuging
	for (int i = 0; i < scout.size(); i++) {
		ofRectangle faceBounds = scout.getObject(i);
		ofPushStyle();
		ofNoFill();
		ofSetLineWidth(5);
		ofSetColor(ofColor::white);
		ofDrawRectangle(faceBounds);
		// adjust face
		faceBounds = adjustFaceBounds(faceBounds, camW, camH);
		// Draw captured face
		ofImage portrait;
		portrait.clone(cam);
		portrait.crop(faceBounds.x, faceBounds.y, faceBounds.width, faceBounds.height);
		portrait.resize(100, 100);
		ofPushMatrix();
		ofTranslate(cam.getWidth(), 0);
		portrait.draw(100 * i, 0);
		ofPopMatrix();
		ofPopStyle();
	}

	// timer
	ss << getTimeDiff(timerDetectFaces) << " End drawing trackers" << endl;
	
	vector<candidate> & followers = candidates.getFollowers();
	for (int i = 0; i < followers.size(); i++) {
		followers[i].draw();
		
	}

	// timer
	ss << getTimeDiff(timerDetectFaces) << " End drawing Candidates" << endl;
	

	debugTrackers.end();
	ofPopMatrix();

	
	// timer
	debugTimers.begin();
	ss << getTimeDiff(timerDetectFaces) << " end";
	ofDrawBitmapStringHighlight(ss.str(), 10, 20, ofColor::black, ofColor::red);
	debugTimers.end();
	////////


}

void manager::detectFaces(ofVideoGrabber cam) {
	ofImage camFrame;
	camFrame.setFromPixels(cam.getPixels());
	detectFaces(camFrame);
}

















ofImage manager::makePortrait( ofImage camFrame, ofRectangle faceBounds, float shdPrepThress) {

	// Timer
	std::stringstream ss;
	ss << "MakePortrait" << endl;
	float timerDetectFaces = ofGetElapsedTimef();
	/////////

	float cScale = cDetectW / float(camW);

	faceBounds = adjustFaceBounds(faceBounds, camW, camH);

	debugPortrait.begin();
	ofClear(0);


	//////////////////////
	// debugPortrait Draw cam
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	camFrame.draw(0, 0);
	ofPopMatrix();
	// -----------------------

	// debugPortrait Draw BG
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	bg.draw(camW, 0);		// to debugPortrait buffer
	ofPopMatrix();
	// -----------------------


	// timer
	ss << getTimeDiff(timerDetectFaces) << " draw cam and BG" << endl;


	//////////////////////
	// Temp Draw face mask - will be replace with faceTracker
	ofFbo faceMask;
	faceMask.allocate(camW, camH, GL_RGBA);
	//faceMask.begin();
	//ofClear(0);
	//ofSetColor(ofColor::white);
	//ofDrawCircle(100, 100, 70);
	//faceMask.end();

	// debugPortrait Draw faceMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	faceMask.draw(camW*2, 0);
	ofPopMatrix();
	// -----------------------


	// timer
	ss << getTimeDiff(timerDetectFaces) << " temp draw mask" << endl;


	//////////////////////
	// Face detection
	// Draw a white square that covers the bounding box of the face
	ofFbo faceBoundsFBO;
	faceBoundsFBO.allocate(camW, camH, GL_RGBA);
	faceBoundsFBO.begin();
	ofClear(0);
	ofSetColor(ofColor::white);
	ofDrawRectangle(faceBounds);
	faceBoundsFBO.end();

	// debugPortrait Draw FaceDetectMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	faceBoundsFBO.draw(0, camH);
	ofPopMatrix();
	// -----------------------


	// timer
	ss << getTimeDiff(timerDetectFaces) << " Face detection" << endl;



	////////////////////
	// Prep Render

	ofFbo prep;
	prep.allocate(camW*cScale, camH*cScale, GL_RGBA);
	prep.begin();
	shdPrep.begin();
	// use as a tex0 the same image you are drawing below
	shdPrep.setUniformTexture("tex0", camFrame.getTexture(), 0);
	if (bg.isAllocated())
		shdPrep.setUniformTexture("tex1", bg.getTexture(), 1);
	shdPrep.setUniformTexture("tex2", faceBoundsFBO.getTexture(), 2);
	//shdPrep.setUniformTexture("tex3", faceMask.getTexture(), 3);
	shdPrep.setUniform1f("thress", shdPrepThress);

	// Render Prep
	ofPushMatrix();
	ofScale(cScale, cScale);
	camFrame.draw(0, 0);
	ofPopMatrix();

	shdPrep.end();
	prep.end();

	// debugPortrait Draw FaceDetectMask
	ofPushMatrix();
	//ofScale(1 / cScale, 1 / cScale);
	ofScale(previewScale, previewScale);
	//ofTranslate(camW, camH);
	prep.draw(camW, camH);
	ofPopMatrix();
	// -----------------------
	

	// timer
	ss << getTimeDiff(timerDetectFaces) << " Prep render" << endl;

	
	

	//////////////////////
	// Find contrours on the binary image
	//////////////////////
	// prep contours
	// Pass Preped buffer to cvImage and detect contours


	ofImage grabPrep;
	prep.begin();
	grabPrep.grabScreen(0,0,prep.getWidth(), prep.getHeight());
	prep.end();

		// timer
		ss << getTimeDiff(timerDetectFaces) << " FBO grab image" << endl;
	//grabPrep.resize(camW*cScale, camH*cScale);

		// timer
		ss << getTimeDiff(timerDetectFaces) << " FBO resize" << endl;


	ofxCvGrayscaleImage cvImgGrayscale;
	cvImgGrayscale.allocate(grabPrep.getWidth(), grabPrep.getHeight());

		ss << getTimeDiff(timerDetectFaces) << " cvAlocate" << endl;

	cvImgGrayscale.setFromPixels(grabPrep.getPixels().getChannel(0));

		ss << getTimeDiff(timerDetectFaces) << " cvGrey" << endl;

	//cvImgGrayscale.resize(camW*cScale, camH*cScale);

		ss << getTimeDiff(timerDetectFaces) << " cvResize" << endl;
	

	//cvImgGrayscale.dilate();
	//cvImgGrayscale.dilate();
	//cvImgGrayscale.erode();
	//cvImgGrayscale.erode();

	// timer
	ss << getTimeDiff(timerDetectFaces) << " cvDilate" << endl;

	// calc contours
	ofxCvContourFinder contourFinder;
	contourFinder.findContours(cvImgGrayscale, ((camW*cScale*0.2)*(camH*cScale*0.2)), ((camW*cScale)*(camH*cScale)), 5, false, true);

	

	// timer
	ss << getTimeDiff(timerDetectFaces) << " Find contours" << endl;


	// populate arrays of contours
	vector<ofPath> controurSurfaces;
	for (unsigned int i = 0; i < contourFinder.blobs.size(); i++) {
		// add all the current vertices to a polyLine
		ofPolyline outline;
		outline.addVertices(contourFinder.blobs[i].pts);
		outline.setClosed(true);
		outline = outline.getSmoothed(2);

		ofPath newPath;
		for (int i = 0; i < outline.getVertices().size(); i++) {
			if (i == 0) {
				newPath.newSubPath();
				newPath.moveTo(outline.getVertices()[i]);
			}
			else
				newPath.lineTo(outline.getVertices()[i]);
		}
		newPath.close();
		newPath.simplify();
		// scale path back to the original image of the camera and flip it to convert from texture coord to ofPixel space or something
		newPath.scale(1/cScale, -1/cScale);
		newPath.translate(ofPoint(0, camH));

		controurSurfaces.push_back(newPath);
	}


	//////////////////////
	// create countourMask
	ofFbo contourMask;
	contourMask.allocate(camW, camH, GL_RGBA);
	contourMask.begin();
	ofPushMatrix();
	ofClear(0);
	ofSetColor(ofColor::black);
	for (unsigned int i = 0; i < controurSurfaces.size(); i++) {
		controurSurfaces[i].setFillColor(ofColor(255));
		controurSurfaces[i].draw();
	}
	ofPopMatrix();
	contourMask.end();

	// debugPortrait Draw countourMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	contourMask.draw(camW*2, 0);
	ofPopMatrix();
	// -----------------------


	// debugPortrait Draw countour lines
	ofSetColor(ofColor::red);
	ofSetLineWidth(3);
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	ofTranslate(camW*2 , 0);

	for (int i = 0; i < controurSurfaces.size(); i++) {
		vector <ofPolyline> outlines = controurSurfaces[i].getOutline();
		for (int i = 0; i < outlines.size(); i++)
			outlines[i].draw();
	}
	ofPopMatrix();
	ofSetColor(ofColor::white);
	// -----------------------



	// timer
	ss << getTimeDiff(timerDetectFaces) << " Draw contour" << endl;





	// Composite
	ofFbo comp;
	comp.allocate(camW, camH, GL_RGBA);
	comp.begin();
	ofClear(0);
	shdComp.begin();
	// use as a tex0 the same image you are drawing below
	shdComp.setUniformTexture("tex0", contourMask.getTexture(), 0);
	shdComp.setUniformTexture("tex1", camFrame.getTexture(), 1);
	//shdComp.setUniformTexture("tex2", cam.getTexture(), 2);

	contourMask.draw(0, 0);
	shdComp.end();
	comp.end();

	// debugPortrait Draw countourMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	comp.draw(camW * 2, camH);
	ofPopMatrix();
	// -----------------------


	debugPortrait.end();

	ofImage fboImage;
	comp.readToPixels(fboImage.getPixelsRef());
	ofImage out;
	out.clone(fboImage);
	
	//cout << faceBounds.x << " " << faceBounds.y << " " << faceBounds.getWidth() << " " << faceBounds.getHeight() << endl;
	out.crop(faceBounds.x, faceBounds.y, faceBounds.getWidth(), faceBounds.getHeight());





	// timer
	debugTimers.begin();
	ss << getTimeDiff(timerDetectFaces) << " end";
	ofDrawBitmapStringHighlight(ss.str(), 10, 250, ofColor::black, ofColor::red);
	debugTimers.end();
	////////




	return out;
}











// Make the faceBounds bigger for better cropping
ofRectangle adjustFaceBounds(ofRectangle faceBounds, int camW, int camH) {
	
	faceBounds.scaleFromCenter(2, 2);
	faceBounds.translateY(faceBounds.getHeight()*0.1);

	// make sure the new bounds are not getting outside the camera image
	faceBounds.setX(ofClamp(faceBounds.x, 0, camW));
	faceBounds.setY(ofClamp(faceBounds.y, 0, camH));
	faceBounds.setWidth(ofClamp(faceBounds.getWidth(), 0, camW - faceBounds.x));
	faceBounds.setHeight(ofClamp(faceBounds.getHeight(), 0, camH - faceBounds.y));

	return faceBounds;
}


// Get the time difference between the refTime and current time with set precision
float getTimeDiff(float refTime) {
	return roundf((ofGetElapsedTimef() - refTime) * 100000) / 100;
}
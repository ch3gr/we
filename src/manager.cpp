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
	
	previewScale = 0.333;

	nextPersonId = 0;
	canvas.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugPortrait.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugTrackers.allocate(ofGetWindowWidth(), ofGetWindowHeight());

	scout.setup("haarcascade_frontalface_default.xml");
//	scout.setup("haarcascade_eye.xml");
	scout.setPreset(ObjectFinder::Fast);
	//scout.setPreset(ObjectFinder::Accurate);
	//scout.setPreset(ObjectFinder::Sensitive);
	scout.setRescale(0.25);
	scout.setMinSizeScale(0.1);

	
	
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
		int x = (1-(float(p) / float(size-1))) * ofGetWidth();
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
}

void manager::drawDebug() {
	debugPortrait.draw(ofGetWidth()-camW, 0);
	
}

void manager::drawDebugTrackers(){
	debugTrackers.draw(0, 0);
}

void manager::info() {
	cout << "I am a manager and these are my people :" << endl;
	for (int p = 0; p < we.size(); ++p)
		we[p].info();


	// print more info
	RectTracker tracker = scout.getTracker();
	const vector<unsigned int>& deadLabels = tracker.getDeadLabels();
	for (int l = 0; l < deadLabels.size(); l++) {
		cout << "DeadLabel : " << deadLabels[l] << endl;
	}
	
}


void manager::clearPeople() {
	we.clear();
}












void manager::detectFaces(ofImage cam) {

	float s = 1;
	cam.resize(cam.getWidth()*s, cam.getHeight()*s);
	
	float timer = ofGetElapsedTimef();

	scout.update(cam);


	// Get the tracker
	RectTracker tracker = scout.getTracker();
	//tracker.setMaximumDistance(100);
	//tracker.setPersistence(30);
	
	//candidates.track(scout.getLabel());
	// HERE
	//////////////////////////////////////////////////////
	

	const vector<unsigned int>& newLabels = tracker.getNewLabels();
	for (int l = 0; l < newLabels.size(); l++) {
		cout << "New Label : " << newLabels[l] << endl;
	}
	

	ofPushMatrix();
	debugTrackers.begin();
	ofClear(0);
	cam.draw(0, 0);
	scout.draw();
	
	for (int i = 0; i < scout.size(); i++) {
		ofRectangle faceRect = scout.getObject(i);
		int label = scout.getLabel(i);
		float age = tracker.getAge(label);

		cv::Vec2f vel = scout.getVelocity(i);

		ofNoFill();
		//ofDrawRectangle(faceRect);
		ofDrawLine(faceRect.getCenter(), faceRect.getCenter() + ofPoint(vel[0],vel[1]));
		ofFill();
		ofDrawCircle(faceRect.getCenter(), 2);

		// adjust face
		faceRect.scaleFromCenter(2, 2);
		faceRect.translateY(faceRect.getHeight()*0.1);

		//if (faceRect.getRight() > camW)
		//	faceRect.setWidth(camW - faceRect.x);
		//if (faceRect.getBottom() > camH)
		//	faceRect.setHeight(camH - faceRect.y);
		faceRect.setX( ofClamp(faceRect.x, 0, camW) );
		faceRect.setY( ofClamp(faceRect.y, 0, camH) );
		faceRect.setWidth( ofClamp(faceRect.getWidth(), 0, camW - faceRect.x) );
		faceRect.setHeight( ofClamp(faceRect.getHeight(), 0, camH - faceRect.y) );

		// Draw captured face
		ofImage portrait;
		portrait.clone(cam);
		portrait.crop(faceRect.x, faceRect.y, faceRect.width, faceRect.height);
		portrait.resize(200, 200);
		ofPushMatrix();
		ofTranslate(cam.getWidth()*s, 0);
		portrait.draw(200 * i, 0);
		ofDrawBitmapString(label, 200 * i, 10);
		ofDrawBitmapString(age, 200 * i, 40);
		ofPopMatrix();



		// If there is a new label, add a person
		for (int l = 0; l < newLabels.size(); l++) {
			if (label == newLabels[l]) {
				ofImage portrait = makePortrait(cam, faceRect, 0.1);
				addPerson(portrait, ofRandom(ofGetWidth()), ofRandom(ofGetHeight()));
			}
		}
	}
	
	ofDrawBitmapString(ofGetElapsedTimef()-timer, 0, 20);
	
	
	debugTrackers.end();
	ofPopMatrix();

	
	
}

void manager::detectFaces(ofVideoGrabber cam) {
	ofImage camFrame;
	camFrame.setFromPixels(cam.getPixels());
	detectFaces(camFrame);
}

















ofImage manager::makePortrait( ofImage camFrame, ofRectangle framing, float shdPrepThress) {

	if (!bg.isAllocated())
		setBg(camFrame);

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


	//////////////////////
	// Face detection
	//detectFaces(camFrame);
	// Draw a white square that covers the bounding box of the face
	ofFbo faceBoundsFBO;
	faceBoundsFBO.allocate(camW, camH, GL_RGBA);
	faceBoundsFBO.begin();
	ofClear(0);
	ofSetColor(ofColor::white);
	ofDrawRectangle(framing);
	//ofDrawRectangle(faceBounds.getX(), faceBounds.getY(), faceBounds.getWidth(), faceBounds.getHeight());
	faceBoundsFBO.end();

	// debugPortrait Draw FaceDetectMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	faceBoundsFBO.draw(0, camH);
	ofPopMatrix();
	// -----------------------




	////////////////////
	// Prep Render

	ofFbo prep;
	prep.allocate(camW, camH, GL_RGBA);
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
	camFrame.draw(0, 0);

	shdPrep.end();
	prep.end();

	// debugPortrait Draw FaceDetectMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	prep.draw(camW, camH);
	ofPopMatrix();
	// -----------------------




	

	//////////////////////
	// Find contrours on the binary image
	//////////////////////
	// prep contours
	// Pass Preped buffer to cvImage and detect contours

	ofPixels prepedPixels;
	prep.readToPixels(prepedPixels, 0);
	//prepedPixels.crop( faceBounds.getX(), faceBounds.getY(), faceBounds.getWidth(), faceBounds.getHeight() );
	ofxCvGrayscaleImage cvImgGrayscale;
	cvImgGrayscale.allocate(prepedPixels.getWidth(), prepedPixels.getHeight());
	cvImgGrayscale.setFromPixels(prepedPixels.getChannel(0));

	//cvSetImageROI(cvImgGrayscale.getCvImage(), cvRect(faceBounds.getX(), faceBounds.getY(), faceBounds.getWidth(), faceBounds.getHeight()));
	

	cvImgGrayscale.dilate();
	cvImgGrayscale.dilate();
	cvImgGrayscale.erode();
	cvImgGrayscale.erode();

	// calc contours
	ofxCvContourFinder contourFinder;
	contourFinder.findContours(cvImgGrayscale, 64 * 64, camW * camH, 5, false, true);


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
		controurSurfaces.push_back(newPath);
	}


	//////////////////////
	// create countourMask
	ofFbo contourMask;
	contourMask.allocate(camW, camH, GL_RGBA);
	contourMask.begin();
	ofClear(0);
	ofSetColor(ofColor::black);
	for (unsigned int i = 0; i < controurSurfaces.size(); i++) {
		controurSurfaces[i].setFillColor(ofColor(255));
		controurSurfaces[i].draw();
	}
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
	ofTranslate(camW , camH);
	for (int i = 0; i < controurSurfaces.size(); i++) {
		vector <ofPolyline> outlines = controurSurfaces[i].getOutline();
		for (int i = 0; i < outlines.size(); i++)
			outlines[i].draw();
	}
	ofPopMatrix();
	ofSetColor(ofColor::white);
	// -----------------------









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
	
	//cout << framing.x << " " << framing.y << " " << framing.getWidth() << " " << framing.getHeight() << endl;
	out.crop(framing.x, framing.y, framing.getWidth(), framing.getHeight());
	return out;
}
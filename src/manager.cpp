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
	
	previewScale = 0.5;

	nextPersonId = 0;
	canvas.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debug.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugTrackers.allocate(ofGetWindowWidth(), ofGetWindowHeight());

	managerFFinder.setup("haarcascade_frontalface_default.xml");
	managerFFinder.setPreset(ObjectFinder::Fast);
	//managerFFinder.setPreset(ObjectFinder::Accurate);
	//managerFFinder.setPreset(ObjectFinder::Sensitive);
	managerFFinder.setRescale(0.25);
	managerFFinder.setMinSizeScale(0.1);

	
	
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



void manager::update() {
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
	debug.draw(0, 0);
}

void manager::drawDebugTrackers(){
	debugTrackers.draw(0, 0);
}

void manager::info() {
	cout << "I am a manager and these are my people :" << endl;
	for (int p = 0; p < we.size(); ++p)
		we[p].info();
}

void manager::clearPeople() {
	we.clear();
}




void manager::detectFaces(ofImage cam) {

	float s = 1;
	cam.resize(cam.getWidth()*s, cam.getHeight()*s);
	
	float timer = ofGetElapsedTimef();
	managerFFinder.update(cam);
	RectTracker tracker = managerFFinder.getTracker();
	

	const vector<unsigned int>& newLabels = tracker.getNewLabels();
	for (int l = 0; l < newLabels.size(); l++) {
		cout << "New Label : " << newLabels[l] << endl;
	}
	

	ofPushMatrix();
	debugTrackers.begin();
	ofClear(0);
	cam.draw(0, 0);
	//managerFFinder.draw();
	
	for (int i = 0; i < managerFFinder.size(); i++) {
		ofRectangle face = managerFFinder.getObject(i);
		int label = managerFFinder.getLabel(i);
		float age = tracker.getAge(label);

		cv::Vec2f vel = managerFFinder.getVelocity(i);

		ofNoFill();
		ofDrawRectangle(face);
		ofDrawLine(face.getCenter(), face.getCenter() + ofPoint(vel[0],vel[1]));
		ofFill();
		ofDrawCircle(face.getCenter(), 2);

		// adjust face
		face.scaleFromCenter(2, 2);
		face.translateY(face.getHeight()*0.1);

		//if (face.getRight() > camW)
		//	face.setWidth(camW - face.x);
		//if (face.getBottom() > camH)
		//	face.setHeight(camH - face.y);
		face.setX( ofClamp(face.x, 0, camW) );
		face.setY( ofClamp(face.y, 0, camH) );
		face.setWidth( ofClamp(face.getWidth(), 0, camW - face.x) );
		face.setHeight( ofClamp(face.getHeight(), 0, camH - face.y) );

		ofImage portrait;
		portrait.clone(cam);
		portrait.crop(face.x, face.y, face.width, face.height);
		portrait.resize(200, 200);
		portrait.draw(200 * i, 600);
		ofDrawBitmapString(label, 200 * i, 600 + 10);
		ofDrawBitmapString(age, 200 * i, 600 + 40);



		// If there is a new label, add a person
		for (int l = 0; l < newLabels.size(); l++) {
			if (label == newLabels[l]) {
				ofImage portrait = makePortrait(cam, face, 0.1);
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

















ofImage manager::makePortrait( ofImage camFrame, ofRectangle crop, float shdPrepThress) {

	if (!bg.isAllocated())
		setBg(camFrame);

	debug.begin();
	ofClear(0);


	//////////////////////
	// debug Draw cam
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	camFrame.draw(0, 0);
	ofPopMatrix();
	// -----------------------

	// debug Draw BG
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	bg.draw(camW, 0);		// to debug buffer
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

	// debug Draw faceMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	faceMask.draw(camW*2, 0);
	ofPopMatrix();
	// -----------------------



	//////////////////////
	// Face detection
	//detectFaces(camFrame);
	// Draw squares for each face tracked
	ofFbo faceDetectMask;
	faceDetectMask.allocate(camW, camH, GL_RGBA);
	faceDetectMask.begin();
	ofClear(0);
	ofSetColor(ofColor::white);
	for (int i = 0; i < managerFFinder.size(); i++) {
		ofRectangle face = managerFFinder.getObjectSmoothed(i);
		face.scaleFromCenter(2, 2);
		face.translateY(face.getHeight()*0.1);
		ofDrawRectangle(face);
	}
	faceDetectMask.end();

	// debug Draw FaceDetectMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	faceDetectMask.draw(0,camH);
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
	shdPrep.setUniformTexture("tex2", faceMask.getTexture(), 2);
	shdPrep.setUniformTexture("tex3", faceDetectMask.getTexture(), 3);

	shdPrep.setUniform1f("thress", shdPrepThress);

	camFrame.draw(0, 0);
	shdPrep.end();
	prep.end();

	// debug Draw FaceDetectMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	prep.draw(camW, camH);
	ofPopMatrix();
	// -----------------------




	


	//////////////////////
	// prep contours
	// Pass Preped buffer to cvImage and detect contours

	ofPixels prepedPixels;
	prep.readToPixels(prepedPixels, 0);
	ofxCvGrayscaleImage cvImgGrayscale;
	cvImgGrayscale.allocate(camW, camH);
	cvImgGrayscale.setFromPixels(prepedPixels.getChannel(0));

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

	// debug Draw countourMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	contourMask.draw(camW*2, 0);
	ofPopMatrix();
	// -----------------------


	// debug Draw countour lines
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

	// debug Draw countourMask
	ofPushMatrix();
	ofScale(previewScale, previewScale);
	comp.draw(camW * 2, camH);
	ofPopMatrix();
	// -----------------------


	debug.end();

	ofImage out;
	comp.readToPixels(out.getPixelsRef());
	
	out.crop(crop.x, crop.y, crop.getWidth(), crop.getHeight());
	return out;
}
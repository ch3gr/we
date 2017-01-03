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
	nextPersonId = 0;
	canvas.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debug.allocate(ofGetWindowWidth(), ofGetWindowHeight());

	managerFFinder.setup("haarcascade_frontalface_default.xml");
	managerFFinder.setPreset(ObjectFinder::Fast);

	
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
	//ofClear(0);

	for (int p = 0; p < we.size(); ++p)
		we[p].draw();


	canvas.end();

	canvas.draw(0, 0);
}

void manager::drawDebug() {
	ofPushMatrix();
	//ofTranslate(1000, 0);
	ofScale(0.4, 0.4);
	debug.draw(0, 0);
	ofPopMatrix();
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

	managerFFinder.update(cam);

	ofPushMatrix();
	ofTranslate(camW, 0);
	cam.draw(0, 0);
	ofPopMatrix();


	//if (managerFFinder.size() > 0 && false )
	//{
	//	ofRectangle bbox = managerFFinder.getObjectSmoothed(0);
	//	bbox.scaleFromCenter(2, 2);
	//	bbox.translateY(bbox.getHeight()*0.1);
	//	if (bbox.getRight() > camW)
	//		bbox.setWidth(camW - bbox.x);
	//	if (bbox.getBottom() > camH)
	//		bbox.setHeight(camH - bbox.y);

	//	ofImage personFace;
	//	ofPixels pixels;


		//comp.readToPixels(pixels);
		//personFace.setFromPixels(pixels.getPixels(), camW, camH, OF_IMAGE_COLOR_ALPHA, true);

		//personFace.crop(bbox.x, bbox.y, bbox.width, bbox.height);


		//ofImage portrait;
		//portrait.setFromPixels(personFace.getPixels());
		//portrait.resize(100, 100);

	//}
}

void manager::detectFaces(ofVideoGrabber cam) {
	ofImage camFrame;
	camFrame.setFromPixels(cam.getPixels());
	detectFaces(camFrame);
}

















ofImage manager::makePortrait( ofImage camFrame, float shdPrepThress) {

	if (!bg.isAllocated())
		setBg(camFrame);

	debug.begin();
	ofClear(0);


	//////////////////////
	// debug Draw cam
	camFrame.draw(0, 0);
	// -----------------------

	// debug Draw BG
	bg.draw(camW, 0);		// to debug buffer
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
	faceMask.draw(camW*2, 0);
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
	faceDetectMask.draw(0,camH);
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
	prep.draw(camW, camH);
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
	contourMask.draw(camW*2, camH);
	// -----------------------


	// debug Draw countour lines
	ofSetColor(ofColor::red);
	ofSetLineWidth(3);
	ofPushMatrix();
	ofTranslate(camW , camH);
	for (int i = 0; i < controurSurfaces.size(); i++) {
		vector <ofPolyline> outlines = controurSurfaces[i].getOutline();
		for (int i = 0; i < outlines.size(); i++)
			outlines[i].draw();
	}
	ofPopMatrix();
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
	//comp.draw(camW*2.5, camH);
	// -----------------------


	debug.end();

	ofImage out;
	comp.readToPixels(out.getPixelsRef());
	

	return out;
}
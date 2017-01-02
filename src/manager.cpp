#include "manager.h"

using namespace ofxCv;


manager::manager(){
}

manager::manager(int _camW, int _camH)
{



	camW = _camW;
	camH = _camH;
	nextPersonId = 0;
	debugMode = true;
	canvas.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debug.allocate(ofGetWindowWidth(), ofGetWindowHeight());

	faceFinder.setup("haarcascade_frontalface_default.xml");
	faceFinder.setPreset(ObjectFinder::Fast);

}


manager::~manager()
{
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
	ofPushMatrix();
	ofTranslate(1000, 0);
	ofScale(0.5, 0.5);
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



void manager::makePortrait( ofImage cam, ofImage bg ) {
	debug.begin();
	ofClear(0);


	//////////////////////
	// debug Draw cam
	cam.draw(0, 0);

	// debug Draw BG
	bg.draw(camW, 0);		// to debug buffer



	////////////////////////
	//// Temp Draw face mask - will be replace with faceTracker
	//ofFbo faceMask;
	//faceMask.allocate(camW, camH, GL_RGBA);
	//faceMask.begin();
	//ofClear(0);
	//ofSetColor(ofColor::white);
	//ofDrawCircle(100, 100, 70);
	//faceMask.end();

	//// debug Draw faceMask
	//faceMask.draw(camW*2, 0);




	////////////////////////
	//// Face detection
	//faceFinder.update(cam);
	//// Draw squares for each face tracked
	//ofFbo faceDetectMask;
	//faceDetectMask.allocate(camW, camH, GL_RGBA);
	//faceDetectMask.begin();
	//ofClear(0);
	//ofSetColor(ofColor::white);
	//for (int i = 0; i < faceFinder.size(); i++) {
	//	ofRectangle face = faceFinder.getObjectSmoothed(i);
	//	face.scaleFromCenter(2, 2);
	//	face.translateY(face.getHeight()*0.1);
	//	ofDrawRectangle(face);
	//}
	//faceDetectMask.end();

	//// debug Draw FaceDetectMask
	//faceDetectMask.draw(0,camH);


	//////////////////////
	// Prep Render

	//ofFbo prep;
	//prep.allocate(camW, camH, GL_RGBA);
	//prep.begin();
	//shdPrep.begin();
	//// use as a tex0 the same image you are drawing below
	//shdPrep.setUniformTexture("tex0", cam.getTexture(), 0);
	//if (bg.isAllocated())
	//	shdPrep.setUniformTexture("tex1", bg.getTexture(), 1);
	//shdPrep.setUniformTexture("tex2", faceMask.getTexture(), 2);
	//shdPrep.setUniformTexture("tex3", faceDetectMask.getTexture(), 3);

	//shdPrep.setUniform1f("thress", 0.1);	// toDo Set thresshold

	//cam.draw(0, 0);
	//shdPrep.end();
	//prep.end();






	debug.end();

}
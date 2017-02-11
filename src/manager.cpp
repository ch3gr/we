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
	
	faceDetectW = 256;
	contourDetectW = camW*0.5;
	debugPortraitScale = 0.5;

	portraitWithAlpha = true;
	debugPeople = true;
	debugUpdateEvidence = false;
	
	

	nextPersonId = 0;
	canvas.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugPortrait.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	debugTrackers.allocate(ofGetWindowWidth(), ofGetWindowHeight());

	shdPrepThress = 0.09;


	scout.setup("haarcascade_frontalface_default.xml");
//	scout.setup("haarcascade_eye.xml");
	scout.setPreset(ObjectFinder::Fast);
	//scout.setPreset(ObjectFinder::Accurate);
	//scout.setPreset(ObjectFinder::Sensitive);
	scout.setRescale( faceDetectW/float(camW) );
	scout.setMinSizeScale(0.1);
	scout.setMaxSizeScale(0.8);

	candidates.setMaximumDistance(camW / 5);
	candidates.setPersistence(10);


	model = createEigenFaceRecognizer(80, 5000);
	//model = createFisherFaceRecognizer(0, 5000);
	//model = createLBPHFaceRecognizer();

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

void manager::addPerson(ofImage _face) {
	person someoneNew = person(nextPersonId++, _face);
	we.push_back(someoneNew);
}

void manager::addPerson(ofImage _face, vector<ofImage> _snapshots) {
	person someoneNew = person(nextPersonId, _face, _snapshots);
	we.push_back(someoneNew);


	//vector<Mat> modelFaces;
	//vector<ofImage> modelOfFaces;
	//vector<int> modelLabels;

	
	for (int s = 0; s < someoneNew.snapshotsCV.size(); s++) {
		modelFaces.push_back(someoneNew.snapshotsCV[s]);
		modelLabels.push_back(nextPersonId);
	}
	//model->update(modelFaces, modelLabels);
	model->train(modelFaces, modelLabels);

	nextPersonId++;
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

	for (int p = 0; p < we.size(); ++p) {
		we[p].draw();

		if(debugPeople )
			we[p].drawDebug();
	}

	canvas.end();
	canvas.draw(0, 0);

	//debugTimers.draw(0, 0);

}









void manager::drawDebug(ofImage camFrame) {

	vector<candidate> & followers = candidates.getFollowers();
	if (followers.size() > 0) {
		ofRectangle someone = followers[0].faceBounds;
		makePortrait(camFrame, someone);
	}


	debugPortrait.draw(ofGetWindowWidth() - (camW*3*debugPortraitScale), 0);
}

void manager::drawDebugTrackers(){

	debugTrackers.begin();
	vector<candidate> & followers = candidates.getFollowers();
	for (int c = 0; c < followers.size(); c++)
	{
		if(followers[c].evidenceIsSet){ 
			// Draw cv_evidence
			ofImage ofMat;
			ofxCv::toOf(followers[c].cv_evidence, ofMat);
			if (ofMat.isAllocated()) {
				ofMat.update();
				ofMat.draw(camW + c * 75, 0);
			}
		}

		// Draw snapshots
		for (int s = 0; s < followers[c].snapshots.size(); s++) {
			followers[c].snapshots[s].draw(camW + c * 75, (s+1) * 75);
		}
	}

	debugTrackers.end();
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
	nextPersonId = 0;

	// doesn't work, but maybe it needs something similar
	//model->~FaceRecognizer();
	//model = createEigenFaceRecognizer(80, 5000);
}












void manager::detectFaces(ofImage cam) {

	// only at the first frame
	if (!bg.isAllocated())
		setBg(cam);


	scout.update(cam);
	candidates.track(scout.getObjects());

	vector<candidate> & followers = candidates.getFollowers();
	for (int c = 0; c < followers.size(); c++) {

		// Take the <<PHOTO>> portrait
		if (!followers[c].exist && followers[c].trigger) {
			
			ofImage portrait;
			// create Alpha portrait
			if (portraitWithAlpha) {
				portrait = makePortrait(cam, followers[c].faceBounds);
				addPerson(portrait, followers[c].snapshots);
			}
			else {
				portrait.clone(cam);
				ofRectangle frame = adjustFaceBounds(followers[c].faceBounds, camW, camH);
				portrait.crop(frame.x, frame.y, frame.getWidth(), frame.getHeight());
				addPerson(portrait, followers[c].snapshots);
			}
		}
		// Or keep a training snapshots
		else if (!followers[c].exist && followers[c].isSnapshot() ) {
			followers[c].takeSnapshot(cam);
		}
	}




	debugTrackers.begin();
	ofClear(0);
	cam.draw(0, 0);

	// Draw the face trackers
	//scout.draw();
	
	for (int i = 0; i < followers.size(); i++) {
		followers[i].draw();

		ofPushStyle();
		ofNoFill();
		ofSetColor(ofColor::lightGrey);
		ofRectangle framing = adjustFaceBounds(followers[i].faceBounds, camW, camH);
		ofDrawRectangle(framing.x, framing.y, framing.width, framing.height);
		ofPopStyle();

	}





	////////////////////////////
	// Face recognition stuff
	// optimize!!


	if (we.size() > 0)
	{
		for (int i = 0; i < followers.size(); i++) {
			int match = -1;
			double confidence = -1;

			// Make sure it has a snapshot
			ofImage idSnapshot;
			Mat cv_idSnapshot;

			// constant update identification evidence
			// only useful for debuging
			if (debugUpdateEvidence) {
				// better/slower use the live cam
				idSnapshot.clone(cam);
				idSnapshot.crop(followers[i].faceBounds.x, followers[i].faceBounds.y, followers[i].faceBounds.getWidth(), followers[i].faceBounds.getHeight());
				idSnapshot.resize(75, 75);

				Mat cv_idSnapshotRGB = ofxCv::toCv(idSnapshot);
				cvtColor(cv_idSnapshotRGB, cv_idSnapshot, CV_RGB2GRAY);
					
				model->predict(cv_idSnapshot, match, confidence);
				ofDrawBitmapStringHighlight("?", followers[i].faceBounds.x+5, followers[i].faceBounds.getBottom() - 10, ofColor::black, ofColor::yellow);

				followers[i].cv_evidence = cv_idSnapshot;
				followers[i].lastMatch = match;
				followers[i].lastConfidence = confidence;
			}
			// or use the predifined snapshot
			else {
				if (!followers[i].exist && followers[i].evidenceIsSet) {
					cv_idSnapshot = followers[i].cv_evidence;

					model->predict(cv_idSnapshot, match, confidence);
					cout << "INVESTIGATE " << ofGetElapsedTimef() << endl;

					followers[i].lastMatch = match;
					followers[i].lastConfidence = confidence;
				}
			}
				

			
		
			if (confidence != -1 && confidence < 1000)
				followers[i].exist = true;

				
			// if no person was found, use the last recorded match
			if (followers[i].lastMatch != -1 && followers[i].lastMatch <= we.size()) {
				
				match = followers[i].lastMatch;
				confidence = followers[i].lastConfidence;
				int pX = we[match].x;
				int pY = we[match].y;
				pX += we[match].face.getWidth() / 2;
				ofDrawLine(followers[i].faceBounds.x, followers[i].faceBounds.getBottom(), pX, pY);
				ofDrawBitmapStringHighlight(ofToString(match).append(":").append(ofToString(confidence)), followers[i].faceBounds.x, followers[i].faceBounds.getBottom() + 20, ofColor::black, ofColor::white);
			}

		}
	}


	// see if I can view the internal model

	//Mat eigenvalues = model->getMat("eigenvectors");
	//ofImage ofMat;
	//ofxCv::toOf(eigenvalues, ofMat);
	//ofMat.update();
	//ofMat.draw(1200, 10);

	//eigenvalues = model->getMat("eigenvalues");
	//ofxCv::toOf(eigenvalues, ofMat);
	//ofMat.update();
	//ofMat.draw(1300, 10);
	//
	//eigenvalues = model->getMat("mean");
	//ofxCv::toOf(eigenvalues, ofMat);
	//ofMat.update();
	//ofMat.draw(1400, 10);

		


	debugTrackers.end();


	

}




void manager::detectFaces(ofVideoGrabber cam) {
	ofImage camFrame;
	camFrame.setFromPixels(cam.getPixels());
	detectFaces(camFrame);
}























ofImage manager::makePortrait( ofImage camFrame, ofRectangle faceBounds) {


	float cScale = contourDetectW / float(camW);

	faceBounds = adjustFaceBounds(faceBounds, camW, camH);

	debugPortrait.begin();
	ofClear(0);


	//////////////////////
	// debugPortrait Draw cam
	ofPushMatrix();
	ofScale(debugPortraitScale, debugPortraitScale);
	camFrame.draw(0, 0);
	ofPopMatrix();
	// -----------------------

	// debugPortrait Draw BG
	ofPushMatrix();
	ofScale(debugPortraitScale, debugPortraitScale);
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
	ofScale(debugPortraitScale, debugPortraitScale);
	faceMask.draw(camW*2, 0);
	ofPopMatrix();
	// -----------------------




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
	ofScale(debugPortraitScale, debugPortraitScale);
	faceBoundsFBO.draw(0, camH);
	ofPopMatrix();
	// -----------------------





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
	ofScale(debugPortraitScale, debugPortraitScale);
	//ofTranslate(camW, camH);
	prep.draw(camW, camH);
	ofPopMatrix();
	// -----------------------
	


	
	

	//////////////////////
	// Find contrours on the binary image
	//////////////////////
	// prep contours
	// Pass Preped buffer to cvImage and detect contours


	ofImage grabPrep;
	prep.begin();
	grabPrep.grabScreen(0,0,prep.getWidth(), prep.getHeight());
	prep.end();


	//grabPrep.resize(camW*cScale, camH*cScale);




	ofxCvGrayscaleImage cvImgGrayscale;
	cvImgGrayscale.allocate(grabPrep.getWidth(), grabPrep.getHeight());
	cvImgGrayscale.setFromPixels(grabPrep.getPixels().getChannel(0));



	//cvImgGrayscale.resize(camW*cScale, camH*cScale);

	

	//cvImgGrayscale.dilate();
	//cvImgGrayscale.dilate();
	//cvImgGrayscale.erode();
	//cvImgGrayscale.erode();



	// calc contours
	ofxCvContourFinder contourFinder;
	contourFinder.findContours(cvImgGrayscale, ((camW*cScale*0.2)*(camH*cScale*0.2)), ((camW*cScale)*(camH*cScale)), 5, false, true);

	

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
	ofScale(debugPortraitScale, debugPortraitScale);
	contourMask.draw(camW*2, 0);
	ofPopMatrix();
	// -----------------------


	// debugPortrait Draw countour lines
	ofSetColor(ofColor::red);
	ofSetLineWidth(3);
	ofPushMatrix();
	ofScale(debugPortraitScale, debugPortraitScale);
	ofTranslate(camW*2 , 0);

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
	ofScale(debugPortraitScale, debugPortraitScale);
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










	return out;
}




// return the person by id
person manager::getPerson(int id) {
	for (int p = 0; p < we.size(); p++) {
		if (we[p].id == id)
			return we[p];
	}
	
}









// Make the faceBounds bigger for better cropping
ofRectangle adjustFaceBounds(ofRectangle faceBounds, int camW, int camH) {
	
	faceBounds.scaleFromCenter(1.5, 2.2);
	faceBounds.translateY(faceBounds.getHeight()*0.05);

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




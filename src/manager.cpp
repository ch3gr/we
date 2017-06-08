#include "manager.h"
#include "ofApp.h"


using namespace ofxCv;



manager::manager()
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




	


	// frame size for debuging
	float debugTrackersW = ofGetWindowWidth() / 2.0 - 300;
	debugTrackersScale = debugTrackersW / camW;

	float debugPortraitW = (ofGetWindowWidth() / 2.0) / 3.0 ;
	debugPortraitScale = debugPortraitW / camW;




	portraitWithAlpha = true;
	debugTrackers = true;
	debugPortrait = true;
	debugPeople = false;
	debugUpdateEvidence = false;
	

	nextPersonId = 0;
	flash = 0;
	FBO_debugPortrait.allocate(ofGetWindowWidth(), ofGetWindowHeight());
	FBO_debugTrackers.allocate(ofGetWindowWidth(), ofGetWindowHeight());



	scout.setup("haarcascade_frontalface_default.xml");
//	scout.setup("haarcascade_eye.xml");
	scout.setPreset(ObjectFinder::Fast);
	//scout.setPreset(ObjectFinder::Accurate);
	//scout.setPreset(ObjectFinder::Sensitive);
	scout.setRescale( 128/float(camW) );
	scout.setMinSizeScale(0.1);
	scout.setMaxSizeScale(0.8);


	candidates.setMaximumDistance(camW / 5);
	candidates.setPersistence(10);


	
	// FisherFaceRecognizer requires an extra sample ?
	addMonkey();
}


manager::~manager()
{
}


void manager::setBg(ofImage & _bg) {
	bg = _bg;
}

void manager::setBg(ofVideoGrabber & _cam) {
	bg.setFromPixels(_cam.getPixels());
}



void manager::addPerson(vector<ofImage> &_frames, vector<ofImage> & _snapshots) {
	person someoneNew = person(nextPersonId, _frames, _snapshots);
	we.push_back(someoneNew);

	for (int s = 0; s < someoneNew.snapshotsCV.size(); s++) {
		model.addSample(someoneNew.snapshotsCV[s], nextPersonId);
	}
	nextPersonId++;
	flash = 1;
}



void manager::addMonkey() {

	// Fisher requires at least two samples 
	//Mat empty(75, 75, CV_8UC3, Scalar(128, 128, 128));
	//modelFaces.push_back(empty);
	//modelLabels.push_back(999);

	ofImage monkeyOf;
	monkeyOf.load("monkey.jpg");
	Mat monkeyCv = ofxCv::toCv(monkeyOf);
	Mat monkeyGray;
	cvtColor(monkeyCv, monkeyGray, CV_RGB2GRAY);

	model.addSample(monkeyGray, 999);
	
}






void manager::curate() {
	int size = we.size();

	// debugging
	if( debugPortrait || debugTrackers)
	{
		// spread them at the bottom of the half window
		for (float p = 0; p < size; ++p) {
			float x = (1-((p+1) / float(size))) * ofGetWidth() + we[p].face.getWidth()/2;
		
			float yMax = (ofGetHeight()*0.4) + we[p].face.getHeight();
			float yMin = ofGetHeight();

			ofSeedRandom(we[p].id);
			float wave = cos(ofGetElapsedTimef() + ofRandom(1000))*0.5 +0.5;
			float y = ofLerp(yMin, yMax, wave);

			we[p].setPos(x, y);
		}
	}
	else {
		ofSeedRandom(size);

		for (int p = 0; p < size; ++p) {
			float param=0;
			float x = 0;
			float y = 0;

			if (size > 1)
			{
				float yMin = ofGetHeight()*0.2;
				float yMax = ofGetHeight() + 100;


				param = float(p) / float(size - 1);
				//x = ofMap(sin(ofGetElapsedTimef()+p*10), -1,1,0, ofGetWidth(), 1);
				x = ofRandom(ofGetWidth());
				y = ofMap(param, 0, 1, yMin, yMax, true);
			}
			else
			{
				x = ofGetWidth() / 2.0;
				y = ofGetWidth() / 2.0;
			}

			x += (ofNoise((p + ofGetElapsedTimef()) * 0.1 )*2-1) * 50;
			y += (ofNoise((p - ofGetElapsedTimef()) * 0.11)*2-1) * 10;


			we[p].setPos(x, y);

			
			float wave = sin(ofGetElapsedTimef() * ofRandom(3, 4));
			we[p].rotate = ofMap( wave, -1, 1, -45, 45);
			wave = sin(ofGetElapsedTimef() * ofRandom(0.2, 1));
			we[p].scale = ofMap( wave, -1, 1, 50, 400) ;
			

		}
	}

	
	
}














void manager::draw() {
	

	ofColor bgColor = ofColor::darkGray;
	ofClear(bgColor);

	for (int p = 0; p < we.size(); ++p) {
		we[p].draw();

		if(debugPeople )
			we[p].drawDebug();
	}


	// Flash fade
	if (flash > 0.01) {
		bgColor = ofColor(ofColor::white, flash*255);
		ofPushStyle();
		ofSetColor(bgColor);
		ofFill();
		ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		ofPopStyle();
		flash *= 0.92;
	}





	/*
	if (we.size() >= 1) {
		Mat eigenvalues = model.model->getMat("eigenvectors");
		ofxCv::drawMat(eigenvalues, 1000, 10);
		ofImage ofMat;
		ofxCv::toOf(eigenvalues, ofMat);
		ofMat.update();
		ofMat.draw(1200, 10);
		
		eigenvalues = model.model->getMat("eigenvalues");
		ofxCv::toOf(eigenvalues, ofMat);
		ofMat.update();
		ofMat.draw(1300, 10);
		
		eigenvalues = model.model->getMat("mean");
		ofxCv::toOf(eigenvalues, ofMat);
		ofMat.update();
		ofMat.draw(1400, 10);
	}
	*/
}









void manager::drawDebugPortrait(ofImage & camFrame) {

	float split = ((ofApp*)ofGetAppPtr())->guiDebugSplit;

	float debugPortraitW = (ofGetWindowWidth() * split) / 3.0;
	debugPortraitScale = debugPortraitW / camW;



	vector<candidate> & followers = candidates.getFollowers();
	if (followers.size() > 0) {
		ofRectangle someone = followers[0].faceBounds;
		makePortrait(camFrame, someone);
	}


	FBO_debugPortrait.draw(ofGetWindowWidth() - (camW*3*debugPortraitScale), 0);
}

void manager::drawDebugTrackers(){

	float split = ((ofApp*)ofGetAppPtr())->guiDebugSplit;
	float debugTrackersW = ofGetWindowWidth() * (1-split) - 300;
	debugTrackersScale = debugTrackersW / camW;



	FBO_debugTrackers.begin();
	vector<candidate> & followers = candidates.getFollowers();
	for (int c = 0; c < followers.size(); c++)
	{
		if(followers[c].evidenceIsSet){ 
			// Draw cv_evidence
			ofImage ofMat;
			ofxCv::toOf(followers[c].cv_evidence, ofMat);
			if (ofMat.isAllocated()) {
				ofMat.update();
				ofMat.draw((camW*debugTrackersScale) + c * 75, 0);
			}
			
		}

		// Draw snapshots
		for (int s = 0; s < followers[c].snapshots.size(); s++) {
			followers[c].snapshots[s].draw((camW*debugTrackersScale) + c * 75, (s+1) * 75);
		}
	}

	FBO_debugTrackers.end();
	FBO_debugTrackers.draw(0, 0);
}





void manager::info() {
	// print people
	cout << "I am the manager and these are my people :" << endl;
	for (int p = 0; p < we.size(); ++p)
		we[p].info();

	cout << "\n   there are " << we.size() << " of us." << endl;

	// candidates
	//vector<candidate> & followers = candidates.getFollowers();
	//for (int i = 0; i < followers.size(); i++) {
	//	cout << "I am follower : " << followers[i].getLabel()  << endl;
	//}

	
}


void manager::forgetUs() {
	we.clear();
	nextPersonId = 0;
	model.forget();

	addMonkey();
}



// Save the group to disk
void manager::saveUs(bool append) {

	string session = "s02";
	string sessionPath = "sessions/" + session + "/";
	string personPrefix = "p";
	string framePrefix = "f";
	string snapshotPrefix = "s";
	string ext = ".tif";
	cout << "\nSaving session \n" << "data/" << sessionPath << session << "/" << endl;

	int nextId = 0;
	if (append) {
		// Find the last person in the session and set the nextId
		ofDirectory sessionDir = ofDirectory(sessionPath);
		if (!sessionDir.exists())
			sessionDir.create();
		else
		{
			sessionDir.listDir();
			if (sessionDir.size() > 0) {
				sessionDir.sort();
				string lastPerson = sessionDir.getName(sessionDir.size() - 1);
				lastPerson = lastPerson.substr(personPrefix.length(), lastPerson.length() - 1);
				nextId = std::stoi(lastPerson) + 1;
				cout << "     append next id :" << nextId << endl;
			}
		}
	}
	else {
		// Delete existing session directory
		ofDirectory::removeDirectory(sessionPath, true, true);
	}

	// ALL OF US
	for (int p = 0; p < we.size(); p++) {
		string personId = padThis(4, p + nextId);
		string personPath = session+"_"+personPrefix + personId + "/";
		cout << "  |__ " << personPath << endl;

		personPath = sessionPath + personPath;

		// SNAPSHOTS and frames
		for (int s = 0; s < we[p].snapshots.size(); s++) {
			
			string snapshotFile = session +"_"+ personPrefix+ personId + "_" + snapshotPrefix + padThis(4, s) + ext;
			string frameFile = session + "_" + personPrefix+ personId + "_" + framePrefix + padThis(4, s) + ext;
			cout << "    |__ " << snapshotFile << endl;
			cout << "    |__ " << frameFile << endl;

			// Prepare snapshot file path and SAVE
			snapshotFile = personPath + snapshotFile;
			we[p].snapshots[s].save(snapshotFile, OF_IMAGE_QUALITY_BEST);
			// Prepare frame file path and SAVE
			frameFile = personPath + frameFile;
			we[p].frames[s].save(frameFile, OF_IMAGE_QUALITY_BEST);
		}
	}
}








// Load a session from disk
void manager::loadUs() {
	string session = "s01";
	string sessionPath = "sessions/"+session+"/";
	string personPrefix = "p";
	string snapshotPrefix = "s";
	string framePrefix = "f";
	string ext = ".tif";


	cout << "\nLoading session \n" << "  data/" << sessionPath << endl;

	ofDirectory sessionDir = ofDirectory(sessionPath);
	sessionDir.listDir();

	// PEOPLE
	for (int p = 0; p < sessionDir.size(); p++) {
		cout << "\n    Loading Person" << endl;
		cout << "    |__ " << sessionDir.getName(p) << endl;

		ofDirectory personDir = ofDirectory(sessionDir.getPath(p));
		if (personDir.exists())
		{
			//ofImage face;
			vector<ofImage> snapshots;
			vector<ofImage> frames;

			// Read all data
			personDir.listDir();
			vector<ofFile> files = personDir.getFiles();
			for (int f = 0; f < files.size(); f++) {
				cout << "      |__ " << files[f].getBaseName() << endl;

				int tokenIndex = session.length() + personPrefix.length() + 6;
				string token = files[f].getBaseName().substr(tokenIndex, 1);

				// Load snapshots
				if( token == snapshotPrefix) {
					ofImage snapshot;
					snapshot.load(files[f].path());
					snapshots.push_back(snapshot);
				}
				// Load frames
				if( token == framePrefix ) {
					ofImage frame;
					frame.load(files[f].path());
					frames.push_back(frame);
				}
			}

			// Are we good to add a person?
			if (frames.size() > 0 && snapshots.size() > 0) {

				addPerson(frames, snapshots);
			}
		}
	}
	
}
















void manager::detectFaces(ofImage & cam) {


	// only at the first frame
	if (!bg.isAllocated())
		setBg(cam);

	// because candidate::takeSnapshot() takes a reference of frame, we need to avoid taking two snapshots on one frame
	bool dirtyFrame = false;


	// Prep the debuggin buffer
	if (debugTrackers) {
		FBO_debugTrackers.begin();
		ofClear(0);
		ofScale(debugTrackersScale, debugTrackersScale);
		cam.draw(0, 0);
	}

	// If model is trainning, dont bother tracking
	if (model.isReady()) {


		// Adjust tracker
		scout.setRescale(((ofApp*)ofGetAppPtr())->guiTrackerWidth / float(camW));
		scout.setMinSizeScale(((ofApp*)ofGetAppPtr())->guiTrackerMinSize);
		scout.setMaxSizeScale(((ofApp*)ofGetAppPtr())->guiTrackerMaxSize);

		scout.update(cam);
		candidates.track(scout.getObjects());

		vector<candidate> & followers = candidates.getFollowers();
		for (int c = 0; c < followers.size(); c++) {

			////////////////////////////
			// DRAW trackers debug
			if (debugTrackers) {
				followers[c].draw();
			}
			////////////////////////////
			// LOGIC

			// Is it possible to take a photo?
			if (!followers[c].ignore && followers[c].active && followers[c].isPhotoTime()) {

				// Create a new <<PERSON>>
				if (followers[c].trigger) {

					addPerson(followers[c].frames, followers[c].snapshots);
					model.train();

					followers[c].ignore = true;
					followers[c].lastMatch = nextPersonId - 1;
				}
				// Or take a <<FRAME>> and a <<SNAPSHOT>>
				else if(!dirtyFrame){

					captureFrame(cam, followers[c]);

					followers[c].setSnapshotCrop(followers[c].faceBounds);
					followers[c].takeSnapshot(cam);
					
					dirtyFrame = true;

					/*
					ofImage portrait;
					if (portraitWithAlpha) {
					
					}
					else {
						portrait.clone(cam);
						ofRectangle frame = adjustFaceBounds(followers[c].faceBounds);
						portrait.crop(frame.x, frame.y, frame.getWidth(), frame.getHeight());
						addPerson(portrait, followers[c].snapshots);
					}
					*/
				}
			}
		}










		////////////////////////////
		// FACE recognition stuff


		if (we.size() > 0)
		{
			for (int i = 0; i < followers.size(); i++) {

				// Break if the previous tracker captured a person, to avoid crash with untrained model
				if (!model.isReady())
					break;

				int match = -1;
				double confidence = -1;

				// Make sure it has a snapshot
				ofImage idSnapshot;
				Mat cv_idSnapshot;

				// constant update identification evidence
				// only useful for debuging
				if (debugUpdateEvidence && debugTrackers && !dirtyFrame) {
					// better/slower use the live cam
					idSnapshot.clone(cam);
					idSnapshot.crop(followers[i].faceBounds.x, followers[i].faceBounds.y, followers[i].faceBounds.getWidth(), followers[i].faceBounds.getHeight());
					idSnapshot.resize(75, 75);

					Mat cv_idSnapshotRGB = ofxCv::toCv(idSnapshot);
					cvtColor(cv_idSnapshotRGB, cv_idSnapshot, CV_RGB2GRAY);

					model.predict(cv_idSnapshot, match, confidence);
					ofDrawBitmapStringHighlight("?", followers[i].faceBounds.x + 5, followers[i].faceBounds.getBottom() - 10, ofColor::black, ofColor::yellow);

					followers[i].cv_evidence = cv_idSnapshot;
					followers[i].lastMatch = match;
					followers[i].lastConfidence = confidence;
				}
				// or use the predifined snapshot
				else if (!followers[i].ignore && followers[i].evidenceIsSet) {
					cv_idSnapshot = followers[i].cv_evidence;

					model.predict(cv_idSnapshot, match, confidence);

					followers[i].lastMatch = match;
					followers[i].lastConfidence = confidence;
				}



				////////////////////////////
				// RECOGNIZED peson
				double guiConf = ((ofApp*)ofGetAppPtr())->guiConfidenceThress;
				if (confidence != -1 && confidence < guiConf)	// 1000
					followers[i].ignore = true;




				// if no person was found, use the last recorded match
				if (debugTrackers && followers[i].lastMatch != -1 && followers[i].lastMatch <= we.size()) {

					match = followers[i].lastMatch;
					confidence = followers[i].lastConfidence;

					if (debugTrackers) {
						float pX = we[match].x;
						float pY = we[match].y;
						if (we[match].face.isAllocated()) {
							pX -= we[match].face.getWidth() / 2;
							pY -= we[match].face.getHeight();
						}

						pX += we[match].face.getWidth() / 2;

						// adjust for debugTrackersScale
						pX /= debugTrackersScale;
						pY /= debugTrackersScale;

						ofDrawLine(followers[i].faceBounds.x, followers[i].faceBounds.getBottom(), pX, pY);
						ofDrawBitmapStringHighlight(ofToString(match).append(":").append(ofToString(confidence)), followers[i].faceBounds.x, followers[i].faceBounds.getBottom() - 15, ofColor::black, ofColor::white);
					}
				}

			}
		}
	}
	else if(debugTrackers){
		ofDrawBitmapStringHighlight( "<<<<<< TRAINING >>>>>>", camH/2,50, ofColor::black, ofColor::red);
		ofPushStyle();
		ofSetColor(ofColor::red);
		ofSetLineWidth(25);
		ofNoFill();
		ofDrawRectangle(0, 0, camW, camH);
		ofPopStyle();
	}

	if (debugTrackers)
		FBO_debugTrackers.end();

	// see if I can view the internal model




}




void manager::detectFaces(ofVideoGrabber & cam) {
	ofImage camFrame;
	camFrame.setFromPixels(cam.getPixels());
	detectFaces( camFrame);
}























ofImage manager::makePortrait( ofImage & camFrame, ofRectangle & faceBounds) {

	// frame size for calculations

	float cScale = ((ofApp*)ofGetAppPtr())->guiContourImgScale;
	//posterize the value, so it doesn't fuck up the scaling
	cScale = ceil(cScale * 19 + 0.0001) / 20;

	faceBounds = adjustFaceBounds(faceBounds);

	FBO_debugPortrait.begin();
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
	//ofFbo faceMask;
	//faceMask.allocate(camW, camH, GL_RGBA);
	//faceMask.begin();
	//ofClear(0);
	//ofSetColor(ofColor::white);
	//ofDrawCircle(100, 100, 70);
	//faceMask.end();

	// debugPortrait Draw faceMask
	//ofPushMatrix();
	//ofScale(debugPortraitScale, debugPortraitScale);
	//faceMask.draw(camW*2, 0);
	//ofPopMatrix();
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
	shdPrep.setUniform1f("thress", ((ofApp*)ofGetAppPtr())->guiLumaKey );
	
	shdPrep.setUniform1f("imgW", camW);
	shdPrep.setUniform1f("imgH", camH);

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



	// dilate and erode the image multiple times to siplify it
	int simplify = ((ofApp*)ofGetAppPtr())->guiContourImgSimplify;
	for (int i = 0; i < simplify; i++)
		cvImgGrayscale.dilate();
	for (int i = 0; i < simplify; i++)
		cvImgGrayscale.erode();




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
		outline = outline.getSmoothed(((ofApp*)ofGetAppPtr())->guiContourSmooth);

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


	FBO_debugPortrait.end();

	ofImage fboImage;
	comp.readToPixels(fboImage.getPixelsRef());
	ofImage out;
	out.clone(fboImage);
	

	out.crop(faceBounds.x, faceBounds.y, faceBounds.getWidth(), faceBounds.getHeight());





	return out;
}
















void manager::captureFrame(ofImage & camFrame, candidate & follower) {

	// frame size for calculations

	float cScale = ((ofApp*)ofGetAppPtr())->guiContourImgScale;
	//posterize the value, so it doesn't fuck up the scaling
	cScale = ceil(cScale * 19 + 0.0001) / 20;

	ofRectangle frameBounds = adjustFaceBounds(follower.faceBounds);
	

	FBO_debugPortrait.begin();
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
	//ofFbo faceMask;
	//faceMask.allocate(camW, camH, GL_RGBA);
	//faceMask.begin();
	//ofClear(0);
	//ofSetColor(ofColor::white);
	//ofDrawCircle(100, 100, 70);
	//faceMask.end();

	// debugPortrait Draw faceMask
	//ofPushMatrix();
	//ofScale(debugPortraitScale, debugPortraitScale);
	//faceMask.draw(camW*2, 0);
	//ofPopMatrix();
	// -----------------------




	//////////////////////
	// Face detection
	// Draw a white square that covers the bounding box of the face
	ofFbo faceBoundsFBO;
	faceBoundsFBO.allocate(camW, camH, GL_RGBA);
	faceBoundsFBO.begin();
	ofClear(0);
	ofSetColor(ofColor::white);
	ofDrawRectangle(frameBounds);
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
	shdPrep.setUniform1f("thress", ((ofApp*)ofGetAppPtr())->guiLumaKey);

	shdPrep.setUniform1f("imgW", camW);
	shdPrep.setUniform1f("imgH", camH);

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
	grabPrep.grabScreen(0, 0, prep.getWidth(), prep.getHeight());
	prep.end();


	//grabPrep.resize(camW*cScale, camH*cScale);




	ofxCvGrayscaleImage cvImgGrayscale;
	cvImgGrayscale.allocate(grabPrep.getWidth(), grabPrep.getHeight());
	cvImgGrayscale.setFromPixels(grabPrep.getPixels().getChannel(0));



	// dilate and erode the image multiple times to siplify it
	int simplify = ((ofApp*)ofGetAppPtr())->guiContourImgSimplify;
	for (int i = 0; i < simplify; i++)
		cvImgGrayscale.dilate();
	for (int i = 0; i < simplify; i++)
		cvImgGrayscale.erode();




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
		outline = outline.getSmoothed(((ofApp*)ofGetAppPtr())->guiContourSmooth);

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
		newPath.scale(1 / cScale, -1 / cScale);
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
	contourMask.draw(camW * 2, 0);
	ofPopMatrix();
	// -----------------------


	// debugPortrait Draw countour lines
	ofSetColor(ofColor::red);
	ofSetLineWidth(3);
	ofPushMatrix();
	ofScale(debugPortraitScale, debugPortraitScale);
	ofTranslate(camW * 2, 0);

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


	FBO_debugPortrait.end();

	ofImage fboImage;
	comp.readToPixels(fboImage.getPixelsRef());
	fboImage.update();
	fboImage.crop(frameBounds.x, frameBounds.y, frameBounds.getWidth(), frameBounds.getHeight());
	follower.storeFrame(fboImage);

	// make sure we remap the snapshot crop to match the now resized image
	follower.setSnapshotCrop(remapSnapshotCrop(follower.faceBounds, frameBounds));

}






























// return the person by id
person manager::getPerson(int id) {
	for (int p = 0; p < we.size(); p++) {
		if (we[p].id == id)
			return we[p];
	}
	
}




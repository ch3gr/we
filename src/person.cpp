#include "person.h"
#include "ofApp.h"

using namespace cv;




person::person(int _id, vector<ofImage> & _frames, vector<ofImage> & _snapshots) {

	id = _id;
	iFrame = 0;

	scale = 250;
	rotate = 0;
	translate = ofVec2f(00,00);


	frames = _frames;
	face = frames[ 0 ];
	snapshots = _snapshots;

	ofSeedRandom(ofGetSystemTimeMicros());
	float o = ofRandomf() * ((ofApp*)ofGetAppPtr())->guiAnimationInterval;
	nextTime = ofGetElapsedTimef() + o;


	// convert snapshots to CV Mat
	for (int s = 0; s < snapshots.size(); s++) {
		ofPixels toMatPixels = snapshots[s].getPixels();

		Mat color = ofxCv::toCv(toMatPixels);
		Mat grey;
		cvtColor(color, grey, CV_RGB2GRAY);
		snapshotsCV.push_back(grey);

	}


	cout << "New Person added to the list. ID :" << id << endl;
}




void person::setPos(float _x, float _y) {
	translate.x = _x;
	translate.y = _y;
}



void person::draw() {

	// incremenet frame
	if (frames.size() != 0) {
		if (ofGetElapsedTimef() > nextTime) {
			iFrame = (iFrame + 1) % frames.size();
			float interval = ((ofApp*)ofGetAppPtr())->guiAnimationInterval;
			nextTime = ofGetElapsedTimef() + interval;
		}

		// transform and draw
		if (frames[iFrame].isAllocated()) {

			// TRANSFORM
			ofPushMatrix();
			// normalize scale first based on width and then by pixels
			float s = scale / frames[iFrame].getWidth();
			ofTranslate( translate.x, translate.y);
			ofScale(s,s);
			ofRotate(rotate);
			ofTranslate(-frames[iFrame].getWidth() / 2, -frames[iFrame].getHeight() / 2);

			frames[iFrame].draw(0,0);

			ofPopMatrix();
		}

	}	
}

void person::drawDebug() {

// TRANSFORM
	ofPushMatrix();
// normalize scale first based on width and then by pixels
	float s = scale / frames[iFrame].getWidth();
	ofTranslate(translate.x, translate.y);
	ofScale(s, s);
	ofRotate(rotate);
	ofTranslate(-frames[iFrame].getWidth() / 2, -frames[iFrame].getHeight() / 2);





	ofPushStyle();
	ofNoFill();
	ofSetColor(ofColor::blue);
	ofDrawRectangle(0, 0, frames[iFrame].getWidth(), frames[iFrame].getHeight() );
	ofSetColor(ofColor::white);

	

	// draw snapshots
	if (false && snapshots.size() > 0) {
		ofPushMatrix();
		//ofScale(0.5, 0.5);
		//for (int s = 0; s < snapshots.size(); s++) {
		//	snapshots[s].draw(0, s * snapshots[s].getHeight());
		//}
		for (int s = 0; s < snapshotsCV.size(); s++) {
			// Draw mat
			ofImage ofMat;
			ofxCv::toOf(snapshotsCV[s], ofMat);
			if (ofMat.isAllocated()) {
				ofMat.update();
				ofMat.draw(snapshots[s].getWidth() * 0, s * snapshots[s].getHeight());
			}
		}
		ofPopMatrix();

	}

	ofDrawBitmapStringHighlight(ofToString(id), face.getWidth()/2+5, -5, ofColor::black, ofColor::white);
	ofDrawBitmapStringHighlight(ofToString(iFrame), face.getWidth() / 2 + 5, +10, ofColor::black, ofColor::white);


	ofPopStyle();
	ofPopMatrix();
}


void person::info() {

	cout << "I'm person: " << id << "\t\tX: " << translate.x << "\tY: " << translate.y <<endl;
}
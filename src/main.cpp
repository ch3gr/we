#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
	ofSetupOpenGL(1600,800,OF_WINDOW);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp());

}


/*
links

http://docs.opencv.org/trunk/dd/d65/classcv_1_1face_1_1FaceRecognizer.html
http://docs.opencv.org/2.4/modules/objdetect/doc/cascade_classification.html

tutorial (the same)
http://docs.opencv.org/master/da/d60/tutorial_face_main.html
http://docs.opencv.org/2.4/modules/contrib/doc/facerec/facerec_tutorial.html

*/



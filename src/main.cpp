#include "ofMain.h"
#include "ofApp.h"



//========================================================================
int main( ){
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);

	ofSetupOpenGL(1920,1080, OF_WINDOW);			// <-------- setup the GL context
	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofSetWindowPosition(1922, 20);

	//ofCreateWindow(settings);
	ofRunApp(new ofApp());

}


/*
// post compile event to copy shaders from /src to /bin/data
xcopy /e /y "$(ProjectDir)src\shaders" "$(ProjectDir)bin\data\shaders\"
links
http://docs.opencv.org/trunk/dd/d65/classcv_1_1face_1_1FaceRecognizer.html
http://docs.opencv.org/2.4/modules/objdetect/doc/cascade_classification.html
tutorial (the same)
http://docs.opencv.org/master/da/d60/tutorial_face_main.html
http://docs.opencv.org/2.4/modules/contrib/doc/facerec/facerec_tutorial.html
image types:
http://stackoverflow.com/questions/35379256/type-difference-on-image-processing-on-openframeworks
toCheck
https://github.com/antimodular/ofxFaceRecognizer/tree/master/src
https://github.com/kylemcdonald/ofxCv#project-setup
https://github.com/kylemcdonald/SharingFaces
of/examples/graphics/polylineBlobsExample
faceMask
\addons\ofxFaceTracker\example-advanced
Check how you can compile with static linking so it doesn't complain about missing :
msvcp140.dll
*/

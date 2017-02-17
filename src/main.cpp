#include "ofMain.h"
#include "ofApp.h"



//========================================================================
int main( ){
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);

	ofSetupOpenGL(1920,1080, OF_WINDOW);
//	ofSetupOpenGL(1920, 1200, OF_FULLSCREEN);
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofSetWindowPosition(1920, 20);

	//ofCreateWindow(settings);
	ofRunApp(new ofApp());

}


/*

toDo
- write to disk all the faces and 10 snapshots etc
- curate
- Threading
- add ofGui
- fine tune tracker settings
- fine tune Face recognition
- Fix camera res & processing resolution
- train model : tighter crop in for face recognition
- train model : align eyes?
- Webcame frame rate
- Mipos to manage exei reference tou cam (h' camHacked) apo tin arxi ston constractor
- remove or add better timers - https://github.com/armadillu/ofxTimeMeasurements
- read up and optimize passing ofImages and <vector>ofImages VS pointers as arguments
- leme twra... instalation autofotizomeni epifania apo piso h opoia na anavei me arduino otan kanei snap to portrait


Gui controls
- perpShaderThress
- candidate : speed thresshold






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


Checkare auto gia high FPS
https://github.com/arturoc/ofxGStreamer

tracker.h sto ofxCv
*/

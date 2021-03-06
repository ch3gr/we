#include "ofMain.h"
#include "ofApp.h"



//========================================================================
int main( ){
	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);

	ofAppGLFWWindow win;
	//win.setNumSamples(8);  
//	win.setMultiDisplayFullscreen(true); //this makes the fullscreen window span across all your monitors  
//	win.setWindowShape(1920, 1080);
//	ofSetupOpenGL(&win, 1920, 1080, OF_FULLSCREEN);

//	ofSetupOpenGL(1280, 720, OF_WINDOW);
	ofSetupOpenGL(1920,1080, OF_WINDOW);
//	ofSetupOpenGL(1920, 1080, OF_FULLSCREEN);
//	ofSetupOpenGL(1920, 1200, OF_FULLSCREEN);

//	ofSetWindowShape(1920, 1080);
//	ofSetWindowPosition(0, -1080);

	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
//	ofSetWindowPosition(1920, 20);

	//ofCreateWindow(settings);
	ofRunApp(new ofApp());

}


/*

toDo

- Add back portraitWithAlpha
- Add UI controls for candidate.snapshotSum and intervals
- Add UI control for candidates.setPersistence(10);
- Crashes if you load session without any prior training
- curate / add size control on the person
- fine tune tracker settings
- fine tune Face recognition
- train model : tighter crop in for face recognition
- train model : align eyes?
- Webcam frame rate
- port all shader versions
- fix scaling of test faces

- move captureFrame(), adjustFaceBounds() & remapSnapshotCrop() to candidate class

- leme twra... instalation autofotizomeni epifania apo piso h opoia na anavei me arduino otan kanei snap to portrait
- remove or add better timers - https://github.com/armadillu/ofxTimeMeasurements


- dokimase ta alla modela
http://docs.opencv.org/2.4/modules/contrib/doc/facerec/facerec_api.html#facerecognizer-train


From Zach
now using : harr
should look into : dlib	hog
local histogram
clahe openCV



Gui controls
- perpShaderThress
- candidate : speed thresshold





// How to get a variable from the scope of ofApp 
// include "ofApp.h"			sto .cpp
// ((ofApp*)ofGetAppPtr())->guiLumaKey



// post compile event to copy shaders from /src to /bin/data
xcopy /e /y "$(ProjectDir)src\shaders" "$(ProjectDir)bin\data\shaders\"


robocopy "$(OF_ROOT)/export/vs/$(Platform_Actual)/" "$(ProjectDir)bin/" "*.dll" /njs /njh /np /fp /bytes
if errorlevel 1 exit 0 else exit %errorlevel%



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



http://eyalarubas.com/face-detection-and-recognition.html




Checkare auto gia high FPS
https://github.com/arturoc/ofxGStreamer

tracker.h sto ofxCv
*/

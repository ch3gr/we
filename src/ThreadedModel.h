#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofThread.h"

using namespace ofxCv;
using namespace cv;

class ThreadedModel : public ofThread {
	void setup() {
		this->model = createEigenFaceRecognizer(80, 5000);
	}

	void threadedFunction() {

	}

public:
	Ptr<FaceRecognizer> model;
	vector<Mat> modelFaces;
	vector<ofImage> modelOfFaces;
	vector<int> modelLabels;

};
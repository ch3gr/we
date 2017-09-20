#pragma once
#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"

using namespace ofxCv;
using namespace cv;



class modelThread: public ofThread {
public:
	modelThread();
	~modelThread();

	void addSample(Mat face, int label);
	void train();
	void predict(Mat &sample, int &match, double &confidence);
	void forget();
	void update();
	void threadedFunction();
	bool isReady();

	void save();
	void load();

	void test();

	void test23();


	Ptr<FaceRecognizer> model;
	vector<Mat> modelFaces;
	vector<int> modelLabels;

	ofThreadChannel < Ptr<FaceRecognizer> > trainedModel;
	ofThreadChannel < bool > needToTrainCh;

	bool training;
};


#include "modelThread.h"



modelThread::modelThread(){

	//model = createEigenFaceRecognizer(80, 5000);
	model = createFisherFaceRecognizer(80, 5000);
	
	startThread();
	training = false;
}


modelThread::~modelThread(){

	trainedModel.close();
	needToTrainCh.close();
	waitForThread(true);
}


void modelThread::addSample(Mat face, int label) {
	modelFaces.push_back(face);
	modelLabels.push_back(label);
}


void modelThread::train() {

	if (modelFaces.size() > 0)
		needToTrainCh.send(true);
	else
		cout << "No Data" << endl;
}

void modelThread::predict(Mat &sample, int &match, double &confidence) {
	
	model->predict(sample, match, confidence);
	cout << "Predict results	match: " << match << " conf: " << confidence << endl;
}


void modelThread::forget() {
	modelFaces.clear();
	modelLabels.clear();
}

void modelThread::update() {
	trainedModel.tryReceive(model);
}




void modelThread::threadedFunction() {

	bool ntt;
	while (needToTrainCh.receive(ntt)) {
		training = true;
		cout << "Thread trigered" << endl;
		float timer = ofGetElapsedTimef();

		model->train(modelFaces, modelLabels);
		trainedModel.send(model);

		training = false;
		cout << "Threaded training took : " << (ofGetElapsedTimef() - timer) << " seconds" << endl;
	}
}

bool modelThread::isReady() {
	return !training ;
}


void modelThread::save() {
	model->save("data/model.xml");
	cout << "Model saved" << endl;
}

void modelThread::load() {
	model->load("data/model.xml");
	cout << "Model loaded" << endl;
}



void modelThread::test() {
	model->save("out.xml");
	//cout <<  << endl;
}
#pragma once
#include "ofMain.h"


class trainThread: public ofThread {

public:
	trainThread();
	~trainThread();

	void threadedFunction();
	void print();


	ofThreadChannel<int> input;
	ofThreadChannel<int> output;
	
	
};


#pragma once

#include "ofMain.h"

#define numReadings 50
class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);

		void audioIn(ofSoundBuffer & input);

	private:
		//initialize arduino variables
		ofArduino arduino;
		void setupArduino(const int& version);
		void analogPinChanged(const int& pin);
		int value;
		int drawValue;

		//intitialize audio variables
		ofSoundStream soundStream;
		int bufferSize;
		float rms = 0.0;
		float drawRms;

		//initialize variables for smoothing
		int readings[numReadings];      // the readings from the analog input
		int readIndex = 0;              // the index of the current reading
		int total = 0;                  // the running total
		int average = 0;

};	

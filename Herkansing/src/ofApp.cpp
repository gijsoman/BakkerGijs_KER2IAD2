#include "ofApp.h"

#define POTMETER 0
//--------------------------------------------------------------
void ofApp::setup(){
	//Setting the background
	ofBackground(ofColor::white);

	ofAddListener(arduino.EInitialized, this, &ofApp::setupArduino);

	arduino.connect("COM2");
	arduino.sendFirmwareVersionRequest();
	ofSetLogLevel(OF_LOG_NOTICE);

	//setting up the soundstream to work with.
	bufferSize = 512;
	soundStream.printDeviceList();
	soundStream.setDeviceID(0);
	soundStream.setup(this, 0, 2, 48000, bufferSize, 4);

	//initizialize all the readings to 0
	for (int thisReading = 0; thisReading < numReadings; thisReading++) {
		readings[thisReading] = 0;
	}
}

//--------------------------------------------------------------
void ofApp::update(){
	arduino.update();
	//logging rms * 50000 to get values i can work with
	//ofLog() << rms * 50000 << endl;

	//smoothen out the values from the mic
	total = total - readings[readIndex];
	readings[readIndex] = rms * 4000;
	total = total + readings[readIndex];
	readIndex = readIndex + 1;
	if (readIndex >= numReadings) {
		readIndex = 0;
	}
	drawRms = total / numReadings;

	//map the value of the potentio meter to the screen height
	drawValue = ofMap(value, 0, 1023, 0, ofGetWindowHeight());

	//dont draw outside the window
	if (drawRms > ofGetWindowHeight())
	{
		drawRms = ofGetWindowHeight();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){

	if (drawRms > drawValue)
	{
		ofSetColor(ofColor::red);
	}
	else
	{
		ofSetColor(ofColor::green);
	}



	ofDrawRectangle(0, ofGetWindowHeight(), ofGetWindowWidth(), -drawRms);

	ofSetLineWidth(200);
	ofSetColor(ofColor::black);
	ofDrawLine(0, ofGetWindowHeight() - drawValue, ofGetWindowWidth(), ofGetWindowHeight() - drawValue);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

void ofApp::audioIn(ofSoundBuffer & input)
{
	//calculating the root mean square to get the volume of the audio.
	int numCounted = 0;
	for (int i = 0; i < bufferSize; i++) {
		float leftSample = input[i * 2] * 0.5;
		float rightSample = input[i * 2 + 1] * 0.5;
		rms += leftSample * leftSample;
		rms += rightSample * rightSample;
		numCounted += 2;
	}
	rms /= (float)numCounted;
	rms = sqrt(rms);
}

void ofApp::setupArduino(const int& version) {
	// setting up the arduino stuff.
	ofLogNotice() << "Arduino initialized" << endl;
	ofRemoveListener(arduino.EInitialized, this, &ofApp::setupArduino);
	//setting the type of the pin
	arduino.sendAnalogPinReporting(POTMETER, ARD_ANALOG);

	ofAddListener(arduino.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}

void ofApp::analogPinChanged(const int& pin) {
	value = arduino.getAnalog(pin);
	ofLogVerbose() << "Analog pin" << pin << " changed to " << value << endl;
	if (pin == POTMETER) {
		//logging the value if the pin where the potmeter is connected to changes.
		ofLog() << value << endl;
	}
}


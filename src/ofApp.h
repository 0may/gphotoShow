#pragma once

#include "ofMain.h"
#include "ofxXmlSettings.h"

class ofApp : public ofBaseApp{
public:

	enum IrSensorState {
		LOW, HIGH
	};

	void setup();
	void update();
	void draw();

	bool captureImage();
	void drawImage();

	void exit();
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:
    void setupArduino(const int & version);
    void analogPinChanged(const int & pinNum);

private:
	ofxXmlSettings m_settings;  // xml settings instance
	string m_imgFolder;         // folder where images are to be stored
	int m_imgCount;             // image count for image file numbering
  char m_imgPath[1024];       // buffer for image file path
	char m_gphotoCmd[1096];     // buffer for gphoto command

	float m_dispW, m_dispH, m_dispAspect;          // width, height and aspect ratio of display
	float m_dispImgW, m_dispImgH, m_dispImgAspect; // width, height and aspect ratio of displayed image
	ofImage* m_pDispImg; // image object to be displayed
	ofImage* m_pLoadImg; // image object to load a new image
	bool m_busy;
	bool m_newImg;

	ofArduino m_arduino;        // arduino instance
	int m_valueA0;              // arduino's A0 value
	bool m_isArduinoConnected;
	bool m_isArduinoSetup;

	int m_threshold;       // sensor value threshold (0 - 830) for capture triggering
	IrSensorState m_state; // sensor's state: LOW if below threshold, HIGH otherwise

	float m_lastCaptureTriggered;  // time of last capture trigger
	bool m_captureEnabled;         // enables/disables capture
};

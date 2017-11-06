#pragma once

#include "ofMain.h"

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
	float m_dispW, m_dispH, m_dispAspect;          // width, height and aspect ratio of display
	float m_dispImgW, m_dispImgH, m_dispImgAspect; // width, height and aspect ratio of displayed image
	ofImage* m_pDispImg; // image object to be displayed
	ofImage* m_pLoadImg; // image object to load a new image
	bool m_busy;
	bool m_newImg;

	ofArduino m_arduino;
	int m_valueA0;
	bool m_arduinoSetup;

	int m_threshold;
	IrSensorState m_state;

	float m_lastCaptureTriggered;

	bool m_enableCapture;
};

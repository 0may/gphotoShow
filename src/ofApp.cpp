#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  m_pDispImg = new ofImage();
  m_pLoadImg = new ofImage();

  m_busy = false;
  m_newImg = false;
  m_arduinoSetup = false;
  m_enableCapture = false;

  m_valueA0 = -1;
  m_state = LOW;

  m_threshold = 400;

  m_lastCaptureTriggered = -1.0;

  m_arduino.connect("/dev/ttyACM0", 57600);

	// listen for EInitialized notification. this indicates that
	// the arduino is ready to receive commands and it is safe to
	// call setupArduino()
	ofAddListener(m_arduino.EInitialized, this, &ofApp::setupArduino);

  ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::update(){

  m_arduino.update();

  if (m_arduinoSetup && m_valueA0 > -1) {

    if (m_valueA0 >= m_threshold && m_state == LOW) {

      m_state = HIGH;
      m_arduino.sendDigital(13, 1);

    }
    else if (m_valueA0 < m_threshold && m_state == HIGH) {

      m_state = LOW;
      m_arduino.sendDigital(13, 0);

      if (m_enableCapture && ofGetElapsedTimef() - m_lastCaptureTriggered > 5.0f) {
        m_lastCaptureTriggered = ofGetElapsedTimef();
        captureImage();
      }
    }
  }
}

//--------------------------------------------------------------
void ofApp::draw(){

  m_dispW = ofGetWidth();
  m_dispH = ofGetHeight();
  m_dispAspect = m_dispW / m_dispH;

  drawImage();

  if (!m_arduinoSetup) {
    ofDrawBitmapStringHighlight("Initializing Arduino...", 200,200);
  }
  else if (!m_enableCapture) {

    ofDrawBitmapStringHighlight("CALIBRATION MODE", 200, 200);
    ofDrawBitmapStringHighlight("Press 'F1' to switch between CALIBRATION and CAPTURE MODE", 200,220);
    ofDrawBitmapStringHighlight("Press 'F5' to toggle between WINDOW and FULLSCREEN MODE", 200,240);
    ofDrawBitmapStringHighlight("Press 'F9' to EXIT", 200,260);
    ofDrawBitmapStringHighlight("Threshold:    " + ofToString(m_threshold) + "\nSensor value: " + ofToString(m_valueA0) + "\nSensor values range from 0 (no IR light) to 800 (high IR light intensity)", 200,300);

  }
}


//--------------------------------------------------------------
void ofApp::exit(){

  m_arduino.disconnect();

  delete m_pDispImg;
  delete m_pLoadImg;

}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
  //cout << key << endl;
  if (key == 265) {
    //while (m_busy);

    ofExit();
  }
  else if (key == 257 && m_arduinoSetup) {

    m_enableCapture = !m_enableCapture;

   // captureImage();
  }
  else if (key == 261) {
    ofToggleFullscreen();
  }
}


bool ofApp::captureImage() {

  bool ret = false;

  if (m_busy)
    return false;

  m_busy = true;

  ret = (system("gphoto2 --capture-image-and-download --force-overwrite --filename=data/latest.jpg") == 0);

  if (ret) {
    ret = m_pLoadImg->load("latest.jpg");

    m_pLoadImg->resize(m_dispW, m_dispW/m_dispImgAspect);
  }


  m_busy = false;
  m_newImg = true;

  return ret;
}


void ofApp::drawImage() {

  if (m_newImg) {
    m_busy = true;

    ofImage* pTmp = m_pDispImg;
    m_pDispImg = m_pLoadImg;
    m_pLoadImg = pTmp;

    m_dispImgW = m_pDispImg->getWidth();
    m_dispImgH = m_pDispImg->getHeight();
    m_dispImgAspect = m_dispImgW / m_dispImgH;

    m_busy = false;
    m_newImg = false;
  }

  if (m_pDispImg->isAllocated()) {

    float sx = 0.0f;
    float sy = 0.0f;
    float sw = m_dispImgW;
    float sh = m_dispImgH;

    if (m_dispImgAspect >= m_dispAspect) {
      sw = m_dispImgW * m_dispAspect/m_dispImgAspect;
      sx = 0.5*(m_dispImgW - sw);
    }
    else {
      sh = m_dispImgH * m_dispImgAspect/m_dispAspect;
      sy = 0.5*(m_dispImgH - sh);
    }

    m_pDispImg->drawSubsection(0, 0, m_dispW, m_dispH, sx, sy, sw, sh);
  }
  else {
    ofBackground(0, 60, 100);
  }
}


void ofApp::setupArduino(const int & version) { // taken from ofArduino example

  // remove listener because we don't need it anymore
  ofRemoveListener(m_arduino.EInitialized, this, &ofApp::setupArduino);

  // it is now safe to send commands to the Arduino
  m_arduinoSetup = true;

  // print firmware name and version to the console
  ofLogNotice() << m_arduino.getFirmwareName();
  ofLogNotice() << "firmata v" << m_arduino.getMajorFirmwareVersion() << "." << m_arduino.getMinorFirmwareVersion();

  // Note: pins A0 - A5 can be used as digital input and output.
  // Refer to them as pins 14 - 19 if using StandardFirmata from Arduino 1.0.
  // If using Arduino 0022 or older, then use 16 - 21.
  // Firmata pin numbering changed in version 2.3 (which is included in Arduino 1.0)
  m_arduino.sendDigitalPinMode(13, ARD_OUTPUT);

  // set pin A0 to analog input
  m_arduino.sendAnalogPinReporting(0, ARD_ANALOG);

  // Listen for changes on the digital and analog pins
  ofAddListener(m_arduino.EAnalogPinChanged, this, &ofApp::analogPinChanged);
}


void ofApp::analogPinChanged(const int & pinNum) {
  if (pinNum == 0)
    m_valueA0 = m_arduino.getAnalog(pinNum);
}



//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){

}

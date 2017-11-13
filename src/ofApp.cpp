#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
  m_pDispImg = new ofImage();
  m_pLoadImg = new ofImage();

  m_busy = false;
  m_newImg = false;
  m_isArduinoConnected = false;
  m_isArduinoSetup = false;
  m_captureEnabled = false;
  m_valueA0 = -1;
  m_state = LOW;
  m_threshold = 400;
  m_lastCaptureTriggered = -1.0;

  
  m_isArduinoConnected = m_arduino.connect("/dev/ttyACM0", 57600);

	// listen for EInitialized notification. this indicates that
	// the arduino is ready to receive commands and it is safe to
	// call setupArduino()
	ofAddListener(m_arduino.EInitialized, this, &ofApp::setupArduino);


  if (m_settings.loadFile("settings.xml")) {
    m_imgFolder = m_settings.getValue("gphotoShow:imageFolder", "FAILED");
    m_imgCount = m_settings.getValue("gphotoShow:imageCount", -1);

    cout << "Image folder: " << m_imgFolder << endl;
    cout << "Image count:  " << m_imgCount << endl;
  }
  else {
    m_imgFolder = "FAILED";
    m_imgCount = -1;
  }


  if (m_imgFolder != "FAILED" && m_imgCount > -1) {
    ofFile dir(m_imgFolder);

    if (!dir.exists()) {
      cout << "WARNING: Image folder '" << m_imgFolder
      << "' does not exist. Please create this folder and restart for images to be saved." << endl;

      m_imgCount = -1;
    }
  }
  else {
    cout << "WARNING: Failed to read valid settings from 'data/settings.xml'. Please re-evaluate your settings and restart for images to be saved." << endl;
    m_imgCount = -1;
  }

  ofHideCursor();
}

//--------------------------------------------------------------
void ofApp::update(){

  if (m_isArduinoConnected) {
    m_arduino.update();

    if (m_isArduinoSetup && m_valueA0 > -1) {

      if (m_valueA0 >= m_threshold && m_state == LOW) {

        // turn on Arduino's LED when IR intensity rises above threshold
        m_state = HIGH;
        m_arduino.sendDigital(13, 1);

      }
      else if (m_valueA0 < m_threshold && m_state == HIGH) {

        // turn off Arduino's LED when IR intensity falls below threshold
        m_state = LOW;
        m_arduino.sendDigital(13, 0);

        // capture image if capture is enabled and last image capture is older than 5 seconds
        if (m_captureEnabled && ofGetElapsedTimef() - m_lastCaptureTriggered > 5.0f) {
          m_lastCaptureTriggered = ofGetElapsedTimef();
          captureImage();
        }
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

  if (!m_isArduinoConnected) {
    ofDrawBitmapStringHighlight("ERROR: Could not connect to Arduino on Port '/dev/ttyACM0'", 200,200);
    ofDrawBitmapStringHighlight("Press 'SPACE' to trigger image capture for testing", 200,220);
  }
  else if (!m_isArduinoSetup) {
    ofDrawBitmapStringHighlight("Initializing Arduino...", 200,200);
  }
  else if (!m_captureEnabled) {

    ofDrawBitmapStringHighlight("CALIBRATION MODE", 200, 200);
    ofDrawBitmapStringHighlight("Press 'F1' to switch between CALIBRATION and CAPTURE MODE", 200,220);
    ofDrawBitmapStringHighlight("Press 'F5' to toggle between WINDOW and FULLSCREEN MODE", 200,240);
    ofDrawBitmapStringHighlight("Press 'F9' to EXIT", 200,260);
    ofDrawBitmapStringHighlight("Use ARROW keys to adjust THRESHOLD", 200,280);
    ofDrawBitmapStringHighlight("Threshold:    " + ofToString(m_threshold) + "\nSensor value: " + ofToString(m_valueA0), 200,320);

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

  if (key == 265) { // F9
    //while (m_busy);
    ofExit();
  }
  else if (key == 257 && m_isArduinoSetup) { // F1
    m_captureEnabled = !m_captureEnabled;
  }
  else if (key == 261) { // F5
    ofToggleFullscreen();
  }
  else if (key == 357 && !m_captureEnabled) { // UP ARROW
    m_threshold += 10;
  }
  else if (key == 357 && !m_captureEnabled) { // DOWN ARROW
    m_threshold -= 10;
  }
  else if (key == 357 && !m_captureEnabled) { // LEFT ARROW
    m_threshold += 1;
  }
  else if (key == 357 && !m_captureEnabled) { // RIGHT ARROW
    m_threshold -= 1;
  }
  else if (key == 32 && !m_isArduinoConnected) {
    // trigger capture by pressing SPACE when no Arduino is connected
    captureImage();
  }

  // limit threshold range
  if (m_threshold > 830)
    m_threshold = 830;
  else if (m_threshold < 0)
    m_threshold = 0;
}


bool ofApp::captureImage() {

  bool ret = false;

  if (m_busy)
    return false;

  m_busy = true;

  if (m_imgCount == -1) { // images will not be stored

    // try to capture image
    ret = (system("gphoto2 --capture-image-and-download --force-overwrite --filename=data/latest.jpg") == 0);

    if (ret) {
      // load captured image into image object
      ret = m_pLoadImg->load("latest.jpg");

      // resize image to display size due to problems with large DSLR images on Raspberry Pi
      m_pLoadImg->resize(m_dispW, m_dispW/m_dispImgAspect);
    }
  }
  else { // images will be stored in m_imgFolder with m_imgCount numbering
    sprintf(m_imgPath, "%s/img_%05d.jpg", m_imgFolder.c_str(), m_imgCount);
    sprintf(m_gphotoCmd, "gphoto2 --capture-image-and-download --force-overwrite --filename=%s", m_imgPath);

    ret = (system(m_gphotoCmd) == 0);

    if (ret) {
      m_imgCount++;
      m_settings.setValue("gphotoShow:imageCount", m_imgCount);
      m_settings.saveFile("settings.xml");

      // load captured image into image object
      ret = m_pLoadImg->load(m_imgPath);

      // resize image to display size due to problems with large DSLR images on Raspberry Pi
      m_pLoadImg->resize(m_dispW, m_dispW/m_dispImgAspect);
    }
  }


  m_busy = false;
  m_newImg = true;

  return ret;
}


void ofApp::drawImage() {

  if (m_newImg) {
    m_busy = true;

    // switch image objects for displaying and loading
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

    // compute image part to be displayed. required for differing aspect ratios of screen and image
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
  m_isArduinoSetup = true;

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

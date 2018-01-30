#include "ofMain.h"

class DataThread : public ofThread {

	public:
	
	void threadedFunction() {
		
		char cmd[1096];
		m_newImgAvailable = false;
	
		while (isThreadRunning()) {
		
			if (m_newImgAvailable) {
				
				lock();     
				sprintf(cmd, "cp data/latest.jpg %s", m_imgPath.c_str());
				unlock();
							
				system(cmd);
				
				lock();
				m_newImgAvailable = false;
				unlock();
			}
			else {
				sleep(200);
			}
		
		
		}
	
	
	}
	
	public:
		bool m_newImgAvailable;
		string m_imgPath;
	

};

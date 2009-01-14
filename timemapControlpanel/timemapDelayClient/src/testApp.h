#ifndef _TEST_APP
#define _TEST_APP

#define HISTORYSIZE 750
#define PICTURESIZE 1327104

#define OF_ADDON_USING_OFXOSC

#include "ofMain.h"
#include "ofAddons.h"
#include "sharedVariable.h"

class testApp : public ofSimpleApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();
	
		ofxOscReceiver	receiver;
		ofxOscSender sender;
		
		vector<SharedVariable> sharedVariables;

		float fade;
		
		ofVideoGrabber vidGrabber;
		unsigned char videoHistory[HISTORYSIZE][PICTURESIZE];

		int historyIndex;
		
		ofTexture videoTexture;
		int showIndex;

};

#endif
	

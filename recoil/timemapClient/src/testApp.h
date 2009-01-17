#ifndef _TEST_APP
#define _TEST_APP

#define HISTORYSIZE 750
#define PICTURESIZE 1327104

#define OF_ADDON_USING_OFXOSC
#define OF_ADDON_USING_OFXBULLET
#define OF_ADDON_USING_OFXOPENCV
#define OF_ADDON_USING_OFSHADER

//#include "ofShader.h"
#include "ofMain.h"
#include "ofAddons.h"
#include "sharedVariable.h"
#include "texts.h"



class testApp : public ofSimpleApp{
	
	public:
		
		void setup();
		void update();
		void draw();
		void drawViewport();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased();
		void bulletSetup();
	
		ofxOscReceiver	receiver;
		ofxOscSender sender;
		
		vector<SharedVariable> sharedVariables;

		float fade;
		
		/*ofVideoGrabber vidGrabber;
		unsigned char videoHistory[HISTORYSIZE][PICTURESIZE];
		int historyIndex;		
		ofTexture videoTexture;
		int showIndex;*/
	
		TextFontHolder * font1;
		Text* text, *text2, *text3;
	
	
		btDiscreteDynamicsWorld * dynamicsWorld;	
		
		btRigidBody * groundRigidBody;	
		btRigidBody **bodies, **bodies2, **bodies3;
		float *sphereRadiuses;

		int millisForUpdate;
	
	
		int					snapCounter;
		char 				snapString[255];
		ofImage 			img;
		bool				makeSnaps;
	
	
		ofxLight light1; //this will be a directional light
	bool bSmoothLight;

	

};

#endif
	

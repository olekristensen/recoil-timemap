#ifndef _TEST_APP
#define _TEST_APP

#define OF_ADDON_USING_OFXOSC
#define OF_ADDON_USING_OFXGUI
#define OF_ADDON_USING_OFXDIRLIST
#define OF_ADDON_USING_OFXVECTORMATH
#define OF_ADDON_USING_OFXXMLSETTINGS

#include "ofMain.h"
#include "ofAddons.h"
#include "sharedVariable.h"
#include "Client.h"

class testApp : public ofSimpleApp, public ofxGuiListener{
	enum
	{
		gui_panel1,
		gui_fade,
		gui_time

	};
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
	
		void handleGui(int parameterId, int task, void* data, int length);
		ofxGui* gui;
		float fade;

		
		ofxOscReceiver	receiver;
	
		vector<Client*> clients;
		Client testClient;
	
		vector<SharedVariable> sharedVariables;
	

		int counter;
		int test;
		int time;
		
};

#endif
	

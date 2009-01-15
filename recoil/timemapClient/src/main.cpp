#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main( ){

	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofSetupOpenGL(768,1024, OF_WINDOW);			// <-------- setup the GL context
	ofSetWindowPosition(+2000, 0);
	ofToggleFullscreen();
	// this kicks off the running of my app
	ofRunApp(new testApp);
	
}

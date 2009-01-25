#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main( ){

	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofEnableSmoothing();
	ofSetupOpenGL(3072,768, OF_WINDOW);			// <-------- setup the GL context
	ofSetWindowPosition(3000, 0);
	ofToggleFullscreen();
	// this kicks off the running of my app
	ofRunApp(new testApp);
	
}

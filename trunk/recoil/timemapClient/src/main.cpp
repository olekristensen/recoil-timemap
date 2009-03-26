#include "ofMain.h"
#include "testApp.h"

//========================================================================
int main( ){
	
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	//ofSetupOpenGL(1536,384, OF_WINDOW);
	ofSetupOpenGL(2400,600, OF_FULLSCREEN);			// <-------- setup the GL context
	ofSetWindowPosition(3000, 0);
	//glEnable(GL_MULTISAMPLE);
	glEnable(GL_MULTISAMPLE);
	//ofSetVerticalSync(true);
	//ofToggleFullscreen();
	// this kicks off the running of my app
	ofRunApp(new testApp);
	
}

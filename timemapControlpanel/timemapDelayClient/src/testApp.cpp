#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	 
	receiver.setup(8001);
	sender.setup("192.168.1.3", 8000);
	fade = 1.0;
	
	
	//Add sharedvariables
	sharedVariables.push_back(SharedVariable(&fade, "fade"));
	sharedVariables.push_back(SharedVariable(&showIndex, "time"));

	//Camreasetup
	vidGrabber.initGrabber(768,576);
	videoTexture.allocate(768,576, GL_RGB);

	historyIndex = 0;
	showIndex = 0;
}

//--------------------------------------------------------------
void testApp::update(){
	//Camerastuff
	vidGrabber.grabFrame();
	if (vidGrabber.isFrameNew()){
		int totalPixels = 768*576*3;
		unsigned char * pixels = vidGrabber.getPixels();
		for (int i = 0; i < totalPixels; i++){
			videoHistory[historyIndex][i] = pixels[i];
		}
		historyIndex++;
		if(historyIndex > HISTORYSIZE-1){
			historyIndex = 0;
		}
	}
//	if(lastShowIndex != showIndex){
	int index = historyIndex-showIndex-1;
	if(index < 0){	
		index = HISTORYSIZE + index;
	}
	videoTexture.loadData(videoHistory[index], 768,576, GL_RGB);
//	}
	
	
	
	//Update shared variables
	for(int i=0;i<sharedVariables.size();i++){
		sharedVariables[i].update(&sender);
	}
	
	//recieve osc messages
	while( receiver.hasWaitingMessages() )
	{
		ofxOscMessage m;
		receiver.getNextMessage( &m );		
		for(int i=0;i<sharedVariables.size();i++){
			sharedVariables[i].handleOsc(&m);
		}
	}
}

//--------------------------------------------------------------
void testApp::draw(){
	ofSetColor(fade*255.0,fade*255.0,fade*255.0 );
	videoTexture.draw(0,0);

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
}

//--------------------------------------------------------------
void testApp::keyReleased  (int key){ 
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mouseReleased(){

}

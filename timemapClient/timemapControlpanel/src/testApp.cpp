#include "testApp.h"

//Setup client id counter
int Client::next_id = 0;

//--------------------------------------------------------------
void testApp::setup(){	 
	ofSetVerticalSync(true);
	
	//Setup OSC communication to recieve from clients or controller
	receiver.setup(8000);
	
	//Setup clients
	testClient = Client(this,"localhost", 8001);
	clients.push_back(&testClient);
	
	//Add sharedvariables
	sharedVariables.push_back(SharedVariable(&fade, "fade", &testClient));
	sharedVariables.push_back(SharedVariable(&time, "time", &testClient));

	//Gui setup
	gui	= ofxGui::Instance(this);
	ofxGuiPanel* panel1 = gui->addPanel(gui_panel1, "General", 10, 10, OFXGUI_PANEL_BORDER, OFXGUI_PANEL_SPACING);
	panel1->addSlider(gui_fade, "Fade", 110, OFXGUI_SLIDER_HEIGHT, 0.0f, 1.0f, fade, kofxGui_Display_Float4, 0);
	panel1->addSlider(gui_time, "Time back", 110, OFXGUI_SLIDER_HEIGHT, 0.0f, 749.0f, time, kofxGui_Display_Int, 0);

	gui->forceUpdate(true);	
	gui->activate(true);


}

//--------------------------------------------------------------
void testApp::update(){
	//Update shared variables
	for(int i=0;i<sharedVariables.size();i++){
		sharedVariables[i].update();
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
	ofBackground(test, test,test);
	gui->draw();

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
	gui->mouseDragged(x, y, button);	

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	gui->mousePressed(x, y, button);	

}

//--------------------------------------------------------------
void testApp::mouseReleased(){
	gui->mouseReleased(mouseX, mouseY, 0);	


}
void testApp::handleGui(int parameterId, int task, void* data, int length)
{
	switch(parameterId)
	{
		case gui_fade:
			if(task == kofxGui_Set_Float)
				fade = *(float*)data;
			break;
		case gui_time:
			if(task == kofxGui_Set_Float){
				time = *(float*)data;
				cout<<time<<endl;
			}
			break;
	}
}

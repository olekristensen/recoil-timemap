#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){	 
	//General setup
	ofBackground(0, 0, 0);
	ofSetFrameRate(100);

	
	//OSC Setup
//	receiver.setup(8001);
//	sender.setup("192.168.1.3", 8000);

	//Add sharedvariables
	fade = 1.0;
	sharedVariables.push_back(SharedVariable(&fade, "fade"));	
	//sharedVariables.push_back(SharedVariable(&showIndex, "time"));

	//Camreasetup
	/*vidGrabber.initGrabber(768,576);
	videoTexture.allocate(768,576, GL_RGB);
	historyIndex = 0;
	showIndex = 0;*/
	
	
	//---Texts---
	//font
	font1 = new TextFontHolder("ArnoPro-Display.otf");
	text = new Text("MotionStates are a way for Bullet to do all the hard work for you getting the objects being simulated into the rendering part of your program. In most situations, your game loop would iterate through all the objects you're simulating before each frame render.", font1,  0, 0, 700);
	cout << "text height " << text->getHeight() << endl;
	//text->setLineHeight(1.0);
	text->setPosition(100,ofGetHeight()-text->getHeight(),0,600);
	cout << "text height " << text->getHeight() << endl;
	
	bulletSetup();
	
	
}

void testApp::bulletSetup(){
	btVector3 worldAabbMin(0,0,0);
	btVector3 worldAabbMax(ofGetWidth(), ofGetHeight(), 1000);
	int maxProxies = 1024;
	btAxisSweep3* broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);

	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0,10,0));

	
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,-1,0),1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(ofGetWidth()/2,ofGetHeight(),0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);
	int numLetters = text->getNumberLetters();
	bodies = new btRigidBody*[numLetters];
//	cout<<text->getNumberLetters()<<endl;
	for(int i=0; i<numLetters; i++){
		Letter * l = text->getLetter(i);
		btCollisionShape* fallShape = new btBoxShape(btVector3((text->getLetter(i)->getWidth()-2)/2.0,(text->getLetter(i)->getFont()->getCalculatedHeight())/2.0,(2.5)/2.0));
		btVector3 pos = btVector3(l->getLoc().x, l->getLoc().y, l->getLoc().z);

		btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),pos));
		btScalar mass = 1;
		btVector3 fallInertia(0,0,0);
		fallShape->calculateLocalInertia(mass,fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
		bodies[i] = new btRigidBody(fallRigidBodyCI);
		//dynamicsWorld->addRigidBody(bodies[i]);
	}

}

//--------------------------------------------------------------
void testApp::update(){
//	cout<<" --- "<<endl;
	int numLetters = text->getNumberLetters();

	
	dynamicsWorld->stepSimulation(2.0f/100.0f, 10);
	btTransform trans[numLetters];
	btVector3 pos[numLetters];
	btMatrix3x3 basis[numLetters];	

	for(int i=0; i<numLetters; i++){
		bodies[i]->getMotionState()->getWorldTransform(trans[i]);
		pos[i] = trans[i].getOrigin();
		basis[i] = trans[i].getBasis();
	}
	
	#pragma omp parallel for 
	for(int i=0; i<numLetters; i++){	
		Letter * l = text->getLetter(i);
		l->setPosition(pos[i].getX(), pos[i].getY(), pos[i].getZ());
		l->matrix[	0] = basis[i].getRow(0)[0];
		l->matrix[4] = basis[i].getRow(0)[1];
		l->matrix[8] = basis[i].getRow(0)[2];
		l->matrix[12] = 0;
		l->matrix[1] = basis[i].getRow(1)[0];
		l->matrix[5] = basis[i].getRow(1)[1];
		l->matrix[9] = basis[i].getRow(1)[2];
		l->matrix[13] = 0;
		l->matrix[2] = basis[i].getRow(2)[0];
		l->matrix[6] = basis[i].getRow(2)[1];
		l->matrix[10] = basis[i].getRow(2)[2];
		l->matrix[14] = 0;
		l->matrix[3] = 0;
		l->matrix[7] = 0;
		l->matrix[11] = 0;
		l->matrix[15] = 1;
	}


	
	
	//Camerastuff
	/*vidGrabber.grabFrame();
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
	*/
	
	
	//Update shared variables
	/*for(int i=0;i<sharedVariables.size();i++){
		sharedVariables[i].update(&sender);
	}*/
	
	//recieve osc messages
/*	while( receiver.hasWaitingMessages() )
	{
		ofxOscMessage m;
		receiver.getNextMessage( &m );		
		for(int i=0;i<sharedVariables.size();i++){
			sharedVariables[i].handleOsc(&m);
		}
	}*/
}

//--------------------------------------------------------------
void testApp::draw(){
  
	glOrtho(-1.0, 1.0, -1.0, 1.0, 5, 100);
	//glEnable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	
	/**
	// Draw Ground
	
	btTransform trans;
	btVector3 pos;
	btQuaternion rot;
	btMatrix3x3 basis;
	
	groundRigidBody->getMotionState()->getWorldTransform(trans);
	pos = trans.getOrigin();	
	
	ofSetRectMode(OF_RECTMODE_CENTER);
	glPushMatrix();
	glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	glColor3f(0.2f, 0.75f, 0.0f);
	glRotatef(90, 1.0f, 0.0f, 0.0f);
	ofRect(0, 0, ofGetWidth(), ofGetWidth());
	glPopMatrix();
	glPopMatrix();
	
	// End Draw Ground
	**/ 
	
	//Draw Letters
	
	ofSetColor(fade*255.0,fade*255.0,fade*255.0 );
	ofSetColor(255, 255, 255);
	
	glDisable(GL_DEPTH_TEST);
	text->draw();
	
	//videoTexture.draw(0,0);
	
	
	
	glPushMatrix();
//	glTranslatef(0, 0, -500);

/*
	for(int i=0; i<text->getNumberLetters(); i++){
		bodies[i]->getMotionState()->getWorldTransform(trans);
		pos = trans.getOrigin();
		rot= trans.getRotation();
		basis = trans.getBasis();
		glPushMatrix();
		glTranslatef(pos.getX(), pos.getY(), pos.getZ());
	//	glRotatef(rot.w()*360.0, rot.x(), rot.y(), rot.z());
		GLdouble myMatrix[16]; 
		myMatrix[0] = basis.getRow(0)[0];
		myMatrix[4] = basis.getRow(0)[1];
		myMatrix[8] = basis.getRow(0)[2];
		myMatrix[12] = 0;
		myMatrix[1] = basis.getRow(1)[0];
		myMatrix[5] = basis.getRow(1)[1];
		myMatrix[9] = basis.getRow(1)[2];
		myMatrix[13] = 0;
		myMatrix[2] = basis.getRow(2)[0];
		myMatrix[6] = basis.getRow(2)[1];
		myMatrix[10] = basis.getRow(2)[2];
		myMatrix[14] = 0;
		myMatrix[3] = 0;
		myMatrix[7] = 0;
		myMatrix[11] = 0;
		myMatrix[15] = 1;
		glMultMatrixd(myMatrix);
		
		glColor3f(1.0f, 1.0f, 1.0f);
		glBegin(GL_QUADS);
		
		glVertex3f(-text->getLetter(i)->getWidth()/2.0, -(text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, 0);
		glVertex3f(-text->getLetter(i)->getWidth()/2.0, (text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, 0);
		glVertex3f(text->getLetter(i)->getWidth()/2.0, (text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, 0);
		glVertex3f(text->getLetter(i)->getWidth()/2.0, -(text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, 0);
		glEnd();

		glBegin(GL_QUADS);
		glVertex3f(-text->getLetter(i)->getWidth()/2.0, -(text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, -10);
		glVertex3f(-text->getLetter(i)->getWidth()/2.0, (text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, -10);
		glVertex3f(text->getLetter(i)->getWidth()/2.0, (text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, -10);
		glVertex3f(text->getLetter(i)->getWidth()/2.0, -(text->getLetter(i)->getFont()->getLineHeight()+5)/2.0, -10);
		glEnd();
		

		glPopMatrix();
	}
	*/


	// Draw Frame
	
	ofSetColor(30, 30, 30);
	ofLine(1, 1, ofGetWidth()-1, 1);
	ofLine(ofGetWidth(), ofGetHeight()-1, 0, ofGetHeight()-1);
	
	

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	int numLetters = text->getNumberLetters();
	for(int i=0; i<numLetters; i++){

		dynamicsWorld->addRigidBody(bodies[i]);
	}
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

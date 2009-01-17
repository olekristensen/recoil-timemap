#include "testApp.h"


#include <FTGL/ftgl.h>
//--------------------------------------------------------------
void testApp::setup(){	 
	//General setup
	
	ofBackground(255, 255, 255);
	ofSetFrameRate(100);
	makeSnaps = false;
	memset(snapString, 0, 255);		// clear the string by setting all chars to 0
	snapCounter = 0;
	
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
 	font1 = new TextFontHolder("ArnoPro-Display.otf", 30);
	text = new Text();
	text->setText("Lorem ipsum dolor sit amet, consectetur adipisicing elit, sed do eiusmod tempor labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.");
	text->setFont(font1);
	text->setWordBlocks(true);
	text->setDepth(40);
	text->setWidth(600);
	text->setLineHeight(30*1.0);
	text->constructText();

	text->translate(100,ofGetHeight()-text->getHeight(),0);
	
	
	text2 = new Text();
	text2->setText("Autem molestie cu ius, ad vel dicit numquam philosophia, ius labores ponderum sadipscing an.  ");
	text2->setWordBlocks(true);
	text2->setFont(font1);
	text2->setDepth(40);
	text2->setWidth(600);
	text2->setLineHeight(30*1.2);
	text2->constructText();
	text2->translate(-20+ofGetHeight(),ofGetHeight()-text2->getHeight(),10);
	
	text3 = new Text();
	text3->setText("Has cu etiam legere iuvaret, pri malorum fuisset tibique ea. Has ipsum dolor ut, has aeque numquam inciderint eu. Sit facilisi mnesarchum vituperata no.  consectetur adipisicing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Audire dolorem interesset sit ad, eos ullum nostrud epicuri ea, vel eripuit facilisi menandri ut.");
	text3->setWordBlocks(true);
	text3->setFont(font1);
	text3->setDepth(40);
	text3->setWidth(600);
	text3->setLineHeight(30*1.0);
	text3->constructText();
	text3->translate(100+ofGetHeight()*2,ofGetHeight()-text3->getHeight(),0);

	bulletSetup();
	
	millisForUpdate = ofGetElapsedTimeMillis();
	
	
	//---
	//Light
	bSmoothLight = true;
	//reflexions!!
	ofxMaterialSpecular(25, 25, 25); //how much specular light will be reflect by the surface
	ofxMaterialShininess(100); //how concentrated the reflexion will be (between 0 and 128
	light1.specular(0, 0, 0);
	light1.diffuse(255,255,255);
	light1.ambient(0,0,0); //this basically tints everything with its color, by default is 0,0,0.

	ofxSetSmoothLight(true);

}

void testApp::bulletSetup(){
	btVector3 worldAabbMin(0,0,0);
	btVector3 worldAabbMax(ofGetWidth(), ofGetHeight(), 1000);
	int maxProxies = 1024;
//	btAxisSweep3* broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,1000,0));
//Ground 	
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,-1,0),1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(ofGetWidth()/2.0,ofGetHeight(),0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

//Collider
	btCollisionShape* fallShape = new btBoxShape(btVector3(60,60,6000));
	btVector3 pos = btVector3(0,0,0);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),pos));
	btScalar mass = 10;
	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(mass,fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
	collider = new btRigidBody(fallRigidBodyCI);
	collider->setCollisionFlags( collider->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	collider->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(collider);
	
//Letters
	int numLetters = text->getNumberLetters();
	bodies = new btRigidBody*[numLetters];
	for(int i=0; i<numLetters; i++){
		Letter * l = text->getLetter(i);
		btCollisionShape* fallShape = new btBoxShape(btVector3((text->getLetter(i)->getWidth()-1)/2.0,(l->getFont()->getCalculatedHeight()/2.0),text->getLetter(i)->getDepth()/2.0));
		btVector3 pos = btVector3(l->getLoc().x, l->getLoc().y, l->getLoc().z);
		btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),pos));
		btScalar mass = 10;
		btVector3 fallInertia(0,0,0);
		fallShape->calculateLocalInertia(mass,fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
		bodies[i] = new btRigidBody(fallRigidBodyCI);
		bodies[i]->setDamping(0.9,0.9);
//		dynamicsWorld->addRigidBody(bodies[i]);
	}
	
	numLetters = text2->getNumberLetters();
	bodies2 = new btRigidBody*[numLetters];
	for(int i=0; i<numLetters; i++){
		Letter * l = text2->getLetter(i);
		btCollisionShape* fallShape = new btBoxShape(btVector3((text2->getLetter(i)->getWidth()-1)/2.0,(l->getFont()->getCalculatedHeight()/2.0),text2->getLetter(i)->getDepth()/2.0));
		btVector3 pos = btVector3(l->getLoc().x, l->getLoc().y, l->getLoc().z);
		btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),pos));
		btScalar mass = 100;
		btVector3 fallInertia(0,0,0);
		fallShape->calculateLocalInertia(mass,fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
		bodies2[i] = new btRigidBody(fallRigidBodyCI);
		bodies[i]->setDamping(0.9,0.9);
		//		dynamicsWorld->addRigidBody(bodies[i]);
	}
	
	numLetters = text3->getNumberLetters();
	bodies3 = new btRigidBody*[numLetters];
	for(int i=0; i<numLetters; i++){
		Letter * l = text3->getLetter(i);
		btCollisionShape* fallShape = new btBoxShape(btVector3((text3->getLetter(i)->getWidth()-1)/2.0,(l->getFont()->getCalculatedHeight()/2.0),text3->getLetter(i)->getDepth()/2.0));
		btVector3 pos = btVector3(l->getLoc().x, l->getLoc().y, l->getLoc().z);
		btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),pos));
		btScalar mass = 10;
		btVector3 fallInertia(0,0,0);
		fallShape->calculateLocalInertia(mass,fallInertia);
		btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
		bodies3[i] = new btRigidBody(fallRigidBodyCI);
		bodies[i]->setDamping(0.9,0.9);			
		//dynamicsWorld->addRigidBody(bodies[i]);
	}
}

//--------------------------------------------------------------
void testApp::update(){
	int numLetters = text->getNumberLetters();
	int t = ofGetElapsedTimeMillis();
//	dynamicsWorld->stepSimulation(1.0/60.0,100);
	dynamicsWorld->stepSimulation((ofGetElapsedTimeMillis()-millisForUpdate)/1000.0f, 20, btScalar(1.)/btScalar(150.));
//	cout << ((ofGetElapsedTimeMillis()-millisForUpdate)/1000.0) << " < " << 20 * (1.0/50.0) << endl;
	millisForUpdate = ofGetElapsedTimeMillis();
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
		if(i == 0) {
			//cout << l->getLetter() << " " << pos[i].getX() << ", " << pos[i].getY() << ", " << pos[i].getZ() << endl;
		}
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
	
	numLetters = text2->getNumberLetters();
	dynamicsWorld->stepSimulation((ofGetElapsedTimeMillis()-millisForUpdate)/1000.0f, 20, btScalar(1.)/btScalar(150.));
	millisForUpdate = ofGetElapsedTimeMillis();
	
	for(int i=0; i<numLetters; i++){
	bodies2[i]->applyCentralForce(btVector3(-100000,0,0));

		bodies2[i]->getMotionState()->getWorldTransform(trans[i]);
		pos[i] = trans[i].getOrigin();
		basis[i] = trans[i].getBasis();
	}
	
#pragma omp parallel for 
	for(int i=0; i<numLetters; i++){
		Letter * l = text2->getLetter(i);
		l->setPosition(pos[i].getX(), pos[i].getY(), pos[i].getZ());
		if(i == 0) {
			//cout << l->getLetter() << " " << pos[i].getX() << ", " << pos[i].getY() << ", " << pos[i].getZ() << endl;
		}
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
	
	numLetters = text3->getNumberLetters();
	dynamicsWorld->stepSimulation((ofGetElapsedTimeMillis()-millisForUpdate)/1000.0f, 20, btScalar(1.)/btScalar(150.));
	//	cout << ((ofGetElapsedTimeMillis()-millisForUpdate)/1000.0) << " < " << 20 * (1.0/150.0) << endl;
	millisForUpdate = ofGetElapsedTimeMillis();

	
	for(int i=0; i<numLetters; i++){
		bodies3[i]->getMotionState()->getWorldTransform(trans[i]);
		pos[i] = trans[i].getOrigin();
		basis[i] = trans[i].getBasis();
	}
	
#pragma omp parallel for 
	for(int i=0; i<numLetters; i++){
		Letter * l = text3->getLetter(i);
		l->setPosition(pos[i].getX(), pos[i].getY(), pos[i].getZ());
		if(i == 0) {
			//cout << l->getLetter() << " " << pos[i].getX() << ", " << pos[i].getY() << ", " << pos[i].getZ() << endl;
		}
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


//Collider
//	collider->translate(btVector3(100,100,0));

	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),btVector3(mouseX,mouseY,0)));
	collider->setMotionState(fallMotionState);

	//collider->setPo
	
	
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

void testApp::draw(){
	//OpenGL stuff
	glEnable(GL_DEPTH_TEST);
	glOrtho(-1.0, 1.0, -1.0, 1.0, 50, 100);
	//ofBackground(255, 255, 255);	
	
//Lights
	float L1DirectionX = 0.4;
	float L1DirectionY = -0.4;
	float L1DirectionZ = -1.0;
	
	//light1.directionalLight(10, 10, 10, L1DirectionX, L1DirectionY, L1DirectionZ);
	light1.pointLight(15, 15, 15, 1000, ofGetHeight(), -100);

	ofxLightsOn(); //turn lights on

#ifdef USE_TRIPLEHEAD
//The screens
	glPushMatrix();
	GLfloat myMatrix[16];
	for(int i = 0; i < 16; i++){
		if(i % 5 != 0) myMatrix[i] = 0.0;
		else myMatrix[i] = 1.0;
	}
	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];	
	cvdst[0].x = 0;
	cvdst[0].y = 0;
	cvdst[1].x = ofGetWidth()/3.0;
	cvdst[1].y = 0;
	cvdst[2].x = ofGetWidth()/3.0;
	cvdst[2].y = ofGetHeight();
	cvdst[3].x = 0;
	cvdst[3].y = ofGetHeight();		
	
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = ofGetWidth();
	cvsrc[2].y = 0;
	cvsrc[3].x = ofGetWidth();
	cvsrc[3].y = ofGetHeight();
	cvsrc[0].x = 0;
	cvsrc[0].y = ofGetHeight();		

	CvMat * translate = cvCreateMat(3,3,CV_32FC1);
	CvMat* src_mat = cvCreateMat( 4, 2, CV_32FC1 );
	CvMat* dst_mat = cvCreateMat( 4, 2, CV_32FC1 );
	cvSetData( src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvSetData( dst_mat, cvdst, sizeof(CvPoint2D32f));
	cvFindHomography(src_mat, dst_mat, translate);
	float *matrix = translate->data.fl;
	myMatrix[0]		= matrix[0];
	myMatrix[4]		= matrix[1];
	myMatrix[12]	= matrix[2];	
	myMatrix[1]		= matrix[3];
	myMatrix[5]		= matrix[4];
	myMatrix[13]	= matrix[5];		
	myMatrix[3]		= matrix[6];
	myMatrix[7]		= matrix[7];
	myMatrix[15]	= matrix[8];			
	glMultMatrixf(myMatrix);
	glScaled(4.0, 1.0, 1.0); 
	drawViewport();
	glPopMatrix();	
//Screen 2
	glPushMatrix();
	glTranslatef( ofGetWidth()/3.0, 0, 0);
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = ofGetWidth();
	cvsrc[2].y = 0;
	cvsrc[3].x = ofGetWidth();
	cvsrc[3].y = ofGetHeight();
	cvsrc[0].x = 0;
	cvsrc[0].y = ofGetHeight();			
	cvSetData( src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvFindHomography(src_mat, dst_mat, translate);
	matrix = translate->data.fl;
	myMatrix[0]		= matrix[0];
	myMatrix[4]		= matrix[1];
	myMatrix[12]	= matrix[2];	
	myMatrix[1]		= matrix[3];
	myMatrix[5]		= matrix[4];
	myMatrix[13]	= matrix[5];		
	myMatrix[3]		= matrix[6];
	myMatrix[7]		= matrix[7];
	myMatrix[15]	= matrix[8];		
	glMultMatrixf(myMatrix);
	glScaled(4.0, 1.0, 1.0); 
	glTranslatef(-ofGetHeight(), 0, 0);
	drawViewport();
	glPopMatrix();	
//Screen 3	
	glPushMatrix();	
	glTranslatef( 2*ofGetWidth()/3.0, 0, 0);	
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = ofGetWidth();
	cvsrc[2].y = 0;
	cvsrc[3].x = ofGetWidth();
	cvsrc[3].y = ofGetHeight();
	cvsrc[0].x = 0;
	cvsrc[0].y = ofGetHeight();		
	cvSetData( src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvFindHomography(src_mat, dst_mat, translate);
	matrix = translate->data.fl;
	myMatrix[0]		= matrix[0];
	myMatrix[4]		= matrix[1];
	myMatrix[12]	= matrix[2];	
	myMatrix[1]		= matrix[3];
	myMatrix[5]		= matrix[4];
	myMatrix[13]	= matrix[5];		
	myMatrix[3]		= matrix[6];
	myMatrix[7]		= matrix[7];
	myMatrix[15]	= matrix[8];		
	glMultMatrixf(myMatrix);
	glScaled(4.0, 1.0, 1.0); 
	glTranslatef(-ofGetHeight()*2, 0, 0);
	drawViewport();
	glPopMatrix();
#else
	
	drawViewport();
	
#endif
	
}

//--------------------------------------------------------------
void testApp::drawViewport(){
	ofBackground(0, 0, 0);
	ofSetColor(255, 255, 255);
	//ofRect(0,0,ofGetWidth(), ofGetHeight());
	ofEnableAlphaBlending();
	glAlphaFunc(GL_GREATER, 0.99);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	ofDisableAlphaBlending();
	
	//Draw Letters	
	ofSetColor(255, 255, 255);
	
	//glDisable(GL_DEPTH_TEST);
	text->drawText();
	text2->drawText();
	text3->drawText();
	//glEnable(GL_DEPTH_TEST);
	
	
	
	//glDisable(GL_ALPHA_TEST);
	text->drawBricks();
	text2->drawBricks();
	text3->drawBricks();
	//glEnable(GL_ALPHA_TEST);

	btTransform trans;
	btVector3 pos;
	btMatrix3x3 basis;		
	collider->getMotionState()->getWorldTransform(trans);
	pos = trans.getOrigin();
	//basis[i] = trans[i].getBasis();
	glPushMatrix();
	glTranslatef((float)pos.getX(),pos.getY(), 300);
	ofSetColor(255, 0, 0);
	ofCircle(0,0,30);
		
	glPopMatrix();
	//videoTexture.draw(0,0);	
	
	// Draw Frame
	if(makeSnaps){
	// grab a rectangle at 200,200, width and height of 300,180
		img.grabScreen(0,0,ofGetWidth(),ofGetHeight());
		char fileName[255];
		sprintf(fileName, "snapshot_%0.3i.png", snapCounter);
		img.saveImage(fileName);
		sprintf(snapString, "saved %s", fileName);
		snapCounter++;
	}
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	int numLetters = text->getNumberLetters();
	for(int i=0; i<numLetters; i++){
		dynamicsWorld->addRigidBody(bodies[i]);
	}
	numLetters = text2->getNumberLetters();
	for(int i=0; i<numLetters; i++){
		dynamicsWorld->addRigidBody(bodies2[i]);
	}
	numLetters = text3->getNumberLetters();
	for(int i=0; i<numLetters; i++){
		dynamicsWorld->addRigidBody(bodies3[i]);
	}
	//makeSnaps = true;
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
	ofxSetSmoothLight(false);

}

//--------------------------------------------------------------
void testApp::mouseReleased(){
//	ofxSetSmoothLight(true);
}
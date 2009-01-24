#include "testApp.h"


#include <FTGL/ftgl.h>
//--------------------------------------------------------------
void testApp::setup(){	 
	//General setup
	//ofHideCursor();
	ofBackground(0, 0, 0);
	ofSetFrameRate(60);
	makeSnaps = false;
	memset(snapString, 0, 255);		// clear the string by setting all chars to 0
	snapCounter = 0;
	
	//OSC Setup
	receiver.setup(8001);
	sender.setup("localhost", 8000);
	
	ofxOscMessage m;
	m.setAddress( ("/sendVariables/please"));
	m.addIntArg(1);
	sender.sendMessage( m );

	//Add sharedvariables

	for(int i=0;i<3;i++){
		cameraCorners[i][0] = ofPoint(0.0,0.0);
		cameraCorners[i][1] = ofPoint(1.0, 0.0);
		cameraCorners[i][2] = ofPoint(1.0, 1.0);
		cameraCorners[i][3] = ofPoint(0.0, 1.0);
		for(int u=0;u<4;u++){
			sharedVariables.push_back(SharedVariable(&cameraCorners[i][u].x, "cameraCornerx"+ofToString(i, 0)+ofToString(u, 0)));
			sharedVariables.push_back(SharedVariable(&cameraCorners[i][u].y, "cameraCornery"+ofToString(i, 0)+ofToString(u, 0)));
		}
	}	
	for(int i=0;i<3;i++){
		projectorCorners[i][0] = ofPoint(0.0,0.0);
		projectorCorners[i][1] = ofPoint(1.0, 0.0);
		projectorCorners[i][2] = ofPoint(1.0, 1.0);
		projectorCorners[i][3] = ofPoint(0.0, 1.0);
		for(int u=0;u<4;u++){
			sharedVariables.push_back(SharedVariable(&projectorCorners[i][u].x, "projectorCornerx"+ofToString(i, 0)+ofToString(u, 0)));
			sharedVariables.push_back(SharedVariable(&projectorCorners[i][u].y, "projectorCornery"+ofToString(i, 0)+ofToString(u, 0)));
		}
	}
	fade = 1.0;
	sharedVariables.push_back(SharedVariable(&fade, "fade"));	
	//sharedVariables.push_back(SharedVariable(&showIndex, "time"));

	//Camreasetup
/*	vidGrabber.initGrabber(768,576);
	videoTexture.allocate(768,576, GL_RGB);
	historyIndex = 0;
	showIndex = 0;*/
	
//Tracker camera 

	// videoTexture.allocate(768,576, GL_RGB);

	
	bulletSetup();
	
	//---Texts---
	//font
 	font1 = new TextFontHolder("ArnoPro-Display.otf", 50);
	text = new Text();
	text->setText("Når andre kan læse ens tanker, hvordan undgår man så at vise sine tanker - man kan ikke helt lade være at tænke - men man kan tænke på noget andet, dække sine egne tanker gennem bevidst og konsekvent at referere andres mulige tanker? Eller? Hvordan forløber et forhør under de omstændigheder? Intet behøver at blive sagt.");
	text->setFont(font1);
	text->setWordBlocks(true);
	text->setDepth(60);
	text->setWidth(600);
	text->setLineHeight(50*1.0);
	text->constructText();
	text->translate(100,ofGetWidth()/3.0-text->getHeight()-100,0);
	text->setupBullet(dynamicsWorld, &bodies);

	
	text2 = new Text();
	text2->setText("  ");
	text2->setWordBlocks(true);
	text2->setFont(font1);
	text2->setDepth(20);
	text2->setWidth(600);
	text2->setLineHeight(30*1.0);
	text2->constructText();
	text2->translate(100+ofGetHeight(),ofGetWidth()/3.0-text2->getHeight()-100,10);
	text2->setupBullet(dynamicsWorld, &bodies);

	text3 = new Text();
	text3->setText("Has cu etiam legere iuvaret, pri malorum fuisset tibiqu");
	text3->setWordBlocks(true);
	text3->setFont(font1);
	text3->setDepth(20);
	text3->setWidth(600);
	text3->setLineHeight(30*1.0);
	text3->constructText();
	text3->translate(100+ofGetHeight()*2,ofGetWidth()/3.0-text3->getHeight()-100,0);
	text3->setupBullet(dynamicsWorld, &bodies);

	
	millisForUpdate = ofGetElapsedTimeMillis();
	
	
	//---
	//Light
	bSmoothLight = true;
	//reflexions!!
	ofxMaterialSpecular(50, 50, 50); //how much specular light will be reflect by the surface
	ofxMaterialShininess(25); //how concentrated the reflexion will be (between 0 and 128
	light1.specular(0, 0, 0);
	light1.diffuse(255,255,255);
	light1.ambient(255,255,255); //this basically tints everything with its color, by default is 0,0,0.
	//light2.specular(0, 0, 0);
	//light2.diffuse(255,255,255);
	//light2.ambient(0,0,0); //this basically tints everything with its color, by default is 0,0,0.
	//light3.specular(0, 0, 0);
	//light3.diffuse(255,255,255);
	//light3.ambient(0,0,0); //this basically tints everything with its color, by default is 0,0,0.
	ofxSetSmoothLight(true);
	
	vidTracker.setVerbose(true);
	vidTracker.initGrabber(720,576);
	
	colorImg.allocate(720,576);
	camera1.setup(&vidTracker, &trackerTexture, &colorImg,  0,0,vidTracker.width/2, vidTracker.height/2);
	cornerWarperIndex = -1;
	
	debug = false;
}

static inline btScalar	UnitRand()
{
	return(rand()/(btScalar)RAND_MAX);
}
static inline btScalar	SignedUnitRand()
{
	return(UnitRand()*2-1);
}
static inline btVector3	Vector3Rand()
{
	const btVector3	p=btVector3(SignedUnitRand(),SignedUnitRand(),SignedUnitRand());
	return(p.normalized());
}


//--------------------------------------------------------------
void testApp::bulletSetup(){
	btVector3 worldAabbMin(0,0,0);
	btVector3 worldAabbMax(ofGetWidth(), ofGetHeight(), 1000);
	int maxProxies = 2048;
//	btAxisSweep3* broadphase = new btAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
	btDbvtBroadphase* broadphase = new btDbvtBroadphase();
	btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
	btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);
	btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0,10,0));

//Ground 	
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,-1,0),1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3((ofGetHeight()/2.0)/100.0,(ofGetWidth()/3.0)/100.0,0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	dynamicsWorld->addRigidBody(groundRigidBody);

//Collider
	btCollisionShape* fallShape = new btBoxShape(btVector3(0.1,0.1,6000));
	btVector3 pos = btVector3(1,1,0);
	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),pos));
	btScalar mass = 20.;
	btVector3 fallInertia(0,0,0);
	fallShape->calculateLocalInertia(mass,fallInertia);
	btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
	collider = new btRigidBody(fallRigidBodyCI);
	collider->setGravity(btVector3(0.,0.,0.));
	collider->setDamping(0.99,0.9);
	collider->setCollisionFlags( collider->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	collider->setActivationState(DISABLE_DEACTIVATION);
	dynamicsWorld->addRigidBody(collider);
	
/*	//TRACEDEMO
	const btScalar	s=2;
	const btScalar	h=10;
	const int		segments=6;
	const int		count=50;
	for(int i=0;i<count;++i)
	{
		btSoftBody*		psb=btSoftBodyHelpers::CreatePatch(m_softBodyWorldInfo,btVector3(-s,h,-s),
														   btVector3(+s,h,-s),
														   btVector3(-s,h,+s),
														   btVector3(+s,h,+s),
														   segments,segments,
														   0,true);
		btSoftBody::Material*	pm=psb->appendMaterial();
		pm->m_flags				-=	btSoftBody::fMaterial::DebugDraw;
		psb->generateBendingConstraints(2,pm);
		psb->m_cfg.kLF			=	0.004;
		psb->m_cfg.kDG			=	0.0003;
		psb->m_cfg.aeromodel	=	btSoftBody::eAeroModel::V_TwoSided;
		btTransform		trs;
		btQuaternion	rot;
		btVector3		ra=Vector3Rand()*0.1;
		btVector3		rp=Vector3Rand()*15+btVector3(0,20,80);
		rot.setEuler(SIMD_PI/8+ra.x(),-SIMD_PI/7+ra.y(),ra.z());
		trs.setIdentity();
		trs.setOrigin(rp);
		trs.setRotation(rot);
		psb->transform(trs);
		psb->setTotalMass(0.1);
		psb->addForce(btVector3(0,2,0),0);
		((btSoftRigidDynamicsWorld*) dynamicsWorld)->addSoftBody(psb);
		
	}
	//*/
}


//--------------------------------------------------------------
void testApp::update(){
	float dt = ((ofGetElapsedTimeMillis()-millisForUpdate))* 0.000001f;
//	dt = 0.003407;
	//int t = ofGetElapsedTimeMillis();1
	int maxSimSubSteps = 1;
	int numSimSteps = 0;
	numSimSteps = dynamicsWorld->stepSimulation(dt,maxSimSubSteps);
//	cout<<dt<<endl;
	//dynamicsWorld->stepSimulation(1.0f/60.f,0);

	//cout<<(ofGetElapsedTimeMillis()-millisForUpdate)/2.0<<endl;
//	dynamicsWorld->stepSimulation(1.0/60.0,100);
	dynamicsWorld->stepSimulation((ofGetElapsedTimeMillis()-millisForUpdate)/1000.0f, 20, btScalar(1.)/btScalar(100.));
//	cout << ((ofGetElapsedTimeMillis()-millisForUpdate)/1000.0) << " < " << 20 * (1.0/150.0) << endl;
	millisForUpdate = ofGetElapsedTimeMillis();

	int numLetters = text->getNumberLetters()+text2->getNumberLetters()+text3->getNumberLetters();
	btTransform trans[numLetters];
	btVector3 pos[numLetters];
	btMatrix3x3 basis[numLetters];
	vector<Letter*> l;
	
	for(int i=0; i<numLetters; i++){
		bodies[i]->getMotionState()->getWorldTransform(trans[i]);
		if(i<text->getNumberLetters()){
			l.push_back(text->getLetter(i));
		}
		else if(i<text->getNumberLetters()+text2->getNumberLetters()){
			l.push_back(text2->getLetter(i-text->getNumberLetters()));
		}
		else {
			l.push_back(text3->getLetter(i-text->getNumberLetters()-text2->getNumberLetters()));
		}
		
		pos[i] = trans[i].getOrigin();
		basis[i] = trans[i].getBasis();		
	}
	
	#pragma omp parallel for 
	for(int i=0; i<numLetters; i++){
		l[i]->setPosition(pos[i].getX()*100, pos[i].getY()*100, pos[i].getZ()*100);
		l[i]->matrix[0] = basis[i].getRow(0)[0];
		l[i]->matrix[4] = basis[i].getRow(0)[1];
		l[i]->matrix[8] = basis[i].getRow(0)[2];
		l[i]->matrix[12] = 0;
		l[i]->matrix[1] = basis[i].getRow(1)[0];
		l[i]->matrix[5] = basis[i].getRow(1)[1];
		l[i]->matrix[9] = basis[i].getRow(1)[2];
		l[i]->matrix[13] = 0;
		l[i]->matrix[2] = basis[i].getRow(2)[0];
		l[i]->matrix[6] = basis[i].getRow(2)[1];
		l[i]->matrix[10] = basis[i].getRow(2)[2];
		l[i]->matrix[14] = 0;
		l[i]->matrix[3] = 0;
		l[i]->matrix[7] = 0;
		l[i]->matrix[11] = 0;
		l[i]->matrix[15] = 1;
	}
	
	//Camerastuff
/*	vidGrabber.grabFrame();
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
	//*/
	
	
	//Update shared variables
	for(int i=0;i<sharedVariables.size();i++){
		sharedVariables[i].update(&sender);
	}
	
	//recieve osc messages
	while( receiver.hasWaitingMessages() )
	{	

		ofxOscMessage m;
		
		receiver.getNextMessage( &m );		
	//	cout<<m.getAddress()<<endl;
		for(int i=0;i<sharedVariables.size();i++){
			
			sharedVariables[i].handleOsc(&m);
		}
	}
	
	vidTracker.grabFrame();
	ofPoint leftPoint = ofPoint(-1,-1);
	ofPoint rightPoint = ofPoint(-1,-1);
	ofxPoint2f p;
	if (vidTracker.isFrameNew()){		
		colorImg.setFromPixels(vidTracker.getPixels(), 720,576
		);			
		for (int i = 0; i < camera1.contourFinder.nBlobs; i++){
			for(int u=camera1.contourFinder.blobs[i].pts.size()-1; u>= 0; u--){
				if(camera1.contourFinder.blobs[i].pts[u].x < leftPoint.x || leftPoint.x == -1){
					leftPoint = camera1.contourFinder.blobs[i].pts[u];
				}
				if(camera1.contourFinder.blobs[i].pts[u].x > rightPoint.x || rightPoint.x == -1){
					rightPoint = camera1.contourFinder.blobs[i].pts[u];
				}
			}			
		}
		
		leftPoint.x /= (float)camera1.size.x;
		leftPoint.y /= (float)camera1.size.y;
		 p = camera1.coordwarp.transform(leftPoint.x, leftPoint.y);
		p.x *= ofGetHeight();
		p.y *= ofGetWidth()/3.0;
//**
		rightPoint.x /= (float)camera1.size.x;
		rightPoint.y /= (float)camera1.size.y;
		p = camera1.coordwarp.transform(rightPoint.x, rightPoint.y);
		rightPoint.x = p.x;
		rightPoint.y = p.y;
		rightPoint.x *= ofGetHeight();
		rightPoint.y *= ofGetWidth()/3.0;
//*/
		camera1.update();			
	}
	
	
	//Collider

	if(mouseX > 0 && mouseX < ofGetWidth()){
		point.x += (mouseX*3.0/4.0-point.x);
		point.y += (mouseY*4.0/3.0-point.y );
	} else {//if(rightPoint.x != 0 && rightPoint.y != 0){
		point.x += (p.x-point.x)*0.2;
		point.y += (p.y-point.y )*0.2;
//		cout<<"asd"<<point.x<<endl;
	}

	btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),btVector3(point.x/100.0,point.y/100.0,0)));
	collider->setMotionState(fallMotionState);	
	btTransform col_trans;
	btVector3 col_pos;
	collider->getMotionState()->getWorldTransform(col_trans);
	col_pos = col_trans.getOrigin();
//	btVector3 f = btVector3(point.x/100.0-col_pos.getX(),point.y/100.0-col_pos.getY(),0)*10.;
//	cout<<"Force: "<<f[0]<<" . "<<f[1]<<endl;
//	if((f[0] > 0.1 || f[0] < -0.1) && (f[1] > 0.1 || f[1] < -0.1) ){
//	collider->clearForces();
	//collider->setGravity(btVector3(0.0,0.0,0.0));
//	collider->applyCentralImpulse(f);
//	}
/*	if(cornerWarperIndex != -1){
		ofxPoint2f d[4];
		d[0] = camera1.getDst(0);
		d[1] = camera1.getDst(1);
		d[2] = camera1.getDst(2);
		d[3] = camera1.getDst(3);
		d[cornerWarperIndex].x = mouseX/(float)ofGetWidth();
		d[cornerWarperIndex].y = mouseY/(float)ofGetHeight();
		camera1.updateWarp(d);
	}*/
	ofxPoint2f d[4];
	for(int i=0;i<4;i++){
		d[i].x = (float)cameraCorners[0][i].x;// mouseX/(float)ofGetWidth();
		d[i].y = (float)cameraCorners[0][i].y;//mouseY/(float)ofGetHeight();
		//
	//	cout<<i<<" "<<d[i].x<<" "<<d[i].y<<endl;

	}
	camera1.updateWarp(d);
}

void testApp::draw(){
	//OpenGL stuff
	glEnable(GL_DEPTH_TEST);


	
//Lights
	float L1DirectionX = 0.4;
	float L1DirectionY = -0.4;
	float L1DirectionZ = 1.0;
	
	light1.directionalLight(255, 255, 255, L1DirectionX, L1DirectionY, L1DirectionZ);
	//light1.pointLight(250, 250, 250, ((768*3)/6*1), 1024/2, -200.0);

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
	
	cvdst[0].x = ofGetWidth()/3.0*projectorCorners[0][0].x;
	cvdst[0].y = ofGetHeight()*projectorCorners[0][0].y;
	cvdst[1].x = ofGetWidth()/3.0*projectorCorners[0][1].x;
	cvdst[1].y = ofGetHeight()*projectorCorners[0][1].y;
	cvdst[2].x = ofGetWidth()/3.0*projectorCorners[0][2].x;
	cvdst[2].y = ofGetHeight()*projectorCorners[0][2].y;
	cvdst[3].x = ofGetWidth()/3.0*projectorCorners[0][3].x;
	cvdst[3].y = ofGetHeight()*projectorCorners[0][3].y;		
	
	cvsrc[2].x = 0;
	cvsrc[2].y = 0;
	cvsrc[3].x = ofGetWidth();
	cvsrc[3].y = 0;
	cvsrc[0].x = ofGetWidth();
	cvsrc[0].y = ofGetHeight();
	cvsrc[1].x = 0;
	cvsrc[1].y = ofGetHeight();		

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
	glOrtho(-1.0, 1.0, -4.0/3.0, 4.0/3.0, 50, 100);	
	glScaled(4.0, 1.0, 1.0); 
	drawViewport();
	glPopMatrix();	
//Screen 2
	glPushMatrix();
	glTranslatef( ofGetWidth()/3.0, 0, 0);
	
	cvdst[0].x = ofGetWidth()/3.0*projectorCorners[1][0].x;
	cvdst[0].y = ofGetHeight()*projectorCorners[1][0].y;
	cvdst[1].x = ofGetWidth()/3.0*projectorCorners[1][1].x;
	cvdst[1].y = ofGetHeight()*projectorCorners[1][1].y;
	cvdst[2].x = ofGetWidth()/3.0*projectorCorners[1][2].x;
	cvdst[2].y = ofGetHeight()*projectorCorners[1][2].y;
	cvdst[3].x = ofGetWidth()/3.0*projectorCorners[1][3].x;
	cvdst[3].y = ofGetHeight()*projectorCorners[1][3].y;		
	
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
	glOrtho(-1.0, 1.0, -4.0/3.0, 4.0/3.0, 50, 100);

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
	cvdst[0].x = ofGetWidth()/3.0*projectorCorners[2][0].x;
	cvdst[0].y = ofGetHeight()*projectorCorners[2][0].y;
	cvdst[1].x = ofGetWidth()/3.0*projectorCorners[2][1].x;
	cvdst[1].y = ofGetHeight()*projectorCorners[2][1].y;
	cvdst[2].x = ofGetWidth()/3.0*projectorCorners[2][2].x;
	cvdst[2].y = ofGetHeight()*projectorCorners[2][2].y;
	cvdst[3].x = ofGetWidth()/3.0*projectorCorners[2][3].x;
	cvdst[3].y = ofGetHeight()*projectorCorners[2][3].y;		
	
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
	glOrtho(-1.0, 1.0, -4.0/3.0, 4.0/3.0, 50, 100);

	glScaled(4.0, 1.0, 1.0); 
	glTranslatef(-ofGetHeight()*2, 0, 0);
	drawViewport();
	glPopMatrix();
#else
	gluOrtho2D(1., 0., 4.0/3.0, 0.);
	drawViewport();
	
#endif


}

//--------------------------------------------------------------
void testApp::drawViewport(){
	ofSetColor(255, 255, 255);

	glEnable(GL_DEPTH_TEST);
	
	//Draw Letters	
	text->drawText();
	text2->drawText();
	text3->drawText();	
	
	text->drawBricks();
	text2->drawBricks();
	text3->drawBricks();

//Collider
	btTransform trans;
	btVector3 pos;
	btMatrix3x3 basis;		
	collider->getMotionState()->getWorldTransform(trans);
	pos = trans.getOrigin();
	//basis[i] = trans[i].getBasis();
	glPushMatrix();
		glTranslatef((float)pos.getX()*100-10,pos.getY()*100-10, 0);
		ofFill();
		ofSetColor(255, 0, 0);
		ofRect(0,0,20,20);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(mouseX*3.0/4.0-10,mouseY*4.0/3.0-10, 0);
	ofFill();
	ofSetColor(0, 255, 0);
	ofRect(0,0,20,20);
	glPopMatrix();
	
	
	if(debug)
		camera1.draw(ofGetHeight(), ofGetWidth()/3.0);
	
	

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
	
	switch (key){
		case 'a':
			camera1.simplify->damp = 0;
			break;
		case 'z':
			camera1.simplify->damp = 0.09;
			break;
		case '1':
			cornerWarperIndex = 0;
			break;
		case '2':
			cornerWarperIndex = 1;
			break;
		case '3':
			cornerWarperIndex = 2;
			break;
		case '4':
			cornerWarperIndex = 3;
			break;
		case 'b':
			camera1.bLearnBakground = true;
			break;
		case '+':
			camera1.threshold ++;
			if (camera1.threshold > 255) camera1.threshold = 255;
			break;
		case '-':
			camera1.threshold --;
			if (camera1.threshold < 0) camera1.threshold = 0;
			break;
		case 'v':
			vidTracker.videoSettings();
			break;
		case 'd':
			debug = !debug;
			break;
		case 's':
				dynamicsWorld->stepSimulation(1.0f/60.f,0);
			break;
		case 'r':
//			text->clear();

			
		case 'p':
			int numLetters = text->getNumberLetters();
			for(int i=0; i<numLetters; i++){
			//	dynamicsWorld->addRigidBody(bodies[i]);
			}
			numLetters = text2->getNumberLetters();
			for(int i=0; i<numLetters; i++){
			//	dynamicsWorld->addRigidBody(bodies2[i]);
			}
			numLetters = text3->getNumberLetters();
			for(int i=0; i<numLetters; i++){
			//	dynamicsWorld->addRigidBody(bodies3[i]);
			}
			break;
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
	if(cornerWarperIndex != -1)
		cornerWarperIndex = -1;
}

//--------------------------------------------------------------
void testApp::mouseReleased(){
}

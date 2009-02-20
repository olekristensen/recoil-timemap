#define USE_PARALLEL_DISPATCHER 1

#include "testApp.h"
#include <FTGL/ftgl.h>

const int maxProxies = 32766;
const int maxOverlap = 65535;

static inline btScalar	UnitRand(){
	return(rand()/(btScalar)RAND_MAX);
}

static inline btScalar	SignedUnitRand(){
	return(UnitRand()*2-1);
}

static inline btVector3	Vector3Rand(){
	const btVector3	p=btVector3(SignedUnitRand(),SignedUnitRand(),SignedUnitRand());
	return(p.normalized());
}

//--------------------------------------------------------------
void testApp::setup(){	 
	
	// General setup
	
	ofHideCursor();
	ofBackground(0, 0, 0);
	ofSetFrameRate(120);
	
	// OSC Setup for sharedVariables
	
	receiver.setup(8001);
	sender.setup("localhost", 8000);
	
	ofxOscMessage m;
	m.setAddress( ("/sendVariables/please"));
	m.addIntArg(1);
	sender.sendMessage( m );

	// Add sharedvariables

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
	
	moveCameraCorners = false;
	moveProjectorCorners = false;
	moveCameraCornersIndex = -1;
	moveProjectorCornersIndex = -1;
	sharedVariables.push_back(SharedVariable(&moveCameraCorners, "moveCameraCorners"));
	sharedVariables.push_back(SharedVariable(&moveProjectorCorners, "moveProjectorCorners"));
	sharedVariables.push_back(SharedVariable(&moveCameraCornersIndex, "moveCameraCornersIndex"));
	sharedVariables.push_back(SharedVariable(&moveProjectorCornersIndex, "moveProjectorCornersIndex"));
	
	// Bullet
	
	btGravity = new btVector3(0,10,0);

	sharedVariables.push_back(SharedVariable(&btGravity->m_floats[0],	"gravityX"));
	sharedVariables.push_back(SharedVariable(&btGravity->m_floats[1],	"gravityY"));
	sharedVariables.push_back(SharedVariable(&btGravity->m_floats[2],	"gravityZ"));
	
	bulletSetup();

	// Fonts
 	
	font1 = new TextFontHolder("ApexSerif-Book.otf", 20);

	// Texts

	for(int i=0;i<3;i++){
		sharedVariables.push_back(SharedVariable(&textStrings[i],	"textStrings"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textState[i],		"textState"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textPosition[i].x,"textPositionX"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textPosition[i].y,"textPositiony"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textWidth[i],		"textWidth"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textDepth[i],		"textDepth"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorR[i],	"textColorR"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorG[i],	"textColorG"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorB[i],	"textColorB"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorA[i],	"textColorA"	+ofToString(i, 0)));
	}
	
	textSetup();

	//---
	//Light
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
	camera2.setup(&vidTracker, &trackerTexture, &colorImg,  vidTracker.width/2,0,vidTracker.width/2, vidTracker.height/2);
	camera3.setup(&vidTracker, &trackerTexture, &colorImg,  0,vidTracker.height/2,vidTracker.width/2, vidTracker.height/2);
	
	makeSnaps = false;
	memset(snapString, 0, 255);		// clear the string by setting all chars to 0
	snapCounter = 0;

	debug = false;
}

//--------------------------------------------------------------
void testApp::textSetup(){
	text1 = new Text();
	text1->setText("Når andre kan læse ens tanker, hvordan undgår man så at vise sine tanker - man kan ikke helt lade være at tænke - men man kan tænke på noget andet, dække sine egne tanker gennem bevidst og konsekvent at referere andres mulige tanker? Eller? Hvordan forløber et forhør under de omstændigheder? Intet behøver at blive sagt.");
	text1->setFont(font1);
	text1->setWordBlocks(true);
	text1->setDepth(100);
	text1->setWidth(500);
	text1->setLineHeight(20*1.3);
	text1->constructText();
	text1->translate(50,ofGetWidth()/3.0-text1->getHeight(),0);
	text1->setupBullet(dynamicsWorld, &bodies);
	
	text2 = new Text();
	text2->setText("Når andre kan\n\n\n\n læse ens tanker, hvordan undgår man så at vise sine tanker?\n - man kan ikke helt lade være at tænke...");
	text2->setWordBlocks(true);
	text2->setFont(font1);
	text2->setDepth(100);
	text2->setWidth(500);
	text2->setLineHeight(20*1.3);
	text2->constructText();
	text2->translate(50+ofGetHeight(),ofGetWidth()/3.0-text2->getHeight(),0);
	text2->setupBullet(dynamicsWorld, &bodies);
	
	text3 = new Text();
	text3->setText("Has cu etiam legere iuvaret, pri malorum fuisset tibiqu\n\n\n\n\n\n");
	text3->setWordBlocks(true);
	text3->setFont(font1);
	text3->setDepth(100);
	text3->setWidth(500);
	text3->setLineHeight(20*1.3);
	text3->constructText();
	text3->translate(50+ofGetHeight()*2,ofGetWidth()/3.0-text3->getHeight(),0);
	text3->setupBullet(dynamicsWorld, &bodies);
	
}

//--------------------------------------------------------------
void testApp::bulletSetup(){

#ifdef USE_PARALLEL_DISPATCHER

		threadSupportSolver = 0;
		threadSupportCollision = 0;
#endif
		
		dispatcher=0;
		collisionConfiguration = new btDefaultCollisionConfiguration();
		
#ifdef USE_PARALLEL_DISPATCHER
		int maxNumOutstandingTasks = 4;
		
#ifdef USE_WIN32_THREADING
		
		threadSupportCollision = new Win32ThreadSupport(Win32ThreadSupport::Win32ThreadConstructionInfo(
																										  "collision",
																										  processCollisionTask,
																										  createCollisionLocalStoreMemory,
																										  maxNumOutstandingTasks));
#else
		
#ifdef USE_LIBSPE2
		
		spe_program_handle_t * program_handle;
#ifndef USE_CESOF
		program_handle = spe_image_open ("./spuCollision.elf");
		if (program_handle == NULL)
		{
			perror( "SPU OPEN IMAGE ERROR\n");
		}
		else
		{
			printf( "IMAGE OPENED\n");
		}
#else
		extern spe_program_handle_t spu_program;
		program_handle = &spu_program;
#endif
        SpuLibspe2Support* threadSupportCollision  = new SpuLibspe2Support( program_handle, maxNumOutstandingTasks);
#elif defined (USE_PTHREADS)
		PosixThreadSupport::ThreadConstructionInfo constructionInfo("collision",
																	processCollisionTask,
																	createCollisionLocalStoreMemory,
																	maxNumOutstandingTasks);
		threadSupportCollision = new PosixThreadSupport(constructionInfo);
	cout<<"paralllel"<<endl;

#else
		
		SequentialThreadSupport::SequentialThreadConstructionInfo colCI("collision",processCollisionTask,createCollisionLocalStoreMemory);
		SequentialThreadSupport* threadSupportCollision = new SequentialThreadSupport(colCI);
		
#endif //USE_LIBSPE2
		
		///Playstation 3 SPU (SPURS)  version is available through PS3 Devnet
		/// For Unix/Mac someone could implement a pthreads version of btThreadSupportInterface?
		///you can hook it up to your custom task scheduler by deriving from btThreadSupportInterface
#endif
		
		
		dispatcher = new	SpuGatheringCollisionDispatcher(threadSupportCollision,maxNumOutstandingTasks,collisionConfiguration);
#else
		
		dispatcher = new	btCollisionDispatcher(collisionConfiguration);
#endif //USE_PARALLEL_DISPATCHER
		
	btVector3 worldAabbMin(0,0,0);
	btVector3 worldAabbMax(ofGetHeight()*3.0, ofGetWidth()/3.0, 1000);
	
	broadphase = new btDbvtBroadphase();

		
		
#ifdef USE_PARALLEL_SOLVER

#ifdef USE_WIN32_THREADING
		threadSupportSolver = new Win32ThreadSupport(Win32ThreadSupport::Win32ThreadConstructionInfo(
																									   "solver",
																									   processSolverTask,
																									   createSolverLocalStoreMemory,
																									   maxNumOutstandingTasks));
#elif defined (USE_PTHREADS)
		PosixThreadSupport::ThreadConstructionInfo solverConstructionInfo("solver", processSolverTask,
																		  createSolverLocalStoreMemory, maxNumOutstandingTasks);
		
    	threadSupportSolver = new PosixThreadSupport(solverConstructionInfo);
#else
		//for now use sequential version	
		SequentialThreadSupport::SequentialThreadConstructionInfo solverCI("solver",processSolverTask,createSolverLocalStoreMemory);
		threadSupportSolver = new SequentialThreadSupport(solverCI);
		
#endif //USE_WIN32_THREADING
		solver = new btParallelSequentialImpulseSolver(threadSupportSolver,maxNumOutstandingTasks);
		
#else
		
		btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
		
		solver = solver;
		//default solverMode is SOLVER_RANDMIZE_ORDER. Warmstarting seems not to improve convergence, see 
		//solver->setSolverMode(0);//btSequentialImpulseConstraintSolver::SOLVER_USE_WARMSTARTING | btSequentialImpulseConstraintSolver::SOLVER_RANDMIZE_ORDER);
		
#endif //USE_PARALLEL_SOLVER
		
		
		
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfiguration);

	dynamicsWorld->getSolverInfo().m_numIterations = 4;
	dynamicsWorld->getSolverInfo().m_solverMode = SOLVER_SIMD+SOLVER_USE_WARMSTARTING;
	
	dynamicsWorld->getDispatchInfo().m_enableSPU = true;
	//dynamicsWorld->setGravity(btVector3(0,10,0));
	dynamicsWorld->setGravity(*btGravity);
	
	//Ground 	
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,-1.0,0),1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0.0,((ofGetWidth()/3.0)+100)/100.0,0)));
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
}


//--------------------------------------------------------------
void testApp::update(){
	
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
	
	// Videograbber
	
	vidTracker.grabFrame();
	ofPoint leftPoint = ofPoint(-1,-1);
	ofPoint rightPoint = ofPoint(-1,-1);
	ofxPoint2f p;
	if (vidTracker.isFrameNew()){		
		colorImg.setFromPixels(vidTracker.getPixels(), 720,576);
		camera1.update();			
		camera2.update();			
		camera3.update();			
	}
	
	//Gravity
	
	dynamicsWorld->setGravity(*btGravity);
	
	// Silhouette 1
	
	silhouette1Shape = new btConvexHullShape();
	if(camera1.contourFinder.nBlobs > 0){
		for(int i=0;i<camera1.simplify->numPoints;i++){
			silhouette1Shape->addPoint(btVector3(camera1.simplify->points[i].x*ofGetHeight()/100.0, camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,+20000));
			silhouette1Shape->addPoint(btVector3(camera1.simplify->points[i].x*ofGetHeight()/100.0, camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,-20000));
			//cout<<camera1.simplify->points[i].x*ofGetHeight()/100.0<< "  ,  "<<camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0<<endl;
		}
	}
	btVector3 silhouette1Pos = btVector3(0,0,0);
	silhouette1MotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette1Pos));
	btScalar silhouette1Mass = 0;
	btVector3 silhouette1FallInertia(0,0,0);
	silhouette1Shape->calculateLocalInertia(silhouette1Mass,silhouette1FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette1RigidBodyCI(silhouette1Mass,silhouette1MotionState,silhouette1Shape,silhouette1FallInertia);
	silhouette1 = new btRigidBody(silhouette1RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette1->setCollisionFlags( silhouette1->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette1->setActivationState(DISABLE_DEACTIVATION);
	
	dynamicsWorld->addRigidBody(silhouette1);
	
	// Silhouette 2
	
	silhouette2Shape = new btConvexHullShape();
	if(camera2.contourFinder.nBlobs > 0){
		for(int i=0;i<camera1.simplify->numPoints;i++){
			silhouette2Shape->addPoint(btVector3(camera2.simplify->points[i].x*ofGetHeight()/100.0, camera2.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,+20000));
			silhouette2Shape->addPoint(btVector3(camera2.simplify->points[i].x*ofGetHeight()/100.0, camera2.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,-20000));
			//cout<<camera1.simplify->points[i].x*ofGetHeight()/100.0<< "  ,  "<<camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0<<endl;
		}
	}
	btVector3 silhouette2Pos = btVector3(0,0,0);
	silhouette2MotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette2Pos));
	btScalar silhouette2Mass = 0;
	btVector3 silhouette2FallInertia(0,0,0);
	silhouette2Shape->calculateLocalInertia(silhouette2Mass,silhouette2FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette2RigidBodyCI(silhouette2Mass,silhouette2MotionState,silhouette2Shape,silhouette2FallInertia);
	silhouette2 = new btRigidBody(silhouette2RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette2->setCollisionFlags( silhouette2->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette2->setActivationState(DISABLE_DEACTIVATION);
	
	dynamicsWorld->addRigidBody(silhouette2);
	
	
	// Silhouette 3
	
	silhouette3Shape = new btConvexHullShape();
	if(camera3.contourFinder.nBlobs > 0){
		for(int i=0;i<camera1.simplify->numPoints;i++){
			silhouette3Shape->addPoint(btVector3(camera3.simplify->points[i].x*ofGetHeight()/100.0, camera3.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,+20000));
			silhouette3Shape->addPoint(btVector3(camera3.simplify->points[i].x*ofGetHeight()/100.0, camera3.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,-20000));
			//cout<<camera1.simplify->points[i].x*ofGetHeight()/100.0<< "  ,  "<<camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0<<endl;
		}
	}
	btVector3 silhouette3Pos = btVector3(0,0,0);
	silhouette3MotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette3Pos));
	btScalar silhouette3Mass = 0;
	btVector3 silhouette3FallInertia(0,0,0);
	silhouette3Shape->calculateLocalInertia(silhouette3Mass,silhouette3FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette3RigidBodyCI(silhouette3Mass,silhouette3MotionState,silhouette3Shape,silhouette3FallInertia);
	silhouette3 = new btRigidBody(silhouette3RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette3->setCollisionFlags( silhouette3->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette3->setActivationState(DISABLE_DEACTIVATION);
	
	dynamicsWorld->addRigidBody(silhouette3);
	
	//Collider

	if(mouseX > 0 && mouseX < ofGetWidth()){
		point.x += (mouseX*3.0/4.0-point.x);
		point.y += (mouseY*4.0/3.0-point.y );
	} else {
		point.x += (p.x-point.x)*0.2;
		point.y += (p.y-point.y )*0.2;
	}

	fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),btVector3(point.x/100.0,point.y/100.0,0)));
	collider->setMotionState(fallMotionState);	
	btTransform col_trans;
	btVector3 col_pos;
	collider->getMotionState()->getWorldTransform(col_trans);
	col_pos = col_trans.getOrigin();

	ofxPoint2f d[4];
	for(int i=0;i<4;i++){
		d[i].x = (float)cameraCorners[0][i].x;
		d[i].y = (float)cameraCorners[0][i].y;
	}
	camera1.updateWarp(d);
	
	for(int i=0;i<4;i++){
		d[i].x = (float)cameraCorners[1][i].x;
		d[i].y = (float)cameraCorners[1][i].y;
	}
	camera2.updateWarp(d);

	for(int i=0;i<4;i++){
		d[i].x = (float)cameraCorners[2][i].x;
		d[i].y = (float)cameraCorners[2][i].y;
	}
	camera3.updateWarp(d);
	
	 
#ifdef FIXED_STEP
	dynamicsWorld->stepSimulation(1.0f/60.f,0);
	
#else
	
	float dt = getDeltaTimeMicroseconds() * 0.000001f;
	
	int maxSimSubSteps = 20;
	int numSimSteps = 0;
	numSimSteps = dynamicsWorld->stepSimulation(dt,maxSimSubSteps, btScalar(1.)/btScalar(60.));
	/*
	 if (!numSimSteps)
	 printf("Interpolated transforms\n");
	 else
	 {
	 if (numSimSteps > maxSimSubSteps)
	 {
	 //detect dropping frames
	 printf("Dropped (%i) simulation steps out of %i\n",numSimSteps - maxSimSubSteps,numSimSteps);
	 } else
	 {
	 printf("Simulated (%i) steps\n",numSimSteps);
	 }
	 }
	 
	 //*/
#endif
	
	int numLetters = text1->getNumberLetters()+text2->getNumberLetters()+text3->getNumberLetters();
	btTransform trans[numLetters];
	btVector3 pos[numLetters];
	btMatrix3x3 basis[numLetters];
	vector<Letter*> l;
	
	for(int i=0; i<numLetters; i++){
		bodies[i]->getMotionState()->getWorldTransform(trans[i]);
		if(i<text1->getNumberLetters()){
			l.push_back(text1->getLetter(i));
		}
		else if(i<text1->getNumberLetters()+text2->getNumberLetters()){
			l.push_back(text2->getLetter(i-text1->getNumberLetters()));
		}
		else {
			l.push_back(text3->getLetter(i-text1->getNumberLetters()-text2->getNumberLetters()));
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
	if(camera1.contourFinder.nBlobs > 0){
		dynamicsWorld->removeRigidBody(silhouette1);
	}
	if(camera2.contourFinder.nBlobs > 0){
		dynamicsWorld->removeRigidBody(silhouette2);
	}
	if(camera3.contourFinder.nBlobs > 0){
		dynamicsWorld->removeRigidBody(silhouette3);
	}
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
	GLfloat myMatrix[16];
	for(int i = 0; i < 16; i++){
		if(i % 5 != 0) myMatrix[i] = 0.0;
		else myMatrix[i] = 1.0;
	}
	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];	
	double eqnLtX[] =
	{ 1.0, 0.0, 0.0, 0.0 }; // klipper x < 0
	double eqnGtX[] =
	{ -1.0, 0.0, 0.0, 0.0 }; //klipper x > 0
	
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0.0, (float)ofGetHeight(),
			0.0, (float)ofGetWidth()/3.0, 10.0, 2500);
	
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1000,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)ofGetWidth()/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport(0, 0, ofGetWidth()/3, ofGetHeight());
	
	cvsrc[0].x = 0;
	cvsrc[0].y = 4.0*ofGetHeight()/3.0;		
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = ofGetWidth()/4.0;
	cvsrc[2].y = 0;
	cvsrc[3].x = ofGetWidth()/4.0;
	cvsrc[3].y = 4.0*ofGetHeight()/3.0;	
	
	cvdst[0].y = ofGetWidth()/3.0*(1.0-projectorCorners[0][3].x);
	cvdst[0].x = ofGetHeight()*projectorCorners[0][3].y;
	cvdst[1].y = ofGetWidth()/3.0*(1.0-projectorCorners[0][0].x);
	cvdst[1].x = ofGetHeight()*projectorCorners[0][0].y;
	cvdst[2].y = ofGetWidth()/3.0*(1.0-projectorCorners[0][1].x);
	cvdst[2].x = ofGetHeight()*projectorCorners[0][1].y;
	cvdst[3].y = ofGetWidth()/3.0*(1.0-projectorCorners[0][2].x);
	cvdst[3].x = ofGetHeight()*projectorCorners[0][2].y;	
	
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

	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);
	
	glPushMatrix();
	glTranslatef(ofGetHeight(), 0, 0);
	glClipPlane(GL_CLIP_PLANE4, eqnGtX);
	glEnable(GL_CLIP_PLANE4);
	glPopMatrix();
		
	drawViewport();
	
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	
	//Screen 2
	
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0.0, (float)ofGetHeight(),
			0.0, (float)ofGetWidth()/3.0, 10.0, 2500);
	
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1000,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)ofGetWidth()/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport(ofGetWidth()/3.0, 0, ofGetWidth()/3.0, ofGetHeight());
	
	cvsrc[0].x = 0;
	cvsrc[0].y = 4.0*ofGetHeight()/3.0;		
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = ofGetWidth()/4.0;
	cvsrc[2].y = 0;
	cvsrc[3].x = ofGetWidth()/4.0;
	cvsrc[3].y = 4.0*ofGetHeight()/3.0;	
	
	cvdst[0].y = ofGetWidth()/3.0*(1.0-projectorCorners[1][3].x);
	cvdst[0].x = ofGetHeight()*projectorCorners[1][3].y;
	cvdst[1].y = ofGetWidth()/3.0*(1.0-projectorCorners[1][0].x);
	cvdst[1].x = ofGetHeight()*projectorCorners[1][0].y;
	cvdst[2].y = ofGetWidth()/3.0*(1.0-projectorCorners[1][1].x);
	cvdst[2].x = ofGetHeight()*projectorCorners[1][1].y;
	cvdst[3].y = ofGetWidth()/3.0*(1.0-projectorCorners[1][2].x);
	cvdst[3].x = ofGetHeight()*projectorCorners[1][2].y;	
	
	translate = cvCreateMat(3,3,CV_32FC1);
	src_mat = cvCreateMat( 4, 2, CV_32FC1 );
	dst_mat = cvCreateMat( 4, 2, CV_32FC1 );
	cvSetData( src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvSetData( dst_mat, cvdst, sizeof(CvPoint2D32f));
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
	
	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);
	
	glPushMatrix();
	glTranslatef(ofGetHeight(), 0, 0);
	glClipPlane(GL_CLIP_PLANE4, eqnGtX);
	glEnable(GL_CLIP_PLANE4);
	glPopMatrix();
	
	glTranslatef(-ofGetHeight(), 0, 0);
	
	drawViewport();
	
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	
	//Screen 3	
	
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0.0, (float)ofGetHeight(),
			0.0, (float)ofGetWidth()/3.0, 10.0, 2500);
	
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1000,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);

	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)ofGetWidth()/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport(2.0*ofGetWidth()/3.0, 0, ofGetWidth()/3.0, ofGetHeight());
	
	cvsrc[0].x = 0;
	cvsrc[0].y = 4.0*ofGetHeight()/3.0;		
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = ofGetWidth()/4.0;
	cvsrc[2].y = 0;
	cvsrc[3].x = ofGetWidth()/4.0;
	cvsrc[3].y = 4.0*ofGetHeight()/3.0;	
	
	cvdst[0].y = ofGetWidth()/3.0*(1.0-projectorCorners[2][3].x);
	cvdst[0].x = ofGetHeight()*projectorCorners[2][3].y;
	cvdst[1].y = ofGetWidth()/3.0*(1.0-projectorCorners[2][0].x);
	cvdst[1].x = ofGetHeight()*projectorCorners[2][0].y;
	cvdst[2].y = ofGetWidth()/3.0*(1.0-projectorCorners[2][1].x);
	cvdst[2].x = ofGetHeight()*projectorCorners[2][1].y;
	cvdst[3].y = ofGetWidth()/3.0*(1.0-projectorCorners[2][2].x);
	cvdst[3].x = ofGetHeight()*projectorCorners[2][2].y;	
	
	translate = cvCreateMat(3,3,CV_32FC1);
	src_mat = cvCreateMat( 4, 2, CV_32FC1 );
	dst_mat = cvCreateMat( 4, 2, CV_32FC1 );
	cvSetData( src_mat, cvsrc, sizeof(CvPoint2D32f));
	cvSetData( dst_mat, cvdst, sizeof(CvPoint2D32f));
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
	
	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);

	glPushMatrix();
		glTranslatef(ofGetHeight(), 0, 0);
		glClipPlane(GL_CLIP_PLANE4, eqnGtX);
		glEnable(GL_CLIP_PLANE4);
	glPopMatrix();
	
	glTranslatef(-ofGetHeight()*2.0, 0, 0);

	drawViewport();

	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	
	//Scene overview	
	
	
	int w, h;
	
	w = ofGetWidth()/6.0;
	h = ofGetHeight();
	
	float halfFov, theTan, screenFov, aspect;
	screenFov 		= 60.0f;
	
	float eyeX 		= (float)w / 2.0;
	float eyeY 		= (float)h / 2.0;
	halfFov 		= PI * screenFov / 360.0;
	theTan 			= tanf(halfFov);
	float dist 		= eyeY / theTan;
	float nearDist 	= dist / 10.0;	// near / far clip plane
	float farDist 	= dist * 10.0;
	aspect 			= (float)w/(float)h;
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(screenFov, aspect, nearDist, farDist);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eyeX*1.5, eyeY*3.0, dist*4.0, eyeX*1.5, eyeY, 0.0, 1.0, 0.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)ofGetWidth()/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport((ofGetWidth()/3.0), 0, ofGetWidth()/6.0, ofGetHeight());

	glTranslatef(-ofGetHeight(), 0, 0);

	ofEnableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	ofSetColor(0, 64, 255,128);
	ofFill();
	ofRect(0,0,ofGetHeight()*3.0,ofGetWidth()/3.0);
	glEnable(GL_DEPTH_TEST);
	
	glPushMatrix();
	glTranslatef(-ofGetWidth()/3.0, 0, 0);
	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(ofGetWidth(), 0, 0);
	glClipPlane(GL_CLIP_PLANE4, eqnGtX);
	glEnable(GL_CLIP_PLANE4);
	glPopMatrix();
	
	drawViewport();
	
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	
#else
	gluOrtho2D(1., 0., 4.0/3.0, 0.);
	drawViewport();
	
#endif


}

//--------------------------------------------------------------
void testApp::drawViewport(){

	//** grids etc. for projection calibration

	if(moveProjectorCorners){
	glTranslatef(0.0, 0.0, -300.0);
	ofSetColor(10, 10, 20);
	ofRect(0, 0, ofGetHeight(), ofGetWidth()/3.0);
	ofRect(ofGetHeight(), 0, ofGetHeight(), ofGetWidth()/3.0);
	ofRect(ofGetHeight()*2.0, 0, ofGetHeight(), ofGetWidth()/3.0);
	ofEnableAlphaBlending();
	ofDisableSmoothing();
	ofSetColor(255, 255, 255, 20);
	for(int v=0;v<ofGetHeight()*3.0;v+=10){
		ofLine(v, 0, v, ofGetWidth()/3.0);
	}
	for(int h=0;h<ofGetWidth()/3.0;h+=10){
		ofLine(0, h, ofGetHeight()*3.0, h);
	}
	glTranslatef(0.0, 0.0, 300.0);
	}
	
	//**/

	glEnable(GL_DEPTH_TEST);
	ofSetColor(255, 255, 255);
	
	//Draw Letters	
	text1->drawText();
	text2->drawText();
	text3->drawText();	
	
	/**
	text1->drawBricks();
	text2->drawBricks();
	text3->drawBricks();
	 //**/
	 
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
	
	
	if(debug){
		camera1.draw(ofGetHeight(), ofGetWidth()/3.0);
		//camera2.draw(ofGetHeight(), ofGetWidth()/3.0);
		//camera3.draw(ofGetHeight(), ofGetWidth()/3.0);
		dynamicsWorld->debugDrawWorld();
	}
	
	//trackerTexture.draw(0,0);	
	
	if(makeSnaps){
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
		case 'b':
			camera1.bLearnBakground = true;
			break;
		case '+':
			camera1.threshold ++;
			if (camera1.threshold > 255) camera1.threshold = 255;
			cout<<camera1.threshold<<endl;
			break;
		case '-':
			camera1.threshold --;
			if (camera1.threshold < 0) camera1.threshold = 0;
			cout<<camera1.threshold<<endl;
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
			break;
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

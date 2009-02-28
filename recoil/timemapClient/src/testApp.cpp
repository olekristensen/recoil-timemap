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

	// Texts

	textSetup();

	textUpdate();

	for(int i=0;i<3;i++){
		sharedVariables.push_back(SharedVariable(&textStrings[i],		"textStrings"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textFontSize[i],		"textFontSize"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textState[i],			"textState"			+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textScaffolding[i],	"textScaffolding"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textRefresh[i],		"textRefresh"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textAnimate[i],		"textAnimate"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textPosition[i].x,	"textPositionX"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textPosition[i].y,	"textPositionY"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textWidth[i],			"textWidth"			+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textDepth[i],			"textDepth"			+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textFriction[i],		"textFriction"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textDamping[i],		"textDamping"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textRestitution[i],	"textRestitution"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorA[i],		"textColorA"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorR[i],		"textColorR"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorG[i],		"textColorG"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorB[i],		"textColorB"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textColorA[i],		"textColorA"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&backgroundColorR[i],	"backgroundColorR"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&backgroundColorG[i],	"backgroundColorG"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&backgroundColorB[i],	"backgroundColorB"	+ofToString(i, 0)));
	}
	
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
	status = false;
	statusFont = TextFontHolder("ApexSerif-Book.otf",10);
	
	testCard1.loadImage("768x1024.001.png");
	testCard2.loadImage("768x1024.002.png");
	testCard3.loadImage("768x1024.003.png");
	
}

//--------------------------------------------------------------
void testApp::textSetup(){
	
	for(int i=0;i<3;i++){
	
		textStrings[i] = "";
		textFontSize[i] = 20.0;
		textState[i] = TEXT_STATE_RESET;
		textPosition[i].x = 0.0;
		textPosition[i].y = 0.0;
		textWidth[i] = 500.0;
		textDepth[i] = 200.0;

		textFriction[i] = 0.25;
		textDamping[i] = 0.0;
		textMass[i] = 1.0;
		textRestitution[i] = 0.4;
		
		textColorR[i] = 1.0;
		textColorG[i] = 1.0;
		textColorB[i] = 1.0;
		textColorA[i] = 1.0;
		
		textRefresh[i] = false;
		textAnimate[i] = false;
		textScaffolding[i] = true;
	}
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
	btScalar mass = 0.;
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
		for(int i=0;i<sharedVariables.size();i++){
			//cout<<m.getAddress()<<endl;
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
	
	silhouette1Shape = btConvexHullShape();
	if(camera1.contourFinder.nBlobs > 0){
		for(int i=0;i<camera1.simplify->numPoints;i++){
			silhouette1Shape.addPoint(btVector3(camera1.simplify->points[i].x*ofGetHeight()/100.0, camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,+20000));
			silhouette1Shape.addPoint(btVector3(camera1.simplify->points[i].x*ofGetHeight()/100.0, camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,-20000));
			//cout<<camera1.simplify->points[i].x*ofGetHeight()/100.0<< "  ,  "<<camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0<<endl;
		}
	}
	btVector3 silhouette1Pos = btVector3(0,0,0);
	silhouette1MotionState = btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette1Pos));
	btScalar silhouette1Mass = 0;
	btVector3 silhouette1FallInertia(0,0,0);
	silhouette1Shape.calculateLocalInertia(silhouette1Mass,silhouette1FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette1RigidBodyCI(silhouette1Mass,&silhouette1MotionState,&silhouette1Shape,silhouette1FallInertia);
	btRigidBody* silhouette1 = new btRigidBody(silhouette1RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette1->setCollisionFlags( silhouette1->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette1->setActivationState(DISABLE_DEACTIVATION);
	
	dynamicsWorld->addRigidBody(silhouette1);
	
	// Silhouette 2
	
	silhouette2Shape = btConvexHullShape();
	if(camera2.contourFinder.nBlobs > 0){
		for(int i=0;i<camera1.simplify->numPoints;i++){
			silhouette2Shape.addPoint(btVector3(camera2.simplify->points[i].x*ofGetHeight()/100.0, camera2.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,+20000));
			silhouette2Shape.addPoint(btVector3(camera2.simplify->points[i].x*ofGetHeight()/100.0, camera2.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,-20000));
			//cout<<camera1.simplify->points[i].x*ofGetHeight()/100.0<< "  ,  "<<camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0<<endl;
		}
	}
	btVector3 silhouette2Pos = btVector3(0,0,0);
	silhouette2MotionState = btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette2Pos));
	btScalar silhouette2Mass = 0;
	btVector3 silhouette2FallInertia(0,0,0);
	silhouette2Shape.calculateLocalInertia(silhouette2Mass,silhouette2FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette2RigidBodyCI(silhouette2Mass,&silhouette2MotionState,&silhouette2Shape,silhouette2FallInertia);
	btRigidBody* silhouette2 = new btRigidBody(silhouette2RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette2->setCollisionFlags( silhouette2->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette2->setActivationState(DISABLE_DEACTIVATION);
	
	dynamicsWorld->addRigidBody(silhouette2);
	
	
	// Silhouette 3
	
	silhouette3Shape = btConvexHullShape();
	if(camera3.contourFinder.nBlobs > 0){
		for(int i=0;i<camera1.simplify->numPoints;i++){
			silhouette3Shape.addPoint(btVector3(camera3.simplify->points[i].x*ofGetHeight()/100.0, camera3.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,+20000));
			silhouette3Shape.addPoint(btVector3(camera3.simplify->points[i].x*ofGetHeight()/100.0, camera3.simplify->points[i].y*(ofGetWidth()/3.0)/100.0,-20000));
			//cout<<camera1.simplify->points[i].x*ofGetHeight()/100.0<< "  ,  "<<camera1.simplify->points[i].y*(ofGetWidth()/3.0)/100.0<<endl;
		}
	}
	btVector3 silhouette3Pos = btVector3(0,0,0);
	silhouette3MotionState = btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette3Pos));
	btScalar silhouette3Mass = 0;
	btVector3 silhouette3FallInertia(0,0,0);
	silhouette3Shape.calculateLocalInertia(silhouette3Mass,silhouette3FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette3RigidBodyCI(silhouette3Mass,&silhouette3MotionState,&silhouette3Shape,silhouette3FallInertia);
	btRigidBody* silhouette3 = new btRigidBody(silhouette3RigidBodyCI);
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

	fallMotionState = btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),btVector3(point.x/100.0,point.y/100.0,0)));
	collider->setMotionState(&fallMotionState);	
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
	
	bulletStatus[3] = bulletStatus[2];
	bulletStatus[2] = bulletStatus[1];
	bulletStatus[1] = bulletStatus[0];
	bulletStatus[0] = "";
	
	if (!numSimSteps)
	 bulletStatus[0] = "Interpolated transforms";
	 else
	 {
	 if (numSimSteps > maxSimSubSteps)
	 {
	 //detect dropping frames
	 bulletStatus[0] = "Dropped (" + ofToString(numSimSteps - maxSimSubSteps) +") simulation steps out of " + ofToString(numSimSteps);
	 } else
	 {
	 bulletStatus[0] = "Simulated (" + ofToString(numSimSteps) + ") steps";
	 }
	 }
	 
#endif
	
	// Text
	
	textUpdate();
	
	// Silhouette

	dynamicsWorld->removeRigidBody(silhouette1);
	dynamicsWorld->removeRigidBody(silhouette2);
	dynamicsWorld->removeRigidBody(silhouette3);
	
	delete silhouette1;
	delete silhouette2;
	delete silhouette3;

}


void testApp::textUpdate(){
	
	for (int i=0;i<3;i++){
		if(textState[i] == TEXT_STATE_RESET){
			texts[i].clearBullet();
			texts[i] = Text();
			texts[i].setText("");
			fonts[i] = TextFontHolder("ApexSerif-Book.otf", textFontSize[i]);
			texts[i].setFont(&fonts[i]);
			texts[i].setWordBlocks(true);
			textState[i] = TEXT_STATE_REFRESH;
		}
		if(!textAnimate[i]){
			if(textState[i] == TEXT_STATE_PHYSICS_ENABLED){
				textState[i] = TEXT_STATE_PHYSICS_DISABLE;
			}
			if(textRefresh[i] && textState[i] == TEXT_STATE_REFRESHED){
				textState[i] = TEXT_STATE_REFRESH;
			}
			if(textRefresh[i] && textState[i] == TEXT_STATE_PHYSICS_DISABLED){
				textState[i] = TEXT_STATE_REFRESH;
			}
		} else {
			if(textState[i] != TEXT_STATE_PHYSICS_ENABLED){
				textState[i] = TEXT_STATE_PHYSICS_ENABLE;
			}
		}
		if(textState[i] == TEXT_STATE_CLEAR){
			texts[i].clear();
			texts[i].setText("");
			texts[i].setWordBlocks(true);
			textState[i] = TEXT_STATE_CLEARED;
		}
		if(textState[i] == TEXT_STATE_REFRESH){
			texts[i].clearBullet();
			if(texts[i].getFontSize() != textFontSize[i] ||
			   texts[i].getText() != textStrings[i] ||
			   texts[i].getDepth() != textDepth[i] ||
			   texts[i].getWidth() != textWidth[i])
			{
				texts[i].clear();
				fonts[i] = TextFontHolder("ApexSerif-Book.otf", textFontSize[i]);
				texts[i].setFont(&fonts[i]);
				texts[i].setText(textStrings[i]);
				texts[i].setWordBlocks(true);
				texts[i].setDepth(textDepth[i]);
				texts[i].setWidth(textWidth[i]);
				texts[i].setLineHeight(textFontSize[i]*1.3);
				texts[i].constructText();
			}
			if(fabs(texts[i].getTranslate().x - ((i*ofGetHeight())+textPosition[i].x)) > 0.05 ||
			   fabs(texts[i].getTranslate().y - textPosition[i].y) > 0.05 ||
			   fabs(texts[i].getTranslate().z - textPosition[i].z) > 0.05)
			{
				texts[i].translate((i*ofGetHeight())+textPosition[i].x,textPosition[i].y,0);
			}
			textState[i] = TEXT_STATE_REFRESHED;
		}
		if(textState[i] == TEXT_STATE_PHYSICS_ENABLE){
			texts[i].setFriction(textFriction[i]);
			texts[i].setDamping(textDamping[i]);
			texts[i].setMass(textMass[i]);
			texts[i].setRestitution(textRestitution[i]);
			texts[i].setupBullet(dynamicsWorld);
			textState[i] = TEXT_STATE_PHYSICS_ENABLED;
		}
		if(textState[i] == TEXT_STATE_PHYSICS_ENABLED){
			if(textRefresh[i]) {
				texts[i].setFriction(textFriction[i]);
				texts[i].setDamping(textDamping[i]);
				texts[i].setRestitution(textRestitution[i]);
			}
			texts[i].updateBullet();
		}
		if(textState[i] == TEXT_STATE_PHYSICS_DISABLE){
			texts[i].clearBullet();
			textState[i] = TEXT_STATE_PHYSICS_DISABLED;
		}
	}
}

//--------------------------------------------------------------

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

	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];	

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
	
	cvReleaseMat( &translate );
	cvReleaseMat( &src_mat );
	cvReleaseMat( &dst_mat );

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
	
	cvReleaseMat( &translate );
	cvReleaseMat( &src_mat );
	cvReleaseMat( &dst_mat );
	
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
	
	cvReleaseMat( &translate );
	cvReleaseMat( &src_mat );
	cvReleaseMat( &dst_mat );
	
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
	
	
	//Debug perspective overview	
	
	if(debug) {
	
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
	gluLookAt(eyeX*1.5, eyeY*3.0, dist*4.0, eyeX*1.5, eyeY*2.0, 0.0, 1.0, 0.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)ofGetWidth()/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport((ofGetWidth()/3.0), 0, ofGetWidth()/6.0, ofGetHeight());

	glTranslatef(-ofGetHeight(), 0, 0);

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
	
	glClear(GL_DEPTH_BUFFER_BIT);
		
		bool statusBefore = status;
		status = false;
		debug = false;
		drawViewport();
		debug = true;
		status = statusBefore;
				
	ofEnableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	ofSetColor(0, 64, 255,64);
	ofFill();
	ofRect(0,0,ofGetHeight()*3.0,ofGetWidth()/3.0);

	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	}
	
#else
	gluOrtho2D(1., 0., 4.0/3.0, 0.);
	drawViewport();
	
#endif

}

//--------------------------------------------------------------
void testApp::drawViewport(){

	//** grids etc. for projection calibration

	if(moveProjectorCorners){
		glTranslatef(0.0, 0.0, -400.0);
		ofSetColor(255,255,255);
		testCard1.draw(0,0,ofGetHeight(),ofGetWidth()/3.0);
		testCard2.draw(ofGetHeight(),0,ofGetHeight(),ofGetWidth()/3.0);
		testCard3.draw(ofGetHeight()*2.0,0,ofGetHeight(),ofGetWidth()/3.0);
		
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
	
	glEnable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	
	//Draw Background
	
	glPushMatrix();
	glTranslatef(0.0, 0.0, -1000.0);
	
	for(int i=0;i<3;i++){
		ofSetColor(backgroundColorR[i]*255, backgroundColorG[i]*255, backgroundColorB[i]*255, 255);
		ofRect(ofGetHeight()*i, 0, ofGetHeight(), ofGetWidth()/3.0);
	}
	glTranslatef(0.0, 0.0, 1000.0);
	glPopMatrix();
	
	
	//Draw Letters

	for(int i=0;i<3;i++){
		ofSetColor(textColorR[i]*255, textColorG[i]*255, textColorB[i]*255, textColorA[i]*255);
		texts[i].drawText();
	}
	

	ofSetColor(255, 255, 255);

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
	
	//trackerTexture.draw(0,0);	
	
	if(debug){
		ofEnableAlphaBlending();
		glPushMatrix();
		glTranslatef(0,0,10);
		ofSetColor(8, 8, 16, 250);
		ofRect(ofGetHeight(),0,ofGetHeight(),(2.0*(ofGetWidth()/6.0)/3.0));
		for(int i=0;i<roundf((ofGetWidth()/6.0)/3.0);i+=3){
			ofSetColor(8, 8, 16, 250*(1.0-(i/((ofGetWidth()/6.0)/3.0))));
			ofRect(ofGetHeight(),(2.0*(ofGetWidth()/6.0)/3.0)+i, ofGetHeight(), 3);
		}
		glPopMatrix();
	}
	
	if(status){
		glDisable(GL_DEPTH_TEST);
		ofEnableAlphaBlending();
		glPushMatrix();
		glTranslatef(0,0,10);
		
		ofSetColor(8, 8, 8, 127);
		ofRect(0,0,ofGetWidth(),100);
		
		ofSetColor(255, 255, 255);

		glPushMatrix();
		glTranslatef(10, 10, 1.0);
		glTranslatef(ofGetHeight()-(190*(4/3.0)), 0, 0);
		camera1.grayDiff.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(90*(4/3.0), 0, 0);
		camera1.grayImage.draw(0,0,(80*(4/3.0)),80);
		glPopMatrix();
		
		glPushMatrix();
		glTranslatef(10, 10, 1.0);
		glTranslatef((2*ofGetHeight())-(190*(4/3.0)), 0, 0);
		camera2.grayDiff.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(90*(4/3.0), 0, 0);
		camera2.grayImage.draw(0,0,(80*(4/3.0)),80);
		glPopMatrix();
		
		glPushMatrix();
		glTranslatef(10, 10, 1.0);
		glTranslatef((3*ofGetHeight())-(190*(4/3.0)), 0, 0);
		camera3.grayDiff.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(90*(4/3.0), 0, 0);
		camera3.grayImage.draw(0,0,(80*(4/3.0)),80);
		glPopMatrix();
		
		
		glPushMatrix();
		ofSetColor(255, 255, 255);
		glTranslatef(10, 20, 1.0);
		statusFont.drastring("FPS: " + ofToString(ofGetFrameRate(),2),0,0);
		glTranslatef(0, 20, 0);
		statusFont.drastring(bulletStatus[3],0,0);
		glTranslatef(0, 13, 0);
		statusFont.drastring(bulletStatus[2],0,0);
		glTranslatef(0, 13, 0);
		statusFont.drastring(bulletStatus[1],0,0);
		glTranslatef(0, 13, 0);
		statusFont.drastring(bulletStatus[0],0,0);
		glPopMatrix();
		
		
		glPopMatrix();
	}
	
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
			ofShowCursor();
			vidTracker.videoSettings();
			ofHideCursor();
			break;
		case 'd':
			debug = !debug;
			break;
		case 's':
			status = !status;
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

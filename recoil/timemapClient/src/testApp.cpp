#define USE_PARALLEL_DISPATCHER 1

#include "testApp.h"


const int maxProxies = 32766;
const int maxOverlap = 65535;
const int rainDropSize = 40;

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
	
	ofBackground(0, 0, 0);
	ofSetBackgroundAuto(false);
	//ofSetFrameRate(30);
	
	// OSC Setup for sharedVariables
	
	receiver.setup(8001);
	sender.setup("localhost", 8000);
	
	ofxOscMessage m;
	m.setAddress( ("/sendVariables/please"));
	m.addIntArg(8001);
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

	worldWrapX = false;
	worldWrapY = false;
	worldGround = true;
	worldConstrainZ = false;
	
	sharedVariables.push_back(SharedVariable(&btGravity->m_floats[0],	"worldGravityX"));
	sharedVariables.push_back(SharedVariable(&btGravity->m_floats[1],	"worldGravityY"));
	sharedVariables.push_back(SharedVariable(&btGravity->m_floats[2],	"worldGravityZ"));
	sharedVariables.push_back(SharedVariable(&worldWrapX,				"worldWrapX"));
	sharedVariables.push_back(SharedVariable(&worldWrapY,				"worldWrapY"));
	sharedVariables.push_back(SharedVariable(&worldConstrainZ,			"worldConstrainZ"));
	sharedVariables.push_back(SharedVariable(&worldGround,				"worldGround"));
	sharedVariables.push_back(SharedVariable(&worldWalls,				"worldWalls"));
	
	bulletSetup();

	// Texts

	textSetup();

	for(int i=0;i<3;i++){
		sharedVariables.push_back(SharedVariable(&textStrings[i],		"textStrings"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textFontSize[i],		"textFontSize"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textState[i],			"textState"			+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textScaffolding[i],	"textScaffolding"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textRefresh[i],		"textRefresh"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&textAnimate[i],		"textAnimate"		+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&wordBlocks[i],		"wordBlocks"		+ofToString(i, 0)));
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
		sharedVariables.push_back(SharedVariable(&textWalls[i],			"textWalls"			+ofToString(i, 0)));
		
		backgroundColorR[i] = 0.0;
		backgroundColorG[i] = 0.0;
		backgroundColorB[i] = 0.0;
		backgroundColorA[i] = 1.0;
		
		sharedVariables.push_back(SharedVariable(&backgroundColorR[i],	"backgroundColorR"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&backgroundColorG[i],	"backgroundColorG"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&backgroundColorB[i],	"backgroundColorB"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&backgroundColorA[i],	"backgroundColorA"	+ofToString(i, 0)));
	
		textState[i] = TEXT_STATE_RESET;
		textAnimate[i] = false;
		textWallsState[i] = false;
		
		camEnable[i] = false;
		camRefresh[i] = false;
		camCaptureBackground[1] = false;
		camDebug[i] = false;
		camThreshold[i] = 31;
		
		sharedVariables.push_back(SharedVariable(&camEnable[i],				"camEnable"				+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&camRefresh[i],			"camRefresh"			+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&camCaptureBackground[i],	"camCaptureBackground"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&camDebug[i],				"camDebug"				+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&camThreshold[i],			"camThreshold"			+ofToString(i, 0)));
		
		blobColorR[3] = 1.0;
		blobColorG[3] = 1.0;
		blobColorB[3] = 1.0;
		blobColorA[3] = 0.5;
		blobDamping[3] = 20;
		blobExpansion[3] = 0.0;
		
		sharedVariables.push_back(SharedVariable(&blobColorR[i],	"blobColorR"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&blobColorG[i],	"blobColorG"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&blobColorB[i],	"blobColorB"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&blobColorA[i],	"blobColorA"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&blobDamping[i],	"blobDamping"	+ofToString(i, 0)));
		sharedVariables.push_back(SharedVariable(&blobExpansion[i],	"blobExpansion"	+ofToString(i, 0)));
	
	}
	
	// 3x updates making sure remote enabling of animation is not crashing us on startup
	textUpdate();
	textUpdate();
	textUpdate();

	// Rain
	
	rainEnable = false;
	rainTrigger = false;
	rainDropCount = 0;
	
	yOffset = 0;
	
	sharedVariables.push_back(SharedVariable(&rainEnable,	"rainEnable"));
	sharedVariables.push_back(SharedVariable(&rainTrigger,	"rainTrigger"));
	sharedVariables.push_back(SharedVariable(&rainDropCount,"rainDropCount"));
	
	rainFontSmall = TextFontHolder("GenAR102.TTF",rainDropSize*0.75);
	rainFontMedium = TextFontHolder("GenAR102.TTF",rainDropSize);
	rainFontBig = TextFontHolder("GenAR102.TTF",rainDropSize*1.25);
	
	rainFontSmall.setLineHeight(rainDropSize*1.1*0.75);
	rainFontMedium.setLineHeight(rainDropSize*1.1);
	rainFontBig.setLineHeight(rainDropSize*1.1*1.25);

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
	
	//tracking kamera
	vidTracker.setVerbose(true);
	vidTracker.setDeviceID(0);
	vidTracker.initGrabber(720,576);

	colorImg.allocate(720,576);
	
	camera[0].setup(&vidTracker, &trackerTexture, &colorImg,  0,0,vidTracker.width/2, vidTracker.height/2);
	camera[1].setup(&vidTracker, &trackerTexture, &colorImg,  vidTracker.width/2,0,vidTracker.width/2, vidTracker.height/2);
	camera[2].setup(&vidTracker, &trackerTexture, &colorImg,  0,vidTracker.height/2,vidTracker.width/2, vidTracker.height/2);
	
	//delay kameraer
	
	vidLeft.setVerbose(true);
	vidLeft.setDeviceID(1);
	vidLeft.initGrabber(720,576);
	
	vidRight.setVerbose(true);
	vidRight.setDeviceID(2);
	vidRight.initGrabber(720,576);
	
	// screen snapshots
	
	makeSnaps = false;
	memset(snapString, 0, 255);		// clear the string by setting all chars to 0
	snapCounter = 0;

	// status displays
	
	statusFont = TextFontHolder("ApexNew-Medium.otf",10);
	statusFontBold = TextFontHolder("ApexNew-Bold.otf",10);

	perspectiveOffset = 0.0;
	statusOffset = 0.0;

	showPerspective = false;
	showStatus = false;
	showStatusCams = false;
	
	sharedVariables.push_back(SharedVariable(&showStatus,		"showStatus"));
	sharedVariables.push_back(SharedVariable(&showPerspective,	"showPerspective"));
	sharedVariables.push_back(SharedVariable(&showStatusCams,	"showStatusCams"));

	screensAliveTime = 0;
	screensAliveLastTime = 0;
	
	sharedVariables.push_back(SharedVariable(&screensAliveTime,		"screensAliveTime"));
	sharedVariables.push_back(SharedVariable(&screensAliveLastTime,	"screensAliveLastTime"));
	
	controlPanelOnline = false;
	
	frameRate = 0.0;
	sharedVariables.push_back(SharedVariable(&frameRate,	"frameRate"));
	
	testCard1.loadImage("768x1024.001.png");
	testCard2.loadImage("768x1024.002.png");
	testCard3.loadImage("768x1024.003.png");
	
	// Create a new tessellation object 
	//	tobj = gluNewTess(); 

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
		wordBlocks[i] = true;
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
		int maxNumOutstandingTasks = 8;
		
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
		
	btVector3 worldAabbMin(0,0,-1000);
	btVector3 worldAabbMax(globals.window.height*3.0, globals.window.width/3.0, 1000);
	
	broadphase = new btDbvtBroadphase();
	//broadphase = new bt32BitAxisSweep3(worldAabbMin,worldAabbMax,maxProxies);
		
		
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

	dynamicsWorld->getSolverInfo().m_numIterations = 8;
	dynamicsWorld->getSolverInfo().m_solverMode = SOLVER_SIMD;
	
	dynamicsWorld->getDispatchInfo().m_enableSPU = true;
	//dynamicsWorld->setGravity(btVector3(0,10,0));
	dynamicsWorld->setGravity(*btGravity);
	
	//Ground
	btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0,-1.0,0),1);
	btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0.0,((globals.window.width/3.0)+100.0)/100.0,0)));
	btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0,groundMotionState,groundShape,btVector3(0,0,0));
	groundRigidBody = new btRigidBody(groundRigidBodyCI);
	groundRigidBody->setFriction(0.01);


	//Left World Wall
	
	btCollisionShape* leftWallShape = new btStaticPlaneShape(btVector3(1,0,0),1);
	btDefaultMotionState* leftWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(-1.0,0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo leftWallRigidBodyCI(0,leftWallMotionState,leftWallShape,btVector3(0,0,0));
	leftWallRigidBody = new btRigidBody(leftWallRigidBodyCI);
	
	//Right World Wall
	
	btCollisionShape* rightWallShape = new btStaticPlaneShape(btVector3(-1,0,0),1);
	btDefaultMotionState* rightWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3((((globals.window.height*3.0)+100.0)/100.0),0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo rightWallRigidBodyCI(0,rightWallMotionState,rightWallShape,btVector3(0,0,0));
	rightWallRigidBody = new btRigidBody(rightWallRigidBodyCI);
	

	//Front World Wall
	
	btCollisionShape* frontWallShape = new btStaticPlaneShape(btVector3(0,0,1),1);
	btDefaultMotionState* frontWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0.0,0.0,-10)));
	btRigidBody::btRigidBodyConstructionInfo frontWallRigidBodyCI(0,frontWallMotionState,frontWallShape,btVector3(0,0,0));
	frontWallRigidBody = new btRigidBody(frontWallRigidBodyCI);
	dynamicsWorld->addRigidBody(frontWallRigidBody);
	
	//Back World Wall
	
	btCollisionShape* backWallShape = new btStaticPlaneShape(btVector3(0,0,-1),1);
	btDefaultMotionState* backWallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0.0,0.0,10)));
	btRigidBody::btRigidBodyConstructionInfo backWallRigidBodyCI(0,backWallMotionState,backWallShape,btVector3(0,0,0));
	backWallRigidBody = new btRigidBody(backWallRigidBodyCI);
	dynamicsWorld->addRigidBody(backWallRigidBody);
	
	//Text 1 walls
	
	btCollisionShape* textWall1LeftShape = new btBoxShape(btVector3(0.015,(globals.window.width/3.0)/100.0,6000));
	btDefaultMotionState* textWall1LeftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(0.0,0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo textWall1LeftRigidBodyCI(0.,textWall1LeftMotionState,textWall1LeftShape,btVector3(0,0,0));
	textWall1LeftRigidBody = new btRigidBody(textWall1LeftRigidBodyCI);
	textWall1LeftRigidBody->setCollisionFlags( textWall1LeftRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	textWall1LeftRigidBody->setActivationState(DISABLE_DEACTIVATION);
	
	btCollisionShape* textWall1RightShape = new btBoxShape(btVector3(0.015,(globals.window.width/3.0)/100.0,6000));
	btDefaultMotionState* textWall1RightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3((((globals.window.height*3.0))/100.0)/3.0,0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo textWall1RightRigidBodyCI(0.,textWall1RightMotionState,textWall1RightShape,btVector3(0,0,0));
	textWall1RightRigidBody = new btRigidBody(textWall1RightRigidBodyCI);
	textWall1RightRigidBody->setCollisionFlags( textWall1RightRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	textWall1RightRigidBody->setActivationState(DISABLE_DEACTIVATION);
	
	//Text 2 walls

	btCollisionShape* textWall2LeftShape = new btBoxShape(btVector3(0.015,(globals.window.width/3.0)/100.0,6000));
	btDefaultMotionState* textWall2LeftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3((((globals.window.height*3.0))/100.0)/3.0,0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo textWall2LeftRigidBodyCI(0.,textWall2LeftMotionState,textWall2LeftShape,btVector3(0,0,0));
	textWall2LeftRigidBody = new btRigidBody(textWall2LeftRigidBodyCI);
	textWall2LeftRigidBody->setCollisionFlags( textWall2LeftRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	textWall2LeftRigidBody->setActivationState(DISABLE_DEACTIVATION);
	
	btCollisionShape* textWall2RightShape = new btBoxShape(btVector3(0.015,(globals.window.width/3.0)/100.0,6000));
	btDefaultMotionState* textWall2RightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(((((globals.window.height*3.0))/100.0)/3.0)*2.0,0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo textWall2RightRigidBodyCI(0.,textWall2RightMotionState,textWall2RightShape,btVector3(0,0,0));
	textWall2RightRigidBody = new btRigidBody(textWall2RightRigidBodyCI);
	textWall2RightRigidBody->setCollisionFlags( textWall2RightRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	textWall2RightRigidBody->setActivationState(DISABLE_DEACTIVATION);
	
	//Text 3 walls
	
	btCollisionShape* textWall3LeftShape = new btBoxShape(btVector3(0.015,((globals.window.width/3.0)+100.0)/100.0,6000));
	btDefaultMotionState* textWall3LeftMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3(((((globals.window.height*3.0))/100.0)/3.0)*2.0,0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo textWall3LeftRigidBodyCI(0.,textWall3LeftMotionState,textWall3LeftShape,btVector3(0,0,0));
	textWall3LeftRigidBody = new btRigidBody(textWall3LeftRigidBodyCI);
	textWall3LeftRigidBody->setCollisionFlags( textWall3LeftRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	textWall3LeftRigidBody->setActivationState(DISABLE_DEACTIVATION);

	btCollisionShape* textWall3RightShape = new btBoxShape(btVector3(0.015,((globals.window.width/3.0)+100.0)/100.0,6000));
	btDefaultMotionState* textWall3RightMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1), btVector3((((globals.window.height*3.0))/100.0),0.0,0)));
	btRigidBody::btRigidBodyConstructionInfo textWall3RightRigidBodyCI(0.,textWall3RightMotionState,textWall3RightShape,btVector3(0,0,0));
	textWall3RightRigidBody = new btRigidBody(textWall3RightRigidBodyCI);
	textWall3RightRigidBody->setCollisionFlags( textWall3RightRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	textWall3RightRigidBody->setActivationState(DISABLE_DEACTIVATION);

	
	//Mouse Collider
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
	// printf("%i update\n", ofGetFrameNum());    
	
	frameRate = ofGetFrameRate();
	
	int screensAliveTimeBeforeUpdate = screensAliveTime;
	
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
	
	controlPanelOnline = (ofGetElapsedTimeMillis() - controlPanelOnlineTime < 500);
	
	if(screensAliveTimeBeforeUpdate != screensAliveTime){
		controlPanelOnlineTime = ofGetElapsedTimeMillis();
		screensAliveLastTime = screensAliveTime + 500;
	}
	
	// Videograbber
	
	vidTracker.grabFrame();
	ofPoint leftPoint = ofPoint(-1,-1);
	ofPoint rightPoint = ofPoint(-1,-1);
	ofxPoint2f p;
	if (vidTracker.isFrameNew()){		
		colorImg.setFromPixels(vidTracker.getPixels(), 720,576);
			for(int i = 0; i < 3; i++) {
			if(camCaptureBackground[i]){
				camera[i].bLearnBakground = true;
				camCaptureBackground[i] = false;
			}
			camera[i].threshold = camThreshold[i];
			if(camRefresh[i])
				camera[i].update(blobExpansion[i]);
		}
	}
		
	//Gravity
	
	dynamicsWorld->setGravity(*btGravity);
	
	//Ground 	
	
	if(worldGround) {
		if(!worldGroundState) {
			dynamicsWorld->addRigidBody(groundRigidBody);
			worldGroundState = true;
		}
	} else {
		if(worldGroundState){
			dynamicsWorld->removeRigidBody(groundRigidBody);
			worldGroundState = false;
		}
	}
	
	//World Walls
	
	if(worldWalls){
		if(!worldWallsState) {
			dynamicsWorld->addRigidBody(leftWallRigidBody);
			dynamicsWorld->addRigidBody(rightWallRigidBody);
			worldWallsState = true;
		}
	} else {
		if(worldWallsState){
			dynamicsWorld->removeRigidBody(leftWallRigidBody);
			dynamicsWorld->removeRigidBody(rightWallRigidBody);
			worldWallsState = false;
		}
	}
	
	// Text 1 Walls
	
	if(textWalls[0]){
		if(!textWallsState[0]){
			dynamicsWorld->addRigidBody(textWall1LeftRigidBody);
			dynamicsWorld->addRigidBody(textWall1RightRigidBody);
			textWallsState[0] = true;
		}
	} else {
		if(textWallsState[0]){
			dynamicsWorld->removeRigidBody(textWall1LeftRigidBody);
			dynamicsWorld->removeRigidBody(textWall1RightRigidBody);
			textWallsState[0] = false;
		}
	}
	
	// Text 2 Walls
	
	if(textWalls[1]){
		if(!textWallsState[1]){
			dynamicsWorld->addRigidBody(textWall2LeftRigidBody);
			dynamicsWorld->addRigidBody(textWall2RightRigidBody);
			textWallsState[1] = true;
		}
	} else {
		if(textWallsState[1]){
			dynamicsWorld->removeRigidBody(textWall2LeftRigidBody);
			dynamicsWorld->removeRigidBody(textWall2RightRigidBody);
			textWallsState[1] = false;
		}
	}
	
	// Text 3 Walls
	
	if(textWalls[2]){
		if(!textWallsState[2]){
			dynamicsWorld->addRigidBody(textWall3LeftRigidBody);
			dynamicsWorld->addRigidBody(textWall3RightRigidBody);
			textWallsState[2] = true;
		}
	} else {
		if(textWallsState[2]){
			dynamicsWorld->removeRigidBody(textWall3LeftRigidBody);
			dynamicsWorld->removeRigidBody(textWall3RightRigidBody);
			textWallsState[2] = false;
		}
	}
	
	// Silhouette 1
	int mult = 1; 

	btConvexHullShape * silhouette1Shape = new btConvexHullShape();
	if(camera[0].contourFinder.nBlobs > 0){
		for(int i=0;i<camera[0].contourFinder.nBlobs;i++){
			for(int j=0;j<camera[0].contourFinder.blobs[i].nPts;j++){
				silhouette1Shape->addPoint(btVector3(camera[0].contourFinder.blobs[i].pts[j].x*globals.window.height/100.0, camera[0].contourFinder.blobs[i].pts[j].y*(globals.window.width/3.0)/100.0,+20000*mult));
				silhouette1Shape->addPoint(btVector3(camera[0].contourFinder.blobs[i].pts[j].x*globals.window.height/100.0, camera[0].contourFinder.blobs[i].pts[j].y*(globals.window.width/3.0)/100.0,-20000*mult));
				mult *= -1;
			}
		}
		/** USING THE SIMPLIFiER
		for(int i=0;i<camera[0].simplify->numPoints;i++){
			silhouette1Shape->addPoint(btVector3(camera[0].simplify->points[i].x*globals.window.height/100.0, camera[0].simplify->points[i].y*(globals.window.width/3.0)/100.0,+20000*mult));
			silhouette1Shape->addPoint(btVector3(camera[0].simplify->points[i].x*globals.window.height/100.0, camera[0].simplify->points[i].y*(globals.window.width/3.0)/100.0,-20000*mult));
			mult *= -1;
			//cout<<camera[0].simplify->points[i].x*globals.window.height/100.0<< "  ,  "<<camera[0].simplify->points[i].y*(globals.window.width/3.0)/100.0<<endl;
		}
		// **/
	}
	btVector3 silhouette1Pos = btVector3(0,0,0);
	silhouette1MotionState = btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette1Pos));
	btScalar silhouette1Mass = 0;
	btVector3 silhouette1FallInertia(0,0,0);
	silhouette1Shape->calculateLocalInertia(silhouette1Mass,silhouette1FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette1RigidBodyCI(silhouette1Mass,&silhouette1MotionState,silhouette1Shape,silhouette1FallInertia);
	btRigidBody* silhouette1 = new btRigidBody(silhouette1RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette1->setRestitution(0.02);
	silhouette1->setFriction(0.1);
	silhouette1->setCollisionFlags( silhouette1->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette1->setActivationState(DISABLE_DEACTIVATION);
	if(camEnable[0]){
		dynamicsWorld->addRigidBody(silhouette1);
	}

	// Silhouette 2

	btConvexHullShape * silhouette2Shape = new btConvexHullShape();
	if(camera[1].contourFinder.nBlobs > 0){
		for(int i=0;i<camera[1].contourFinder.nBlobs;i++){
			for(int j=0;j<camera[1].contourFinder.blobs[i].nPts;j++){
				silhouette2Shape->addPoint(btVector3((globals.window.height+(camera[1].contourFinder.blobs[i].pts[j].x*globals.window.height))/100.0, camera[1].contourFinder.blobs[i].pts[j].y*(globals.window.width/3.0)/100.0,+20000*mult));
				silhouette2Shape->addPoint(btVector3((globals.window.height+(camera[1].contourFinder.blobs[i].pts[j].x*globals.window.height))/100.0, camera[1].contourFinder.blobs[i].pts[j].y*(globals.window.width/3.0)/100.0,-20000*mult));
				mult *= -1;
			}
		}
		/** USING THE SIMPLIFiER
		for(int i=0;i<camera[1].simplify->numPoints;i++){
			silhouette2Shape->addPoint(btVector3((globals.window.height+(camera[1].simplify->points[i].x*globals.window.height))/100.0, camera[1].simplify->points[i].y*(globals.window.width/3.0)/100.0,+20000*mult));
			silhouette2Shape->addPoint(btVector3((globals.window.height+(camera[1].simplify->points[i].x*globals.window.height))/100.0, camera[1].simplify->points[i].y*(globals.window.width/3.0)/100.0,-20000*mult));
			mult *= -1;
			//cout<<camera[1].simplify->points[i].x*globals.window.height/100.0<< "  ,  "<<camera[1].simplify->points[i].y*(globals.window.width/3.0)/100.0<<endl;
		}
		 // **/
	}
	btVector3 silhouette2Pos = btVector3(0,0,0);
	silhouette2MotionState = btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette2Pos));
	btScalar silhouette2Mass = 0;
	btVector3 silhouette2FallInertia(0,0,0);
	silhouette2Shape->calculateLocalInertia(silhouette2Mass,silhouette2FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette2RigidBodyCI(silhouette2Mass,&silhouette2MotionState,silhouette2Shape,silhouette2FallInertia);
	btRigidBody* silhouette2 = new btRigidBody(silhouette2RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette2->setRestitution(0.02);
	silhouette2->setFriction(0.1);
	silhouette2->setCollisionFlags( silhouette2->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette2->setActivationState(DISABLE_DEACTIVATION);
	if(camEnable[1]){
		dynamicsWorld->addRigidBody(silhouette2);
	}
	
	// Silhouette 3
	
	btConvexHullShape * silhouette3Shape = new btConvexHullShape();
	if(camera[2].contourFinder.nBlobs > 0){
		for(int i=0;i<camera[2].contourFinder.nBlobs;i++){
			for(int j=0;j<camera[2].contourFinder.blobs[i].nPts;j++){
				silhouette3Shape->addPoint(btVector3((globals.window.height*2.0+(camera[2].contourFinder.blobs[i].pts[j].x*globals.window.height))/100.0, camera[2].contourFinder.blobs[i].pts[j].y*(globals.window.width/3.0)/100.0,+20000*mult));
				silhouette3Shape->addPoint(btVector3((globals.window.height*2.0+(camera[2].contourFinder.blobs[i].pts[j].x*globals.window.height))/100.0, camera[2].contourFinder.blobs[i].pts[j].y*(globals.window.width/3.0)/100.0,-20000*mult));
				mult *= -1;
			}
		}
		/** USING THE SIMPLIFiER
		for(int i=0;i<camera[2].simplify->numPoints;i++){
			silhouette3Shape->addPoint(btVector3((globals.window.height*2.0+(camera[2].simplify->points[i].x*globals.window.height))/100.0, camera[2].simplify->points[i].y*(globals.window.width/3.0)/100.0,+20000*mult));
			silhouette3Shape->addPoint(btVector3((globals.window.height*2.0+(camera[2].simplify->points[i].x*globals.window.height))/100.0, camera[2].simplify->points[i].y*(globals.window.width/3.0)/100.0,-20000*mult));
						mult *= -1;
			//cout<<camera[2].simplify->points[i].x*globals.window.height/100.0<< "  ,  "<<camera[2].simplify->points[i].y*(globals.window.width/3.0)/100.0<<endl;
		}
		 // **/
	}
	btVector3 silhouette3Pos = btVector3(0,0,0);
	silhouette3MotionState = btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),silhouette3Pos));
	btScalar silhouette3Mass = 0;
	btVector3 silhouette3FallInertia(0,0,0);
	silhouette3Shape->calculateLocalInertia(silhouette3Mass,silhouette3FallInertia);
	btRigidBody::btRigidBodyConstructionInfo silhouette3RigidBodyCI(silhouette3Mass,&silhouette3MotionState,silhouette3Shape,silhouette3FallInertia);
	btRigidBody* silhouette3 = new btRigidBody(silhouette3RigidBodyCI);
	//silhouette->setGravity(btVector3(0.,0.,0.));
	//silhouette->setDamping(0.99,0.9);
	silhouette3->setRestitution(0.02);
	silhouette3->setFriction(0.1);
	silhouette3->setCollisionFlags( silhouette3->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);  
	silhouette3->setActivationState(DISABLE_DEACTIVATION);
	if(camEnable[2]){
		dynamicsWorld->addRigidBody(silhouette3);
	}
	
	//Collider

	if(mouseX > 0 && mouseX < globals.window.width){
		ofHideCursor();
		point.x += (mouseX*3.0/4.0-point.x);
		point.y += (mouseY*4.0/3.0-point.y );
	} else {
		ofShowCursor();
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
	camera[0].updateWarp(d);
	
	for(int i=0;i<4;i++){
		d[i].x = (float)cameraCorners[1][i].x;
		d[i].y = (float)cameraCorners[1][i].y;
	}
	camera[1].updateWarp(d);

	for(int i=0;i<4;i++){
		d[i].x = (float)cameraCorners[2][i].x;
		d[i].y = (float)cameraCorners[2][i].y;
	}
	camera[2].updateWarp(d);
	
	
	// Text
	
	textUpdate();
	
	
	// Rain
	
	if(rainEnable) {
		rainDropCount = rainDrops.size();
		if(rainDropCount > 100){
			rainDrops[0].pauseBullet();
			rainDrops.erase(rainDrops.begin());
		}
		if(rainTrigger){
			float randomFactor = ofRandom(0.5, 1.0);
			Text drop = Text();
			drop.setText("§");
			if(rainDropCount%3 == 0){
				drop.setFont(&rainFontSmall);
				drop.setLineHeight(rainDropSize*0.75);
				drop.setWidth(rainDropSize*0.5*0.75);
			} else if ((rainDropCount+1)%3 == 0){
				drop.setFont(&rainFontMedium);
				drop.setLineHeight(rainDropSize);
				drop.setWidth(rainDropSize*0.5);
			} else if ((rainDropCount+2)%3 == 0){
				drop.setFont(&rainFontBig);
				drop.setLineHeight(rainDropSize*1.25);
				drop.setWidth(rainDropSize*0.5*1.25);
			}
			drop.setWordBlocks(true);
			drop.setDepth(50);
			drop.constructText(false);
			drop.translate(ofRandom(0.0,1.0)*(3*globals.window.height),-175,0);
			drop.setFriction(1.0);
			drop.setDamping(0.20);
			drop.setMass(0.1);
			drop.setInertia(ofRandom(-1.0,1.0),ofRandom(0.0,1.0),0.0);
			drop.setRestitution(0.8);
			drop.setupBullet(dynamicsWorld, true);
			rainDrops.push_back(drop);
		}
#pragma omp parallel for
		for(int i = 0; i < rainDrops.size(); i++){
			rainDrops[i].setWorldWrapX(worldWrapX);
			rainDrops[i].setWorldWrapY(worldWrapY);
			rainDrops[i].setWorldConstrainZ(worldConstrainZ);
			rainDrops[i].updateBullet(false);
		}
	} else {
		yOffset = 0;
		for(int i = 0; i < rainDrops.size(); i++){
			rainDrops[i].clear();
		}
		rainDrops.clear();
	}
	rainTrigger = false;
	
	
	// Walls
	 
//#define FIXED_STEP
	
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
		
	// Silhouette
	
	if(camEnable[0]){
	dynamicsWorld->removeRigidBody(silhouette1);
	}
	if(camEnable[1]){
	dynamicsWorld->removeRigidBody(silhouette2);
	}
	if(camEnable[2]){
	dynamicsWorld->removeRigidBody(silhouette3);
	}
	delete silhouette1;
	delete silhouette2;
	delete silhouette3;
		
	delete silhouette1Shape;
	delete silhouette2Shape;
	delete silhouette3Shape;
	
	//Update shared variables
	for(int i=0;i<sharedVariables.size();i++){
		sharedVariables[i].update(&sender);
	}
		
}


void testApp::textUpdate(){
	
	for (int i=0;i<3;i++){
		if(!textScaffolding[i]){
			texts[i].removeScaffolding();
		}
		if(textState[i] == TEXT_STATE_RESET){
			texts[i].clear();
			texts[i].clearBullet();
			texts[i] = Text();
			texts[i].setText("");
			fonts[i] = TextFontHolder("ApexSerif-Book.otf", textFontSize[i]);
			fonts[i].setLineHeight(textFontSize[i]*1.3);
			texts[i].setFont(&fonts[i]);
			texts[i].setLineHeight(textFontSize[i]*1.3);
			texts[i].setWordBlocks(wordBlocks[i]);
			textState[i] = TEXT_STATE_REFRESH;
		}
		if(textState[i] == TEXT_STATE_CLEAR){
			texts[i].clear();
			texts[i].setText("");
			texts[i].setWordBlocks(wordBlocks[i]);
			texts[i].setLineHeight(textFontSize[i]*1.3);
			textState[i] = TEXT_STATE_CLEARED;
		}
		if(textState[i] == TEXT_STATE_REFRESH){
			texts[i].clearBullet();
			if(texts[i].getFontSize() != textFontSize[i] ||
			   texts[i].getText() != textStrings[i] ||
			   texts[i].getDepth() != textDepth[i] ||
			   texts[i].getWidth() != textWidth[i] ||
			   texts[i].getWordBlocks() != wordBlocks[i] )
			{
				texts[i].clear();
				texts[i].setText(textStrings[i]);
				texts[i].setFont(&fonts[i]);
				fonts[i].setLineHeight(textFontSize[i]*1.3);
				fonts[i].setFontSize(textFontSize[i]);
				//texts[i].setLineHeight(textFontSize[i]*1.3);
				texts[i].setWordBlocks(wordBlocks[i]);
				texts[i].setDepth(textDepth[i]);
				texts[i].setWidth(textWidth[i]);
				texts[i].constructText(false);
			}
			if(fabs(texts[i].getTranslate().x - ((i*globals.window.height)+textPosition[i].x)) > 0.05 ||
			   fabs(texts[i].getTranslate().y - textPosition[i].y) > 0.05 ||
			   fabs(texts[i].getTranslate().z - textPosition[i].z) > 0.05)
			{
				texts[i].translate((i*globals.window.height)+textPosition[i].x,textPosition[i].y,0);
			}
			if(textScaffolding[i]){
				texts[i].createScaffolding();
			}
			textState[i] = TEXT_STATE_REFRESHED;
		}
		if(textRefresh[i] && (textState[i] == TEXT_STATE_REFRESHED || textState[i] == TEXT_STATE_CLEARED)){
				textState[i] = TEXT_STATE_REFRESH;
		}
		if(!textAnimate[i]){
			if(textState[i] == TEXT_STATE_PHYSICS_ENABLED){
				textState[i] = TEXT_STATE_PHYSICS_DISABLE;
			}
		} else {
			if(textState[i] != TEXT_STATE_PHYSICS_ENABLED){
				textState[i] = TEXT_STATE_PHYSICS_ENABLE;
			}
		}
		if(textState[i] == TEXT_STATE_PHYSICS_ENABLE){
			texts[i].setFriction(textFriction[i]);
			texts[i].setDamping(textDamping[i]);
			texts[i].setMass(textMass[i]);
			texts[i].setRestitution(textRestitution[i]);
			texts[i].setupBullet(dynamicsWorld, true);
			textState[i] = TEXT_STATE_PHYSICS_ENABLED;
		}
		if(textState[i] == TEXT_STATE_PHYSICS_ENABLED){
			texts[i].setFriction(textFriction[i]);
			texts[i].setDamping(textDamping[i]);
			texts[i].setRestitution(textRestitution[i]);
			texts[i].setWorldWrapX(worldWrapX);
			texts[i].setWorldWrapY(worldWrapY);
			texts[i].setWorldConstrainZ(worldConstrainZ);
			texts[i].updateBullet(true);
		}
		if(textState[i] == TEXT_STATE_PHYSICS_DISABLE){
			texts[i].pauseBullet();
			textState[i] = TEXT_STATE_PHYSICS_DISABLED;
		}
	}
}

//--------------------------------------------------------------

void testApp::draw(){
	
//	printf("%i draw\n", ofGetFrameNum());   
		
	glClear(GL_DEPTH_BUFFER_BIT);

	if(rainEnable)
		yOffset=100;
	else 
		yOffset = 0.0;
	
	if(showStatus){
		if(statusOffset <1.0)
			statusOffset += 0.01+((1.0-statusOffset)*0.06);
	} else {
		if(statusOffset >0.0)
			statusOffset -= 0.01+(statusOffset*0.05);
	}
	
	if(showPerspective){
		if(perspectiveOffset <1.0)
			perspectiveOffset += 0.01+((1.0-perspectiveOffset)*0.06);
	} else {
		if(perspectiveOffset >0.0)
			perspectiveOffset -= 0.01+(perspectiveOffset*0.05);
	}
	
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
	double eqnLtY[] =
	{ 0.0, 1.0, 0.0, 0.0 }; // klipper x < 0
	double eqnGtY[] =
	{ 0.0, -1.0, 0.0, 0.0 }; //klipper x > 0
	
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0.0, (float)globals.window.height,
			0.0, (float)globals.window.width/3.0, 10.0, 2500);
	
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1000,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)globals.window.width/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport(0, 0, globals.window.width/3, globals.window.height);

	CvPoint2D32f cvsrc[4];
	CvPoint2D32f cvdst[4];	

	cvsrc[0].x = 0;
	cvsrc[0].y = 4.0*globals.window.height/3.0;		
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = globals.window.width/4.0;
	cvsrc[2].y = 0;
	cvsrc[3].x = globals.window.width/4.0;
	cvsrc[3].y = 4.0*globals.window.height/3.0;	
	
	cvdst[0].y = globals.window.width/3.0*(1.0-projectorCorners[0][3].x);
	cvdst[0].x = globals.window.height*projectorCorners[0][3].y;
	cvdst[1].y = globals.window.width/3.0*(1.0-projectorCorners[0][0].x);
	cvdst[1].x = globals.window.height*projectorCorners[0][0].y;
	cvdst[2].y = globals.window.width/3.0*(1.0-projectorCorners[0][1].x);
	cvdst[2].x = globals.window.height*projectorCorners[0][1].y;
	cvdst[3].y = globals.window.width/3.0*(1.0-projectorCorners[0][2].x);
	cvdst[3].x = globals.window.height*projectorCorners[0][2].y;	
	
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
	
	glClipPlane(GL_CLIP_PLANE2, eqnLtY);
	glEnable(GL_CLIP_PLANE2);
	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);
	
	glPushMatrix();
	glTranslatef(globals.window.height, 0, 0);
	glClipPlane(GL_CLIP_PLANE4, eqnGtX);
	glEnable(GL_CLIP_PLANE4);
	glPopMatrix();

	drawViewport(0);

	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	//A frame
	glDisable(GL_DEPTH_TEST);
	ofSetColor(0,0,0,255);
	ofRect((globals.window.height*0)-100, -100, globals.window.height+200, 100); //top
	ofRect((globals.window.height*(0+1)), -100, 100, (globals.window.width/3.0)+200); //right
	if(!rainEnable)
		ofRect((globals.window.height*0)-100, (globals.window.width/3.0), globals.window.height+200, (globals.window.width/3.0)+100); //bottom
	ofRect((globals.window.height*0)-100, -100, 100, (globals.window.width/3.0)+200); //left
	
	//Screen 2
	
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0.0, (float)globals.window.height,
			0.0, (float)globals.window.width/3.0, 10.0, 2500);
	
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1000,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)globals.window.width/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport(globals.window.width/3.0, 0, globals.window.width/3.0, globals.window.height);
	
	cvsrc[0].x = 0;
	cvsrc[0].y = 4.0*globals.window.height/3.0;		
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = globals.window.width/4.0;
	cvsrc[2].y = 0;
	cvsrc[3].x = globals.window.width/4.0;
	cvsrc[3].y = 4.0*globals.window.height/3.0;	
	
	cvdst[0].y = globals.window.width/3.0*(1.0-projectorCorners[1][3].x);
	cvdst[0].x = globals.window.height*projectorCorners[1][3].y;
	cvdst[1].y = globals.window.width/3.0*(1.0-projectorCorners[1][0].x);
	cvdst[1].x = globals.window.height*projectorCorners[1][0].y;
	cvdst[2].y = globals.window.width/3.0*(1.0-projectorCorners[1][1].x);
	cvdst[2].x = globals.window.height*projectorCorners[1][1].y;
	cvdst[3].y = globals.window.width/3.0*(1.0-projectorCorners[1][2].x);
	cvdst[3].x = globals.window.height*projectorCorners[1][2].y;	
	
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
	
	glClipPlane(GL_CLIP_PLANE2, eqnLtY);
	glEnable(GL_CLIP_PLANE2);
	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);
	
	glPushMatrix();
	glTranslatef(globals.window.height, 0, 0);
	glClipPlane(GL_CLIP_PLANE4, eqnGtX);
	glEnable(GL_CLIP_PLANE4);
	glPopMatrix();
	
	glTranslatef(-globals.window.height, 0, 0);

	drawViewport(1);
	
	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	//A frame
	glDisable(GL_DEPTH_TEST);
	ofSetColor(0,0,0,255);
	ofRect((globals.window.height*1)-100, -100, globals.window.height+200, 100); //top
	ofRect((globals.window.height*(1+1)), -100, 100, (globals.window.width/3.0)+200); //right
	if(!rainEnable)
		ofRect((globals.window.height*1)-100, (globals.window.width/3.0), globals.window.height+200, (globals.window.width/3.0)+100); //bottom
	ofRect((globals.window.height*1)-100, -100, 100, (globals.window.width/3.0)+200); //left
	
	//Screen 3	
	
	glMatrixMode(GL_PROJECTION); 
	glLoadIdentity(); 
	glOrtho(0.0, (float)globals.window.height,
			0.0, (float)globals.window.width/3.0, 10.0, 2500);
	
	glMatrixMode(GL_MODELVIEW); 
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 1000,
			  0.0, 0.0, 0.0,
			  0.0, 1.0, 0.0);

	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)globals.window.width/3.0, 0);       // shift origin up to upper-left corner.
	
	glViewport(2.0*globals.window.width/3.0, 0, globals.window.width/3.0, globals.window.height);
	
	cvsrc[0].x = 0;
	cvsrc[0].y = 4.0*globals.window.height/3.0;		
	cvsrc[1].x = 0;
	cvsrc[1].y = 0;
	cvsrc[2].x = globals.window.width/4.0;
	cvsrc[2].y = 0;
	cvsrc[3].x = globals.window.width/4.0;
	cvsrc[3].y = 4.0*globals.window.height/3.0;	
	
	cvdst[0].y = globals.window.width/3.0*(1.0-projectorCorners[2][3].x);
	cvdst[0].x = globals.window.height*projectorCorners[2][3].y;
	cvdst[1].y = globals.window.width/3.0*(1.0-projectorCorners[2][0].x);
	cvdst[1].x = globals.window.height*projectorCorners[2][0].y;
	cvdst[2].y = globals.window.width/3.0*(1.0-projectorCorners[2][1].x);
	cvdst[2].x = globals.window.height*projectorCorners[2][1].y;
	cvdst[3].y = globals.window.width/3.0*(1.0-projectorCorners[2][2].x);
	cvdst[3].x = globals.window.height*projectorCorners[2][2].y;	
	
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
	
	glClipPlane(GL_CLIP_PLANE2, eqnLtY);
	glEnable(GL_CLIP_PLANE2);
	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);

	glPushMatrix();
		glTranslatef(globals.window.height, 0, 0);
		glClipPlane(GL_CLIP_PLANE4, eqnGtX);
		glEnable(GL_CLIP_PLANE4);
	glPopMatrix();
	
	glTranslatef(-globals.window.height*2.0, 0, 0);

	drawViewport(2);

	glDisable(GL_CLIP_PLANE2);
	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	//A frame
	glDisable(GL_DEPTH_TEST);
	ofSetColor(0,0,0,255);
	ofRect((globals.window.height*2)-100, -100, globals.window.height+200, 100); //top
	ofRect((globals.window.height*(2+1)), -100, 100, (globals.window.width/3.0)+200); //right
	if(!rainEnable)
		ofRect((globals.window.height*2)-100, (globals.window.width/3.0), globals.window.height+200, (globals.window.width/3.0)+100); //bottom
	ofRect((globals.window.height*2)-100, -100, 100, (globals.window.width/3.0)+200); //left
	
	//perspective overview	
	
	if(perspectiveOffset > 0.0) {
	
	int w, h;
	
	w = globals.window.width/6.0;
	h = globals.window.height;
	
	float halfFov, theTan, screenFov, aspect;
	screenFov 		= 60.0f;
	
	float eyeX 		= (float)w / 2.0;
	float eyeY 		= (((1.0-perspectiveOffset)*-1.0)+1.0) * (float)h / 2.0;
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
	gluLookAt(eyeX*1.5, eyeY*3.5, dist*4.0, eyeX*1.5, (((1.0-perspectiveOffset)*-1.8)+1.0)*eyeY*(1+(statusOffset)), 0.0, 1.0, 0.0, 0.0);
	
	glScalef(1, -1, 1);           // invert Y axis so increasing Y goes down.
  	glTranslatef(0, -(float)globals.window.width/3.0, 0);       // shift origin up to upper-left corner.
	glTranslatef(0,(1.0-statusOffset)*-100.0,0);
	
	glViewport((globals.window.width/3.0), 0, globals.window.width/6.0, globals.window.height);

	glTranslatef(-globals.window.height, 0, 0);

	glPushMatrix();
	glTranslatef(-globals.window.width/3.0, 0, 0);
	glClipPlane(GL_CLIP_PLANE3, eqnLtX);
	glEnable(GL_CLIP_PLANE3);
	glPopMatrix();
	
	glPushMatrix();
	glTranslatef(globals.window.width, 0, 0);
	glClipPlane(GL_CLIP_PLANE4, eqnGtX);
	glEnable(GL_CLIP_PLANE4);
	glPopMatrix();
	
	glClear(GL_DEPTH_BUFFER_BIT);
		
		bool showStatusBefore = showStatus;
		bool showPerspectiveBefore = showPerspective;
		float statusOffsetBefore = statusOffset;
		float perspectiveOffsetBefore = perspectiveOffset;
		showStatus = false;
		statusOffset = 0.0;
		perspectiveOffset = 0.0;
		showPerspective = false;
		
		drawViewport(-1);
		
		showPerspective = showPerspectiveBefore;
		showStatus = showStatusBefore;
		statusOffset = statusOffsetBefore;
		perspectiveOffset = perspectiveOffsetBefore;
				
	ofEnableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	ofSetColor(0, 64, 255,64*perspectiveOffset);
	ofFill();
	ofRect(0,0,globals.window.height*3.0,globals.window.width/3.0);

	glDisable(GL_CLIP_PLANE3);
	glDisable(GL_CLIP_PLANE4);
	
	}
	
#else
	gluOrtho2D(1., 0., 4.0/3.0, 0.);
	drawViewport();
	
#endif

}

//--------------------------------------------------------------
void testApp::drawViewport(int screen){
	//** grids etc. for projection calibration
	glDisable(GL_DEPTH_TEST);
	ofEnableAlphaBlending();
	ofFill();

	//Draw Background
	
	glPushMatrix();
	glTranslatef(0.0, 0.0, -1000.0);
	
	if(rainEnable) {
		glPushMatrix();
		glTranslatef(0.0, yOffset, 0);
		ofFill();
		ofSetColor(0,0,0,255);
		ofRect(0,-500, (globals.window.height*3.0), (500+(globals.window.width/3.0)));
		glPopMatrix();
	} else {
		for(int i=0;i<3;i++){
			if(i == screen || screen < 0){
				ofFill();
				ofSetColor(backgroundColorR[i]*255, backgroundColorG[i]*255, backgroundColorB[i]*255, backgroundColorA[i]*255);
				ofRect((globals.window.height*i),0, globals.window.height, (globals.window.width/3.0));
			}
		}
	}
	glTranslatef(0.0, 0.0, 1000.0);
	glPopMatrix();
	
	glEnable(GL_DEPTH_TEST);

	if(moveProjectorCorners){
		glPushMatrix();
		glTranslatef(0.0, 0.0, -400.0);
		ofSetColor(255,255,255,255);
		if(screen == 0 || screen < 0)
		testCard1.draw(0,0,globals.window.height,globals.window.width/3.0);
		if(screen == 1 || screen < 0)
		testCard2.draw(globals.window.height,0,globals.window.height,globals.window.width/3.0);
		if(screen == 2 || screen < 0)
		testCard3.draw(globals.window.height*2.0,0,globals.window.height,globals.window.width/3.0);
		
		ofEnableAlphaBlending();
		ofDisableSmoothing();
		ofSetColor(255, 255, 255, 20);
		for(int v=0;v< globals.window.height*3.0;v+=10){
			ofLine(v, 0, v, globals.window.width/3.0);
		}
		for(int h=0;h< globals.window.width/3.0;h+=10){
			ofLine(0, h, globals.window.height*3.0, h);
		}
		glTranslatef(0.0, 0.0, 400.0);
		glPopMatrix();
	}
	
	//Draw Letters

	ofEnableAlphaBlending();
	for(int i=0;i<3;i++){
		ofFill();
		ofSetColor(textColorR[i]*255, textColorG[i]*255, textColorB[i]*255, textColorA[i]*255);
		texts[i].drawText();
		//texts[i].drawBricks();
		
	}
	
	// Draw Rain
	
	if(rainEnable){
		glPushMatrix();
		glTranslatef(0.0, yOffset, 0);
		ofFill();
		glEnable(GL_DEPTH_TEST);
		for(int i = 0; i < rainDrops.size(); i++){
			ofFill();
			ofSetColor(255,255,255,255);
			rainDrops[i].drawText();
		}
		glPopMatrix();
	}
		
	ofSetColor(255, 255, 255);

	//Collider
	
	if (mouseX > 0){
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
	}
	
	if(perspectiveOffset > 0.0){
		glDisable(GL_DEPTH_TEST);
		ofEnableAlphaBlending();
		glPushMatrix();
		glTranslatef(0,0,10);
		ofFill();
		ofSetColor(8+(backgroundColorR[1]*64.0), 10+(backgroundColorG[1]*64.0), 16+(backgroundColorB[1]*64.0), 240 * perspectiveOffset);
		ofRect(globals.window.height,0,globals.window.height,(2.0*(globals.window.width/6.0)/3.0)+(statusOffset*90.0));
		for(int i=0;i<roundf((globals.window.width/6.0)/3.0);i+=3){
			ofFill();
			ofSetColor(8+(backgroundColorR[1]*64.0), 10+(backgroundColorG[1]*64.0), 16+(backgroundColorB[1]*64.0),  perspectiveOffset*240*cos(2.0*(i/((globals.window.width/6.0)/3.0))));
			ofRect(globals.window.height,(2.0*(globals.window.width/6.0)/3.0)+i+(statusOffset*90.0), globals.window.height, 3);
		}
		glPopMatrix();
	}
	
	if(statusOffset > 0.0){
		glDisable(GL_DEPTH_TEST);
		ofEnableAlphaBlending();
		glPushMatrix();
		glTranslatef(0,0,10);
		ofFill();
		for(int i=0 ; i<100 ; i+=2){
			ofSetColor(12*(i/100.0), 12*(i/100.0), (12*(i/100.0))+4, statusOffset*((50.0-(i/2.0))+64.0));
			ofRect(0,i, globals.window.height*3.0,2.0);
		}
		
		ofRect(0,0,globals.window.width,100);
		
		ofSetColor(255, 255, 255, 255*statusOffset);

		float textOffset = globals.window.height-20;

		if(showStatusCams){

		textOffset = globals.window.height-((20+(((80*(4/3.0))+10)*3)));

		if(screen == 0){
		glPushMatrix();
		glTranslatef(10, 10, 1.0);
		glTranslatef(globals.window.height-(10+(((80*(4/3.0))+10)*3)), 0, 0);
		camera[0].grayDiff.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(10+(80*(4/3.0)), 0, 0);
		camera[0].grayImage.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(10+(80*(4/3.0)), 0, 0);
		camera[0].colorImg->draw(0,0,(80*(4/3.0)),80);
		glPopMatrix();
		}
		if(screen == 1){
		glPushMatrix();
		glTranslatef(10, 10, 1.0);
		glTranslatef((2*globals.window.height)-(10+(((80*(4/3.0))+10)*3)), 0, 0);
		camera[1].grayDiff.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(10+(80*(4/3.0)), 0, 0);
		camera[1].grayImage.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(10+(80*(4/3.0)), 0, 0);
		camera[1].colorImg->draw(0,0,(80*(4/3.0)),80);
		glPopMatrix();
		}
		if(screen == 2){
		glPushMatrix();
		glTranslatef(10, 10, 1.0);
		glTranslatef((3*globals.window.height)-(10+(((80*(4/3.0))+10)*3)), 0, 0);
		camera[2].grayDiff.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(10+(80*(4/3.0)), 0, 0);
		camera[2].grayImage.draw(0,0,(80*(4/3.0)),80);
		glTranslatef(10+(80*(4/3.0)), 0, 0);
		camera[2].colorImg->draw(0,0,(80*(4/3.0)),80);
		glPopMatrix();
		}
		}
			
		for(int i = 0 ; i < 3 ; i++) {
			if(screen == i){
			glPushMatrix();
			glTranslatef((i*globals.window.height)+10, 20, 1.0);
			glPushMatrix();
			glTranslatef(textOffset-(2*80*(4/3.0)), 0, 0);
			ofSetColor(255, 255, 255, 255*statusOffset);
			statusFontBold.drawString("FPS: " + ofToString(frameRate,2),0,0);
			glTranslatef(0, 13, 0);
			if(controlPanelOnline){
				ofSetColor(255, 255, 255, 255*statusOffset);
				statusFontBold.drawString("Online",0,0);
			} else {
				ofSetColor(255, 0, 0, 255*statusOffset*cos(ofGetElapsedTimef()*6.0));
				statusFontBold.drawString("Controlpanel not responding",0,0);
			}
			glTranslatef(0, 13, 0);
			ofSetColor(255, 255, 255, 63*statusOffset);
			statusFont.drawString(bulletStatus[3],0,0);
			glTranslatef(0, 13, 0);
			ofSetColor(255, 255, 255, 127*statusOffset);
			statusFont.drawString(bulletStatus[2],0,0);
			glTranslatef(0, 13, 0);
			ofSetColor(255, 255, 255, 191*statusOffset);
			statusFont.drawString(bulletStatus[1],0,0);
			glTranslatef(0, 13, 0);
			ofSetColor(255, 255, 255, 255*statusOffset);
			statusFontBold.drawString(bulletStatus[0],0,0);
			ofSetColor(255, 255, 255, 255*statusOffset);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(textOffset-(80*(4/3.0)), 0, 0);
			statusFontBold.drawString("Screen # "+ofToString(i+1),0,0);
			glTranslatef(0, 27, 0);
			statusFont.drawString("Bullet Bodies:",0,0);
			glTranslatef(0, 13, 0);
			statusFont.drawString("Text Lines:",0,0);
			glTranslatef(0, 13, 0);
			statusFont.drawString("Blobs:",0,0);
			glTranslatef(0, 13, 0);
			statusFont.drawString("Threshold:",0,0);
			glPopMatrix();
			ofSetColor(255, 255, 255, 255*statusOffset);
			glPushMatrix();
			glTranslatef(textOffset-statusFontBold.getCharSetWidth(ofToString(texts[i].getNumberBodies())), 27, 0);
			statusFontBold.drawString(ofToString(texts[i].getNumberBodies()),0,0);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(textOffset-statusFontBold.getCharSetWidth(ofToString(texts[i].getNumberLines())), 40, 0);
			statusFontBold.drawString(ofToString(texts[i].getNumberLines()),0,0);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(textOffset-statusFontBold.getCharSetWidth(ofToString(camera[i].contourFinder.nBlobs)), 53, 0);
			statusFontBold.drawString(ofToString(camera[i].contourFinder.nBlobs ),0,0);
			glPopMatrix();
			glPushMatrix();
			glTranslatef(textOffset-statusFontBold.getCharSetWidth(ofToString(camera[i].threshold)), 66, 0);
			statusFontBold.drawString(ofToString(camera[i].threshold ),0,0);
			glPopMatrix();
			glPopMatrix();
			}
		}
	glPopMatrix();
	}

	//Cameras
	
	// images 
	ofFill();
	if(moveCameraCorners){
		glPushMatrix();
		glTranslatef(0.0, 0.0, 4.0);
		ofSetColor(127,127,127);
		glTranslatef(0.0, (globals.window.width/3.0)-(globals.window.height*(camera[0].grayImage.height/(float)camera[0].grayImage.width)), 0.0);
		if(screen == 0)
		camera[0].grayImage.draw(0,0,globals.window.height,globals.window.height*(camera[0].grayImage.height/(float)camera[0].grayImage.width));
		glTranslatef(globals.window.height, 0.0, 0.0);
		if(screen == 1)
		camera[1].grayImage.draw(0,0,globals.window.height,globals.window.height*(camera[1].grayImage.height/(float)camera[1].grayImage.width));
		glTranslatef(globals.window.height, 0.0, 0.0);
		if(screen == 2)
		camera[2].grayImage.draw(0,0,globals.window.height,globals.window.height*(camera[2].grayImage.height/(float)camera[2].grayImage.width));
		glPopMatrix();
	}
	glPushMatrix();
	if(camDebug[0] || moveCameraCorners){
		ofFill();
		camera[0].draw(globals.window.height,globals.window.width/3.0);
	}
	glTranslatef(globals.window.height, 0.0, 0.0);
	if(camDebug[1] || moveCameraCorners){
		ofFill();
		camera[1].draw(globals.window.height,globals.window.width/3.0);
	}
	glTranslatef(globals.window.height, 0.0, 0.0);
	if(camDebug[2] || moveCameraCorners){
		ofFill();
		camera[2].draw(globals.window.height,globals.window.width/3.0);
	}
	glPopMatrix();
	
	
	// Blobs
	
	glPushMatrix();
	ofEnableAlphaBlending();
	glDisable(GL_DEPTH_TEST);
	for(int u=0;u<3;u++){
		if(blobColorA[u] > 0.0 && (screen == u || screen < 0)){
		ofSetColor(blobColorR[u]*255,blobColorG[u]*255,blobColorB[u]*255,blobColorA[u]*255);
		ofFill();
		if(camera[u].contourFinder.nBlobs > 0){
			/** using ofShapes
			ofBeginShape();
			for(int i=0;i<camera[u].simplify->numPoints;i++){
				ofCurveVertex((u*globals.window.height)+camera[u].simplify->points[i].x*globals.window.height, camera[u].simplify->points[i].y*globals.window.width/3.0);
			}
			ofEndShape(true);
			**/

			int numSteps = 33;
			
			float spacing = 1.0/numSteps;

			ofSetPolyMode(OF_POLY_WINDING_POSITIVE);
			
			ofBeginShape();
			for(float f=0; f<1; f+= spacing) {
				ofxVec2f v = camera[u].simplify->mySpline2D.sampleAt(f);
				ofCurveVertex((u*globals.window.height)+v.x*globals.window.height, v.y*globals.window.width/3.0);
			}
			ofxVec2f v = camera[u].simplify->mySpline2D.sampleAt(0.0);
			ofCurveVertex((u*globals.window.height)+v.x*globals.window.height, v.y*globals.window.width/3.0);
			ofEndShape(true);
			
		}
		}
	}
	glPopMatrix();
	
	
	if(makeSnaps){
		img.grabScreen(0,0,globals.window.width,globals.window.height);
		char fileName[255];
		sprintf(fileName, "snapshot_%0.3i.png", snapCounter);
		img.saveImage(fileName);
		sprintf(snapString, "saved %s", fileName);
		snapCounter++;
	}
	
	
	
/*	if(camera[0].contourFinder.nBlobs > 0){
		for(int i=0;i<camera[0].simplify->numPoints;i++){
			silhouette1Shape->addPoint(btVector3(camera[0].simplify->points[i].x*globals.window.height/100.0, camera[0].simplify->points[i].y*(globals.window.width/3.0)/100.0,+20000));
			silhouette1Shape->addPoint(btVector3(camera[0].simplify->points[i].x*globals.window.height/100.0, camera[0].simplify->points[i].y*(globals.window.width/3.0)/100.0,-20000));
			//cout<<camera[0].simplify->points[i].x*globals.window.height/100.0<< "  ,  "<<camera[0].simplify->points[i].y*(globals.window.width/3.0)/100.0<<endl;
		}
	}

*/
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	
	switch (key){
		case 'a':
			camera[0].simplify->damp = 0;
			camera[1].simplify->damp = 0;
			camera[2].simplify->damp = 0;
			break;
		case 'z':
			camera[0].simplify->damp = 0.09;
			camera[1].simplify->damp = 0.09;
			camera[2].simplify->damp = 0.09;
			break;
		case 'b':
			camera[0].bLearnBakground = true;
			camera[1].bLearnBakground = true;
			camera[2].bLearnBakground = true;
			break;
		case '+':
			camera[0].threshold ++;
			camera[1].threshold ++;
			camera[2].threshold ++;
			if (camera[0].threshold > 255) camera[0].threshold = 255;
			if (camera[1].threshold > 255) camera[1].threshold = 255;
			if (camera[2].threshold > 255) camera[2].threshold = 255;
			break;
		case '-':
			camera[0].threshold --;
			camera[1].threshold --;
			camera[2].threshold --;
			if (camera[0].threshold < 0) camera[0].threshold = 0;
			if (camera[1].threshold < 0) camera[1].threshold = 0;
			if (camera[2].threshold < 0) camera[2].threshold = 0;
			break;
		case 'v':
			ofShowCursor();
			vidTracker.videoSettings();
			ofHideCursor();
			break;
		case 'c':
			showStatusCams = !showStatusCams;
			break;
		case 'p':
			showPerspective = !showPerspective;
			break;
		case 's':
			showStatus = !showStatus;
			break;
		case 'r':
			rainEnable = !rainEnable;
			break;
		case 'd':
			rainTrigger = true;
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

#ifndef _TEST_APP
#define _TEST_APP

#define HISTORYSIZE 250
#define PICTURESIZE 1327104  //(768*576*3)

#define OF_ADDON_USING_OFXOSC
#define OF_ADDON_USING_OFXUTF8
#define OF_ADDON_USING_OFXBULLET
#define OF_ADDON_USING_OFXOPENCV
#define OF_ADDON_USING_OFSHADER
#define OF_ADDON_USING_OFX3DUTIL
#define OF_ADDON_USING_OFXVECTORMATH


//#include "ofShader.h"
#include "TextFontHolder.h"
#include "ofMain.h"
#include "ofAddons.h"
#include "sharedVariable.h"
#include "texts.h"
#include "camera.h"
#include "ofxBlobSimplify.h"
#include "globals.h"

//#include "of1394VideoGrabber.h"
#include <AGL/agl.h>

#define USE_TRIPLEHEAD 

class btBroadphaseInterface;
class btCollisionShape;
class btOverlappingPairCache;
class btCollisionDispatcher;
class btConstraintSolver;
class btDefaultCollisionConfiguration;

class testApp : public ofSimpleApp{
	
public:
	
	void setup();

	void bulletSetup();
	void textSetup();

	void update();
	void textUpdate();
	
	void draw();
	void drawViewport(int screen);

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
	
	btScalar	getDeltaTimeMicroseconds(){
		btScalar dt = clock.getTimeMicroseconds();
		clock.reset();
		return dt;
	}
	
	ofxOscReceiver	receiver;
	ofxOscSender sender;
	ofxOscSender isadora;
	
	vector<SharedVariable> sharedVariables;
	
	bool moveCameraCorners;
	bool moveProjectorCorners;
	int moveCameraCornersIndex;
	int moveProjectorCornersIndex;
	
	ofPoint cameraCorners[3][4];
	ofPoint projectorCorners[3][4];
	
	Camera camera[3];
	
	ofVideoGrabber	videoGrabber1;
	ofTexture		videoTexture1;
	ofxCvColorImage	videoCvImage1;
	unsigned char	videoHistory1[HISTORYSIZE][PICTURESIZE];
	int				videoHistory1index;
	int				videoHistory1showIndex;
	
	ofVideoGrabber	videoGrabber2;
	ofTexture		videoTexture2;
	ofxCvColorImage	videoCvImage2;
	unsigned char	videoHistory2[HISTORYSIZE][PICTURESIZE];
	int				videoHistory2index;
	int				videoHistory2showIndex;
	
	ofVideoGrabber	videoGrabber3;
	ofTexture		videoTexture3;
	ofxCvColorImage	videoCvImage3;
	unsigned char	videoHistory3[HISTORYSIZE][PICTURESIZE];
	int				videoHistory3index;
	int				videoHistory3showIndex;
	
	btDiscreteDynamicsWorld * dynamicsWorld;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btBroadphaseInterface* broadphase;
	btCollisionDispatcher* dispatcher;
	btConstraintSolver* solver;
	
	class	btThreadSupportInterface*		threadSupportCollision;
	class	btThreadSupportInterface*		threadSupportSolver;
		
	btRigidBody * groundRigidBody;
	btRigidBody * leftWallRigidBody;
	btRigidBody * rightWallRigidBody;
	btRigidBody * frontWallRigidBody;
	btRigidBody * backWallRigidBody;
	vector<btRigidBody*> bodies;
	
	btRigidBody *collider;
	
	btSoftBodyWorldInfo	m_softBodyWorldInfo;
	
	int millisForUpdate;
	
	int screensAliveTime;
	int screensAliveLastTime;
	int controlPanelOnlineTime;
	
	bool controlPanelOnline;

	btClock clock;
	
	btDefaultMotionState silhouette1MotionState;

	btDefaultMotionState silhouette2MotionState;
	
	btDefaultMotionState silhouette3MotionState;
	
	btDefaultMotionState fallMotionState;
	
	btVector3* btGravity;
	
	bool worldWrapX;
	bool worldWrapY;
	bool worldConstrainZ;
	bool worldGround;
	bool worldGroundState;
	bool worldWalls;
	bool worldWallsState;
	
	int cornerWarperIndex;
	
	bool showPerspective;
	bool showStatus;
	bool showStatusCams;

	ofxLight light1; 
	bool bSmoothLight;
	
	int					snapCounter;
	char 				snapString[255];
	ofImage 			img;
	bool				makeSnaps;
	ofPoint point;

	TextFontHolder fonts[3];
	Text texts[3];
	
	string	textStrings[3];

	typedef int textStates;
	enum {
		TEXT_STATE_NOOP = 0,
		TEXT_STATE_CLEAR = 1,
		TEXT_STATE_CLEARED = 2,
		TEXT_STATE_REFRESH = 3,
		TEXT_STATE_REFRESHED = 4,
		TEXT_STATE_PHYSICS_ENABLE = 5,
		TEXT_STATE_PHYSICS_ENABLED = 6,
		TEXT_STATE_PHYSICS_DISABLE = 7,
		TEXT_STATE_PHYSICS_DISABLED = 8,
		TEXT_STATE_RESET = 9,
	};
	
	int		textState[3];
	ofPoint textPosition[3];
	float	textWidth[3];
	float	textDepth[3];
	float	textFontSize[3];
	float	textColorR[3];
	float	textColorG[3];
	float	textColorB[3];
	float	textColorA[3];
	float	backgroundColorR[3];
	float	backgroundColorG[3];
	float	backgroundColorB[3];
	float	backgroundColorA[3];
	float	textFriction[3];
	float	textDamping[3];
	float	textMass[3];
	float	textRestitution[3];
	bool	textRefresh[3];
	bool	textAnimate[3];
	bool	wordBlocks[3];
	bool	textWalls[3];
	bool	textWallsState[3];
	bool	textScaffolding[3];
	
	bool	camEnable[3];
	bool	camRefresh[3];
	bool	camCaptureBackground[3];
	bool	camDebug[3];
	int		camThreshold[3];
	
	float	blobColorR[3];
	float	blobColorG[3];
	float	blobColorB[3];
	float	blobColorA[3];
	
	float blobDamping[3];
	float blobExpansion[3];
	
	float statusOffset;
	float perspectiveOffset;
	string	bulletStatus[4];
	ofImage testCard1;
	ofImage testCard2;
	ofImage testCard3;
	
	btRigidBody * textWall1LeftRigidBody;
	btRigidBody * textWall1RightRigidBody;
	btRigidBody * textWall2LeftRigidBody;
	btRigidBody * textWall2RightRigidBody;
	btRigidBody * textWall3LeftRigidBody;
	btRigidBody * textWall3RightRigidBody;
	
	TextFontHolder  statusFont;
	TextFontHolder  statusFontBold;
	
	float frameRate;

	bool rainEnable;
	bool rainTrigger;

	vector<Text> rainDrops;

	int rainDropCount;
	
	TextFontHolder rainFontSmall;
	TextFontHolder rainFontMedium;
	TextFontHolder rainFontBig;
	
	float yOffset;
	
	GLUtesselator *tobj;
	
	bool delayTripple;
	
	bool delayUpdate[3];
	float delayAlpha[3];
	float delayTime[3];

};

#endif


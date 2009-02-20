#ifndef _TEST_APP
#define _TEST_APP

#define HISTORYSIZE 750
#define PICTURESIZE 1327104

#define OF_ADDON_USING_OFXOSC
#define OF_ADDON_USING_OFXBULLET
#define OF_ADDON_USING_OFXOPENCV
#define OF_ADDON_USING_OFSHADER

//#include "ofShader.h"
#include "ofMain.h"
#include "ofAddons.h"
#include "sharedVariable.h"
#include "texts.h"
#include "camera.h"
#include "ofxBlobSimplify.h"
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
	void draw();
	void drawViewport();

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
	
	vector<SharedVariable> sharedVariables;
	
	bool moveCameraCorners;
	bool moveProjectorCorners;
	int moveCameraCornersIndex;
	int moveProjectorCornersIndex;
	
	ofPoint cameraCorners[3][4];
	ofPoint projectorCorners[3][4];
	
	Camera camera1;
	Camera camera2;
	Camera camera3;
		
	ofVideoGrabber vidTracker;
	ofTexture trackerTexture;
	ofxCvColorImage		colorImg;
	
	TextFontHolder * font1;
	Text* text1, *text2, *text3;
	
	btDiscreteDynamicsWorld * dynamicsWorld;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btBroadphaseInterface* broadphase;
	btCollisionDispatcher* dispatcher;
	btConstraintSolver* solver;
	
	class	btThreadSupportInterface*		threadSupportCollision;
	class	btThreadSupportInterface*		threadSupportSolver;
		
	btRigidBody * groundRigidBody;	
	vector<btRigidBody*> bodies;
	
	btRigidBody *collider;
	btRigidBody *silhouette1;
	btRigidBody *silhouette2;
	btRigidBody *silhouette3;
	
	btSoftBodyWorldInfo	m_softBodyWorldInfo;
	
	int millisForUpdate;
	btClock clock;
	
	btDefaultMotionState* silhouette1MotionState;
	btConvexHullShape * silhouette1Shape;
	btVector3* silhouette1Points;
	int numSilhouette1Points;

	btDefaultMotionState* silhouette2MotionState;
	btConvexHullShape * silhouette2Shape;
	btVector3* silhouette2Points;
	int numSilhouette2Points;
	
	btDefaultMotionState* silhouette3MotionState;
	btConvexHullShape * silhouette3Shape;
	btVector3* silhouette3Points;
	int numSilhouette3Points;
	
	btDefaultMotionState* fallMotionState;
	
	btVector3* btGravity;
	
	int cornerWarperIndex;
	
	bool debug;

	ofxLight light1; 
	bool bSmoothLight;
	
	int					snapCounter;
	char 				snapString[255];
	ofImage 			img;
	bool				makeSnaps;
	ofPoint point;

	string	textStrings[3];

	typedef int textStates;
	enum {
		TEXT_STATE_CLEAR = 0,
		TEXT_STATE_UPDATE = 1,
		TEXT_STATE_PHYSICS_ENABLED = 2,
		TEXT_STATE_PHYSICS_DISABLED = 3,
		TEXT_STATE_RESET = 4
	};
	
	int		textState[3];
	ofPoint textPosition[3];
	float	textWidth[3];
	float	textDepth[3];
	float	textColorR[3];
	float	textColorG[3];
	float	textColorB[3];
	float	textColorA[3];

};

#endif


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
#include "of1394VideoGrabber.h"

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
	void update();
	void draw();
	void drawViewport();
	
	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased();
	void bulletSetup();
	
	btScalar	getDeltaTimeMicroseconds(){
		btScalar dt = clock.getTimeMicroseconds();
		clock.reset();
		return dt;
	}
	
	ofxOscReceiver	receiver;
	ofxOscSender sender;
	
	vector<SharedVariable> sharedVariables;
	
	float fade;
	
	/*		ofVideoGrabber vidGrabber;
	 unsigned char videoHistory[HISTORYSIZE][PICTURESIZE];
	 int historyIndex;		
	 ofTexture videoTexture;
	 int showIndex;*/
	
	//Video tracker
	ofVideoGrabber vidTracker;
	//of1394VideoGrabber 	vidGrabber;
	
	ofTexture trackerTexture;
	
	ofxCvColorImage		colorImg;
	
	TextFontHolder * font1;
	Text* text, *text2, *text3;
	
	btDiscreteDynamicsWorld * dynamicsWorld;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btBroadphaseInterface* broadphase;
	btCollisionDispatcher* dispatcher;
	btConstraintSolver* solver;
	
	class	btThreadSupportInterface*		threadSupportCollision;
	class	btThreadSupportInterface*		threadSupportSolver;
	
	
	btRigidBody * groundRigidBody;	
	//btRigidBody **bodies, **bodies2, **bodies3;
	vector<btRigidBody*> bodies;
	
	btRigidBody *collider;
	btRigidBody *silhouette;
	
	btSoftBodyWorldInfo	m_softBodyWorldInfo;
	
	int millisForUpdate;
	btClock clock;
	
	int					snapCounter;
	char 				snapString[255];
	ofImage 			img;
	bool				makeSnaps;
	
	ofxLight light1; 
	bool bSmoothLight;
	
	ofPoint cameraCorners[3][4];
	ofPoint projectorCorners[3][4];
	
	Camera camera1;
	ofPoint point;
	
	btVector3* silhouettePoints;
	int numSilhouettePoints;
	
	int cornerWarperIndex;
	
	bool debug;
	
	
};

#endif


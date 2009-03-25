#ifndef _OFX_BLOBLSIMPLIFY
#define _OFX_BLOBLSIMPLIFY

#include "ofMain.h"
#define OF_ADDON_USING_OFXVECTORMATH
#define OF_ADDON_USING_OFXOPENCV
#define OF_ADDON_USING_OFXMSASPLINE

#include "ofAddons.h"


class ofxBlobSimplify{
public:
	vector<ofxPoint2f> points;
	ofxCvBlob* blob;
	int numPoints;
	vector<ofxVec2f> pointForces;
	vector<ofxVec2f> pointVel;
	float damp;
	bool hasBlob;
	ofxBlobSimplify(int num, int w, int h, float damping);
	ofxMSASpline2D	mySpline2D;
	
	void update(vector <ofxCvBlob>* blobs);
	void draw(int x, int y);
	
	bool pointInPolygonTest(ofxPoint2f point, vector <ofxPoint2f>* polygon, int polygonSiz);
};

#endif
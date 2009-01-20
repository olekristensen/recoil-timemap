#ifndef RECOIL_CAMERA
#define RECOIL_CAMERA
#define OF_ADDON_USING_OFXOPENCV

#include "ofMain.h"
#include "ofAddons.h"
#include "coordWarp.h"

class Camera{
public:
	ofVideoGrabber *	grabber;
	ofTexture *			texture;	
	ofxCvColorImage *	colorImg;
	
	ofxCvContourFinder	contourFinder;	
	ofxCvColorImage		crop;	
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBg;
	ofxCvGrayscaleImage	grayDiff;
	
	int	threshold;
	bool 	bLearnBakground;
	
	ofPoint pos;
	ofPoint size;
	
	ofxPoint2f dst[4], src[4];
	coordWarping coordwarp;
	
	Camera(){}
	
	void setup(ofVideoGrabber * _grabber, ofTexture * _texture, ofxCvColorImage * _colorImg, int x, int y, double w, double h){
		pos.x = x; 
		pos.y = y;
		size.x = w;
		size.y = h;
		grabber = _grabber;
		texture = _texture;
		colorImg = _colorImg;

		crop.allocate(w,h);
		grayImage.allocate(w,h);
		grayBg.allocate(w,h);
		grayDiff.allocate(w,h);
		
		threshold = 50;
		bLearnBakground = true;
		

		src[0] = ofxPoint2f(0,0);
		src[1] = ofxPoint2f(1,0);
		src[2] = ofxPoint2f(1,1);
		src[3] = ofxPoint2f(0,1);	
		
		dst[0] = ofxPoint2f(0,0);
		dst[1] = ofxPoint2f(1,0);
		dst[2] = ofxPoint2f(1,1);
		dst[3] = ofxPoint2f(0,1);		
		coordwarp.calculateMatrix(src, dst);
	}
	
	void updateWarp(ofxPoint2f  dstIn[4]){
		dst[0] = dstIn[0];
		dst[1] = dstIn[1];		
		dst[2] = dstIn[2];
		dst[3] = dstIn[3];
		coordwarp.calculateMatrix(src, dst);	
	}
	
	ofxPoint2f getDst(int i){
		return dst[i];
	}
	
	void update(){
		CvMat* c = cvCreateMat(size.x,size.y, CV_8UC1);
		CvMat* sRect = cvGetSubRect(colorImg->getCvImage(),c, cvRect(pos.x, pos.y, size.x, size.y) );
		cvCopy(sRect, crop.getCvImage());
		grayImage = crop;
		
		if (bLearnBakground == true){
			grayBg = grayImage;
			bLearnBakground = false;
		}

		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);		
		contourFinder.findContours(grayDiff, 2, (size.x*size.y)/3, 10, true);	// find holes
	}
	
	
	
	void draw(int w, int h){
		glPushMatrix();
		glDisable(GL_DEPTH_TEST);
		ofSetColor(255, 255, 255);
		ofLine(dst[0].x*w, dst[0].y*h, dst[1].x*w, dst[1].y*h);
		ofLine(dst[2].x*w, dst[2].y*h, dst[1].x*w, dst[1].y*h);
		ofLine(dst[2].x*w, dst[2].y*h, dst[3].x*w, dst[3].y*h);
		ofLine(dst[0].x*w, dst[0].y*h, dst[3].x*w, dst[3].y*h);
		
		grayImage.draw(0,0);
		glTranslatef(-size.x, -size.y, 0);
		for (int i = 0; i < contourFinder.nBlobs; i++){
			ofSetColor(255, 0, 0);
			contourFinder.blobs[i].draw(size.x, size.y);
		}

		glEnable(GL_DEPTH_TEST);
		glPopMatrix();	
		//glPopMatrix();	

	}
	
	
};

#endif
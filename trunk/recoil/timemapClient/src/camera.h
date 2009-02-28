#ifndef RECOIL_CAMERA
#define RECOIL_CAMERA
#define OF_ADDON_USING_OFXOPENCV

#include "ofMain.h"
#include "ofAddons.h"
#include "coordWarp.h"
#include "ofxBlobSimplify.h"

class Camera{
public:
	ofVideoGrabber *	grabber;
	ofTexture *			texture;	
	ofxCvColorImage *	colorImg;
	
	ofxCvContourFinder	contourFinder;	
	ofxBlobSimplify * simplify;
	
	ofxCvColorImage		crop;	
	ofxCvGrayscaleImage grayImage;
	ofxCvGrayscaleImage grayBg;
	ofxCvGrayscaleImage	grayDiff;
	CvMat* c;

	
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
		c = cvCreateMat(size.x,size.y, CV_8UC1);
		
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
		simplify = new ofxBlobSimplify(20, 1, 1, 0.45);
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
		if(contourFinder.nBlobs > 0){
			for(int j=0;j<contourFinder.nBlobs;j++){
				for(int i=0;i<contourFinder.blobs[j].nPts;i++){
					contourFinder.blobs[j].pts[i] = coordwarp.transform(contourFinder.blobs[j].pts[i].x/ (float)size.x,contourFinder.blobs[j].pts[i].y/ (float)size.y);
				}
			}
			simplify->update(&contourFinder.blobs);
		} else {
			simplify->hasBlob = false;
		}
		//delete sRect;
	}
	
	
	
	void draw(int w, int h){
		glPushMatrix();
		glDisable(GL_DEPTH_TEST);
		ofSetColor(255, 255, 255);
		grayImage.draw(0,0,w,h);
		/**
		 ofLine(dst[0].x*w, dst[0].y*h, dst[1].x*w, dst[1].y*h);
		 ofLine(dst[2].x*w, dst[2].y*h, dst[1].x*w, dst[1].y*h);
		 ofLine(dst[2].x*w, dst[2].y*h, dst[3].x*w, dst[3].y*h);
		 ofLine(dst[0].x*w, dst[0].y*h, dst[3].x*w, dst[3].y*h);
		 **/		
		//	glTranslatef(-size.x, -size.y, 0);
		//	for (int i = 0; i < contourFinder.nBlobs; i++){
		//		ofSetColor(255, 0, 0);
		//	contourFinder.blobs[i].draw(size.x, size.y);
		
		//	}
		glPushMatrix();
		
		/* ---------------------------- draw the bounding rectangle
		 ofSetColor(0xDD00CC);
		 glPushMatrix();
		 glTranslatef( x, y, 0.0 );
		 
		 ofNoFill();
		 for( int i=0; i<blobs.size(); i++ ) {
		 ofRect( blobs[i].boundingRect.x, blobs[i].boundingRect.y,
		 blobs[i].boundingRect.width, blobs[i].boundingRect.height );
		 }
		 */
		if(simplify->hasBlob){
			
			ofSetColor(128,128, 128);
			glBegin(GL_POLYGON);	
			for(int i=0;i<simplify->points.size();i++){
				glVertex3f(simplify->points[i].x*w, simplify->points[i].y*h,0);
			}
			glEnd();
			/*/// ---------------------------- draw the blobs
			ofSetColor(0x00FFFF);
			
			for( int i=0; i<contourFinder.nBlobs; i++ ) {
				
				ofNoFill();
				ofBeginShape();
				for( int j=0; j<contourFinder.blobs[i].nPts; j++ ) {
					ofxPoint2f p = ofxPoint2f(contourFinder.blobs[i].pts[j].x*w , contourFinder.blobs[i].pts[j].y *h);
					ofVertex( p.x , p.y );
					//	cout<<"oaisjdoiasjd"<<endl;
				}
				ofEndShape();
				
				
				
				for(int j=0;j<contourFinder.blobs[i].nPts;j++){
					//if(!pointInPolygonTest(ofxPoint2f(blob->pts[i].x,blob->pts[i].y), &points, points.size())){
					
					if(!simplify->pointInPolygonTest(ofxPoint2f(contourFinder.blobs[i].pts[j].x,contourFinder.blobs[i].pts[j].y), &simplify->points, simplify->points.size())){
						ofxVec2f Co;
						ofxVec2f A1o,ao;
						bool dragPoint = false;
						for(int u=0;u<simplify->points.size();u++){
							
							ofxVec2f a = ofxVec2f(simplify->points[u].x,simplify->points[u].y);
							ofxVec2f b = ofxVec2f(simplify->points[(u+1)%simplify->points.size()].x, simplify->points[(u+1)%simplify->points.size()].y);
							ofxVec2f c = ofxVec2f(contourFinder.blobs[i].pts[j].x,contourFinder.blobs[i].pts[j].y);
							ofxVec2f A = c - a;
							ofxVec2f B = b - a;
							ofxVec2f A1 = ((A.dot(B))/(B.length()*B.length())) * B;
							ofxVec2f C = c - (a + A1);
							
							if(((A1+a).x< a.x && (A1+a).x > b.x) || ((A1+a).x > a.x && (A1+a).x < b.x)){
								if(((A1+a).y< a.y && (A1+a).y > b.y) || ((A1+a).y > a.y && (A1+a).y < b.y)){
									if(Co.length() == 0 || Co.length() > C.length()){
										Co = C;
										A1o = A1;
										ao = a;
										dragPoint = false;
									}
								}
								
								//					break;
							}
							if(Co.length() == 0 ||(c-a).length() < Co.length()){
								Co = (c-a);
								dragPoint = true;
								ao = a;
								A1o = ofxVec2f();
							}
							
						}
						if(Co.length() != 0){
							
							ofSetColor(0, 255, 255);
							//ofLine(a.x,a.y, a.x+A1o.x,a.y+A1o.y);
							
							ofSetColor(255, 255, 0);
							ofLine(contourFinder.blobs[i].pts[j].x*w,contourFinder.blobs[i].pts[j].y*h, (contourFinder.blobs[i].pts[j].x-Co.x)*w,(contourFinder.blobs[i].pts[j].y-Co.y)*h);
							ofSetColor(255, 0, 255);
							ofLine(contourFinder.blobs[i].pts[j].x*w,contourFinder.blobs[i].pts[j].y*h, ao.x*w,ao.y*h);
							
						}
					}
					
					
					//}
					
				}
			}
			 //*/
		}
		

		//if(contourFinder.nBlobs > 0)
		//		simplify->draw(0,0);
		
		glPopMatrix();
		glEnable(GL_DEPTH_TEST);
		glPopMatrix();	
		//glPopMatrix();	
		
	}
	
	
};

#endif
#ifndef RECOIL_CAMERA
#define RECOIL_CAMERA
#define OF_ADDON_USING_OFXOPENCV
#define OF_ADDON_USING_OFXVECTORMATH

#include "ofMain.h"
#include "ofAddons.h"
#include "coordWarp.h"
#include "ofxBlobSimplify.h"
#include "cv.h" 
#include "cvaux.h" 

class Camera{
public:
	ofVideoGrabber *	grabber;
	ofTexture *			texture;	
	ofxCvColorImage *	colorImg;
	
	ofxCvContourFinder	contourFinder;	
	ofxBlobSimplify * simplify;
	
	/**
	CvBGStatModel* gauss_bgModel; 
	ofxCvGrayscaleImage      gauss_foregroundImg; 
	ofxCvColorImage      gauss_backgroundImg; 
    
	CvBGStatModel* fgd_bgModel; 
	ofxCvGrayscaleImage      fgd_foregroundImg; 
	ofxCvColorImage      fgd_backgroundImg; 
	**/
	
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
		
		/**
		gauss_foregroundImg.allocate(w,h); 
		gauss_backgroundImg.allocate(w,h); 
		
		fgd_foregroundImg.allocate(w,h); 
		fgd_backgroundImg.allocate(w,h); 
		
		CvGaussBGStatModelParams* params = new CvGaussBGStatModelParams;                   
		params->win_size=2;    
		params->n_gauss=5; 
		params->bg_threshold=0.7; 
		params->std_threshold=3.5; 
		params->minArea=15; 
		params->weight_init=0.05; 
		params->variance_init=30; 
		
		//bgModel = cvCreateGaussianBGModel(colorImg.getCvImage() ,params); 
		gauss_bgModel = cvCreateGaussianBGModel(crop.getCvImage()); 
		fgd_bgModel = cvCreateFGDStatModel(crop.getCvImage());
		**/
		
		c = cvCreateMat(size.x,size.y, CV_8UC1);
		
		threshold = 100;
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
		simplify = new ofxBlobSimplify(80, 1, 1, 0.35);
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
	
	void update(float blobExpansion = 0.0){
		CvMat* sRect = cvGetSubRect(colorImg->getCvImage(),c, cvRect(pos.x, pos.y, size.x, size.y) );
		cvCopy(sRect, crop.getCvImage());
		
		cvRectangle( crop.getCvImage(), cvPoint(0,0), cvPoint(size.x,size.y), cvScalar(0.5),
						 10);
		
		/**
		if(bLearnBakground){
			gauss_bgModel = cvCreateGaussianBGModel(crop.getCvImage()); 
			fgd_bgModel = cvCreateFGDStatModel(crop.getCvImage());
		}
		
		cvUpdateBGStatModel(crop.getCvImage() ,gauss_bgModel);
		cvCopy(gauss_bgModel->foreground,gauss_foregroundImg.getCvImage()); 
		cvCopy(gauss_bgModel->background,gauss_backgroundImg.getCvImage()); 
		
		
		cvUpdateBGStatModel(crop.getCvImage() ,fgd_bgModel); 
		cvCopy(fgd_bgModel->foreground,fgd_foregroundImg.getCvImage()); 
		cvCopy(fgd_bgModel->background,fgd_backgroundImg.getCvImage()); 
		**/
		
		crop.blurGaussian(5);
		
		grayImage = crop;

		 if (bLearnBakground == true){
			grayBg = grayImage;
			bLearnBakground = false;
		}
		
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);
		contourFinder.findContours(grayDiff, 2, (size.x*size.y)/3, 10, false);	// do not find holes
		ofPoint centroidAvg = ofPoint(0.0,0.0);
		
		if(contourFinder.nBlobs > 0){
			for(int j=0;j<contourFinder.nBlobs;j++){
				for(int i=0;i<contourFinder.blobs[j].nPts;i++){
					contourFinder.blobs[j].pts[i] = coordwarp.transform(contourFinder.blobs[j].pts[i].x/ (float)size.x,contourFinder.blobs[j].pts[i].y/ (float)size.y);
				}
				centroidAvg.x += contourFinder.blobs[j].centroid.x;
				centroidAvg.y += contourFinder.blobs[j].centroid.y;
				
			}
			centroidAvg.x = centroidAvg.x / (float)contourFinder.nBlobs;
			centroidAvg.y = centroidAvg.y / (float)contourFinder.nBlobs;
			centroidAvg = coordwarp.transform(centroidAvg.x/ (float)size.x,centroidAvg.y/ (float)size.y);
			if(blobExpansion > 0.0){
				//top left corner
				float cornerX = centroidAvg.x*(1.0-blobExpansion);
				float cornerY = centroidAvg.y*(1.0-blobExpansion);
				//make points
				ofPoint topLeft1 = ofPoint(cornerX,cornerY);
				ofPoint topLeft2 = ofPoint(cornerX+0.01,cornerY);
				ofPoint topLeft3 = ofPoint(cornerX+0.01,cornerY+0.01);
				ofPoint topLeft4 = ofPoint(cornerX,cornerY+0.01);
				//make blob
				ofxCvBlob topLeftBlob = ofxCvBlob();
				topLeftBlob.pts.push_back(topLeft1);
				topLeftBlob.pts.push_back(topLeft2);
				topLeftBlob.pts.push_back(topLeft3);
				topLeftBlob.pts.push_back(topLeft4);
				topLeftBlob.area = 0.01*0.01;
				topLeftBlob.length = 0.4;
				topLeftBlob.boundingRect = ofRectangle(topLeft1.x,topLeft1.y,0.01,0.01);
				topLeftBlob.centroid = ofPoint(cornerX,cornerY);
				topLeftBlob.nPts = 4;
				//add blob
				contourFinder.blobs.push_back(topLeftBlob);
				contourFinder.nBlobs++;
				
				//top right corner
				cornerX = centroidAvg.x + ((1.0-centroidAvg.x)*blobExpansion);
				cornerY = centroidAvg.y*(1.0-blobExpansion);
				//make points
				ofPoint topRight1 = ofPoint(cornerX-0.01,cornerY);
				ofPoint topRight2 = ofPoint(cornerX,cornerY);
				ofPoint topRight3 = ofPoint(cornerX,cornerY+0.01);
				ofPoint topRight4 = ofPoint(cornerX-0.01,cornerY+0.01);
				//make blob
				ofxCvBlob topRightBlob = ofxCvBlob();
				topRightBlob.pts.push_back(topRight1);
				topRightBlob.pts.push_back(topRight2);
				topRightBlob.pts.push_back(topRight3);
				topRightBlob.pts.push_back(topRight4);
				topRightBlob.area = 0.01*0.01;
				topRightBlob.length = 0.4;
				topRightBlob.boundingRect = ofRectangle(topRight1.x,topRight1.y,0.01,0.01);
				topRightBlob.centroid = ofPoint(cornerX,cornerY);
				topRightBlob.nPts = 4;
				//add blob
				contourFinder.blobs.push_back(topRightBlob);
				contourFinder.nBlobs++;
				
				//bottom left corner
				cornerX = centroidAvg.x*(1.0-blobExpansion);
				cornerY = centroidAvg.y + ((1.0-centroidAvg.y)*blobExpansion);
				//make points
				ofPoint bottomLeft1 = ofPoint(cornerX,cornerY-0.01);
				ofPoint bottomLeft2 = ofPoint(cornerX+0.01,cornerY-0.01);
				ofPoint bottomLeft3 = ofPoint(cornerX+0.01,cornerY);
				ofPoint bottomLeft4 = ofPoint(cornerX,cornerY);
				//make blob
				ofxCvBlob bottomLeftBlob = ofxCvBlob();
				bottomLeftBlob.pts.push_back(bottomLeft1);
				bottomLeftBlob.pts.push_back(bottomLeft2);
				bottomLeftBlob.pts.push_back(bottomLeft3);
				bottomLeftBlob.pts.push_back(bottomLeft4);
				bottomLeftBlob.area = 0.01*0.01;
				bottomLeftBlob.length = 0.4;
				bottomLeftBlob.boundingRect = ofRectangle(bottomLeft1.x,bottomLeft1.y,0.01,0.01);
				bottomLeftBlob.centroid = ofPoint(cornerX,cornerY);
				bottomLeftBlob.nPts = 4;
				//add blob
				contourFinder.blobs.push_back(bottomLeftBlob);
				contourFinder.nBlobs++;
				
				//bottom right corner
				cornerX = centroidAvg.x + ((1.0-centroidAvg.x)*blobExpansion);
				cornerY = centroidAvg.y + ((1.0-centroidAvg.y)*blobExpansion);
				//make points
				ofPoint bottomRight1 = ofPoint(cornerX-0.01,cornerY-0.01);
				ofPoint bottomRight2 = ofPoint(cornerX,cornerY-0.01);
				ofPoint bottomRight3 = ofPoint(cornerX,cornerY);
				ofPoint bottomRight4 = ofPoint(cornerX-0.01,cornerY);
				//make blob
				ofxCvBlob bottomRightBlob = ofxCvBlob();
				bottomRightBlob.pts.push_back(bottomRight1);
				bottomRightBlob.pts.push_back(bottomRight2);
				bottomRightBlob.pts.push_back(bottomRight3);
				bottomRightBlob.pts.push_back(bottomRight4);
				bottomRightBlob.area = 0.01*0.01;
				bottomRightBlob.length = 0.4;
				bottomRightBlob.boundingRect = ofRectangle(bottomRight1.x,bottomRight1.y,0.01,0.01);
				bottomRightBlob.centroid = ofPoint(cornerX,cornerY);
				bottomRightBlob.nPts = 4;
				//add blob
				contourFinder.blobs.push_back(bottomRightBlob);
				contourFinder.nBlobs++;
				
				//big square
				cornerX = centroidAvg.x*(1.0-(blobExpansion*blobExpansion));
				cornerY = centroidAvg.y*(1.0-(blobExpansion*blobExpansion));
				//make points
				ofPoint center1 = ofPoint(cornerX,cornerY);
				
				cornerX = centroidAvg.x + ((1.0-centroidAvg.x)*blobExpansion*blobExpansion);
				cornerY = centroidAvg.y*(1.0-(blobExpansion*blobExpansion));
				ofPoint center2 = ofPoint(cornerX,cornerY);

				cornerX = centroidAvg.x*(1.0-(blobExpansion*blobExpansion));
				cornerY = centroidAvg.y + ((1.0-centroidAvg.y)*blobExpansion*blobExpansion);
				ofPoint center3 = ofPoint(cornerX,cornerY);
				
				cornerX = centroidAvg.x + ((1.0-centroidAvg.x)*blobExpansion*blobExpansion);
				cornerY = centroidAvg.y + ((1.0-centroidAvg.y)*blobExpansion*blobExpansion);
				ofPoint center4 = ofPoint(cornerX,cornerY);
				
				//make blob
				ofxCvBlob centerBlob = ofxCvBlob();
				centerBlob.pts.push_back(center1);
				centerBlob.pts.push_back(center2);
				centerBlob.pts.push_back(center3);
				centerBlob.pts.push_back(center4);
				centerBlob.area = 0.01*0.01;
				centerBlob.length = 0.4;
				centerBlob.boundingRect = ofRectangle(center1.x,center1.y,center4.x,center4.y);
				centerBlob.centroid = ofPoint(cornerX,cornerY);
				centerBlob.nPts = 4;
				//add blob
				contourFinder.blobs.push_back(centerBlob);
				contourFinder.nBlobs++;
				
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
		//grayImage.draw(0,0,w,h);
		/**
		 ofLine(dst[0].x*w, dst[0].y*h, dst[1].x*w, dst[1].y*h);
		 ofLine(dst[2].x*w, dst[2].y*h, dst[1].x*w, dst[1].y*h);
		 ofLine(dst[2].x*w, dst[2].y*h, dst[3].x*w, dst[3].y*h);
		 ofLine(dst[0].x*w, dst[0].y*h, dst[3].x*w, dst[3].y*h);
		// **/
		
		ofSetColor(255, 255, 64, 64);
		ofBeginShape();
		for(int u=0;u<4;u++){
			ofVertex(dst[u].x*w, dst[u].y*h);
		}
		ofEndShape(true);
		
		
		 //**/		
		//	glTranslatef(-size.x, -size.y, 0);
		//	for (int i = 0; i < contourFinder.nBlobs; i++){
		//		ofSetColor(255, 0, 0);
		//	contourFinder.blobs[i].draw(size.x, size.y);
		
		//	}
		
		
		/**
		glPushMatrix();
		glTranslatef(0, 0, 0);
		ofSetColor(0xffffff); 
	
		gauss_foregroundImg.draw(0,0, 200,150); 
		gauss_backgroundImg.draw(0,170, 200,150); 
		
		fgd_foregroundImg.draw(220,0, 200,150); 
		fgd_backgroundImg.draw(220,170, 200,150); 
		
		glPopMatrix();
		**/
		
		glPushMatrix();
		
		/** ---------------------------- draw the bounding rectangle
		 ofSetColor(0xDD00CC);
		 glPushMatrix();
		 glTranslatef( pos.x, pos.y, 0.0 );
		 
		 ofNoFill();
		 for( int i=0; i<blobs.size(); i++ ) {
		 ofRect( blobs[i].boundingRect.x, blobs[i].boundingRect.y,
		 blobs[i].boundingRect.width, blobs[i].boundingRect.height );
		 }
		 //**/
		if(simplify->hasBlob){
			
			ofSetColor(0,0, 0, 64);
			glBegin(GL_POLYGON);
			for(int i=0;i<simplify->points.size();i++){
				glVertex3f(simplify->points[i].x*w, simplify->points[i].y*h,0);
			}
			glEnd();
			/**/// ---------------------------- draw the blobs
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
			 //**/
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
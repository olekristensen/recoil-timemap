
#include "ofxBlobSimplify.h"

ofxBlobSimplify::ofxBlobSimplify(int num, int w, int h, float damping = 0.2){
	numPoints = num;
	int o = w*2+h*2;
	damp = damping;
	for(int i=0;i<num;i++){
		int v = (o/(float)num)*i;
		if(v < w)
			points.push_back(ofxPoint2f(0,0));
		else if(v <w+h){
			points.push_back(ofxPoint2f(w,0));
		} else if(v < w+h+w){
			points.push_back(ofxPoint2f(w,h));	
		} else {
			points.push_back(ofxPoint2f(0,h));	
		}
		pointVel.push_back(ofxVec2f());
		pointForces.push_back(ofxVec2f());
	}
	hasBlob = false;
}
void ofxBlobSimplify::update(vector <ofxCvBlob>* blobs){
	int num = 1;
	if(!hasBlob){
		hasBlob = true;
		num = 150;
	}
	for(int n=0;n<num;n++){
		for(int i=0;i<points.size();i++){
			pointVel[i] *= 0.2;
			pointForces[i] *= 0.0;
		}	
		
		
		for(int i=0;i<points.size();i++){
			pointForces[i] += (points[(i+1)%points.size()] - points[i])*damp;
			pointForces[(i+1)%points.size()] += (points[i] - points[(i+1)%points.size()])*damp;
		}
		
		
		for(int i=0;i<points.size();i++){
			pointVel[i] += pointForces[i];
			points[i] += pointVel[i];
		}
		
		for(int i=0;i<blobs->size();i++){
			blob = &blobs->at(i);
			
			for(int i=0;i<blob->nPts;i++){
				ofxVec2f Co;
				ofxVec2f A1o, Bo, ao;
				bool dragPoint = false;
				
				int Uo;
				if(!pointInPolygonTest(ofxPoint2f(blob->pts[i].x,blob->pts[i].y), &points, points.size())){
					
					for(int u=0;u<points.size();u++){
						
						ofxVec2f a = ofxVec2f(points[u].x,points[u].y);
						ofxVec2f b = ofxVec2f(points[(u+1)%points.size()].x, points[(u+1)%points.size()].y);
						ofxVec2f c = ofxVec2f(blob->pts[i].x,blob->pts[i].y);
						ofxVec2f A = c - a;
						ofxVec2f B = b - a;
						ofxVec2f A1 = ((A.dot(B))/(B.length()*B.length())) * B;
						ofxVec2f C = c - (a + A1);
						
						if(((A1+a).x< a.x && (A1+a).x > b.x) || ((A1+a).x > a.x && (A1+a).x < b.x)){
							if(((A1+a).y< a.y && (A1+a).y > b.y) || ((A1+a).y > a.y && (A1+a).y < b.y)){
								if(Co.length() == 0 || Co.length() > C.length()){
									Co = C;
									Bo = B;
									A1o = A1;
									Uo = u;
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
							Uo = u;
							A1o = ofxVec2f();
						}
					}
					//pointVel[Uo] *= 0.0;
					//	pointVel[(Uo+1)%points.size()] *= 0.0;
					if(damp != 0){
						if(!dragPoint) {
							points[(Uo+1)%points.size()] +=1* Co*2.0* A1o.length()/(Bo.length());
							points[Uo] +=1*Co*2.0* (Bo.length()-A1o.length())/(Bo.length());
						} else {
							points[Uo] +=Co*1.0;	
						}
					}
				}
			}
			
		}
		
		for(int i=0;i<points.size();i++){
			ofxVec2f a = ofxVec2f(points[i].x,points[i].y);
			ofxVec2f b = ofxVec2f(points[(i+1)%points.size()].x, points[(i+1)%points.size()].y);
			ofxVec2f c = ofxVec2f(points[(i-1)%points.size()].x, points[(i-1)%points.size()].y);
			ofxVec2f A = a - c;
			ofxVec2f B = b - a;
			
			float angle = A.angle(B);

		}
		
	}
	
	
	//	points.clear();
	for(int i=0;i<blob->nPts;i++){
		//		points.push_back(ofxPoint2f(blob->pts[i].x, blob->pts[i].y));
	}
}

void ofxBlobSimplify::draw(int x=0, int y=0){
	ofSetColor(255, 0, 255);
	//	glPushMatrix();
	//glTranslated(x, y, 0);
	glBegin(GL_LINE_LOOP);	
	for(int i=0;i<points.size();i++){
		glVertex3f(points[i].x, points[i].y,0);
	}
	glEnd();
	
	
	//glPopMatrix();
}

bool ofxBlobSimplify::pointInPolygonTest(ofxPoint2f point, vector <ofxPoint2f>* polygon, int polygonSize){
	int crossings = 0;
	float x1, x2;
	for ( int i = 0; i < polygonSize; i++ ){
		if ( polygon->at(i).x < polygon->at( (i+1)%polygonSize ).x ){
			x1 = polygon->at(i).x;
			x2 = polygon->at((i+1)%polygonSize).x ;
		} else {
			x1 = polygon->at((i+1)%polygonSize).x ;
			x2 = polygon->at(i).x;
		}
		
		if ( point.x > x1 && point.x <= x2 && ( point.y < polygon->at(i).y || point.y <= polygon->at((i+1)%polygonSize).y ) ) {
			static const float eps = 0.000000001;
			
			float dx = polygon->at((i+1)%polygonSize).x - polygon->at(i).x;
			float dy = polygon->at((i+1)%polygonSize).y - polygon->at(i).y;
			float k;
			
			if ( fabs(dx) < eps ){
				k = INFINITY;	// math.h
			} else {
				k = dy/dx;
			}
			
			float m = polygon->at(i).y - k * polygon->at(i).x;
			
			float y2 = k * point.x + m;
			if ( point.y <= y2 ){
				crossings++;
			}
		}
	}
	
	if ( crossings % 2 == 1 ){
		return true;
	}
	return false;
}
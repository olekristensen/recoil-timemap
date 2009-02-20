#ifndef RECOIL_LETTER
#define RECOIL_LETTER
#define OF_ADDON_USING_OFXBULLET

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "TextFontHolder.h"

class Letter : public Particle, TextObject  {
public:	
	GLdouble matrix[16]; 
	btRigidBody *bulletBodie;

	Letter(string _letter, TextFontHolder* _font, int _depth, int _forcedWidth = 0, bool _blockTogether = false){
		forcedWidth = _forcedWidth;
		setFont(_font);
		setLetter(_letter);		
		depth = _depth;
		blockTogether = _blockTogether;
		setPosition(0,0,0);
	}
	
	void setLetter(string _letter){
		letter = _letter;
	}

	string getLetter(){
		return letter;
	}
	
	void setFont(TextFontHolder* _font){
		font = _font;
	}	
	TextFontHolder* getFont(){
		return font;
	}
	void drawBricks(){
		
		
		glPushMatrix();		
		glTranslated(loc.x, loc.y, loc.z);		
		glMultMatrixd(matrix);		
		glTranslated(-getWidth()/2.0,-getFont()->getCalculatedHeight()/2.0, 0);
	
		//** BOUNDING BOX BEGIN
		ofDisableAlphaBlending();	
		glPushMatrix();
		ofSetColor(40, 40, 40);
		//ofSetColor(0, 0, 255);
		glNormal3d(0.0,0.0,-1.0);
		
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, depth/2.0);
		glVertex3i(getWidth(), -1, depth/2.0);
		glVertex3i(getWidth(), font->getCalculatedHeight()+1, depth/2.0);
		glVertex3i(-1, font->getCalculatedHeight()+1, depth/2.0);
		glEnd();
		//ofSetColor(0, 255,0);
		
		glNormal3d(0.0,0.0,1.0);
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, -depth/2.0);
		glVertex3i(getWidth(), -1, -depth/2.0);
		glVertex3i(getWidth(), font->getCalculatedHeight()+1, -depth/2.0);
		glVertex3i(-1, font->getCalculatedHeight()+1, -depth/2.0);
		glEnd();
		//ofSetColor(255, 0, 0);
		
		glNormal3d(-1.0,0.0,0.0);
		glBegin(GL_QUADS);
		glVertex3i(getWidth(), -1, -depth/2.0);
		glVertex3i(getWidth(), -1, depth/2.0);
		glVertex3i(getWidth(), font->getCalculatedHeight()+1, depth/2.0);
		glVertex3i(getWidth(), font->getCalculatedHeight()+1, -depth/2.0);
		glEnd();
		//ofSetColor(0, 255, 255);
		
		glNormal3d(1.0,0.0,0.0);
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, -depth/2.0);
		glVertex3i(-1, -1, depth/2.0);
		glVertex3i(-1, font->getCalculatedHeight()+1, depth/2.0);
		glVertex3i(-1, font->getCalculatedHeight()+1, -depth/2.0);
		glEnd();
		//ofSetColor(255, 0, 255);
		
		glNormal3d(0.0,-1.0,0.0);
		glBegin(GL_QUADS);
		glVertex3i(-1, font->getCalculatedHeight()+1, -depth/2.0);
		glVertex3i(getWidth(), font->getCalculatedHeight()+1, -depth/2.0);
		glVertex3i(getWidth(), font->getCalculatedHeight()+1, depth/2.0);
		glVertex3i(-1, font->getCalculatedHeight()+1, depth/2.0);
		glEnd();
		//ofSetColor(255, 255, 0);
		
		glNormal3d(0.0,1.0,0.0);
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, -depth/2.0);
		glVertex3i(getWidth(), -1, -depth/2.0);
		glVertex3i(getWidth(), -1, depth/2.0);
		glVertex3i(-1, -1, depth/2.0);
		glEnd();
		glPopMatrix();
		
		//END BOUNDING BOX **/

		glPopMatrix();
	}
	
	
	void drawText(){
		glPushMatrix();		
			glTranslated(loc.x, loc.y, loc.z);		
			glMultMatrixd(matrix);		
			glTranslated(-getWidth()/2.0,-getFont()->getCalculatedHeight()/2.0, 0);
			ofSetColor(255,255,255, 255);
			glPushMatrix();
		glTranslated(3, (font->getCalculatedHeight())/2.0+6,  0.0);//-depth/2.0);
				glNormal3d(0.0,0.0,1.0);
				font->renderString((string)letter);
			glPopMatrix();
		glPopMatrix();
	}
	
	float getWidth(){
		if(forcedWidth != 0){
			return forcedWidth;
		} else if (blockTogether){
			return font->getWidth(letter + "p");
		}
		else if(letter == "\n" || letter == " "){
			return font->getCharSetWidth("p");
		} else {
			/*int c = 0;
			for(int i=0;i<letter.size();i++){
				c+=font->getCharSetWidth(letter[i]);
				if(letter[i] == ' '){
					c+= font->getCharSetWidth("p");
				}
			}*/
			return font->getCharSetWidth(letter)+1;
		}
	}
	
	float getHeight(){
		if(letter == "\n" || letter == " "){
			return font->getHeight((string)"p");
		} else {
			return font->getHeight((string)letter);
		}
	}
	
	void setPosition(float x, float y, float z){
		loc.x = x;
		loc.y = y;
		loc.z = z;
	}
	
	void translate(float x, float y, float z){
		loc.x += x;
		loc.y += y;
		loc.z += z;
	}
	
	ofPoint getLoc(){
		return loc;
	}
	
	float getDepth(){
		return depth;
	}
private:
	
	TextFontHolder* font;
	string letter;
	ofPoint loc;
	float depth;
	int forcedWidth;
	bool blockTogether;

	

};

#endif
#ifndef RECOIL_LETTER
#define RECOIL_LETTER
#define OF_ADDON_USING_OFXBULLET

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "TextFontHolder.h"

class Letter : public TextObject  {
public:	
	GLdouble matrix[16]; 
	btRigidBody *bulletBodie;

	Letter(wstring _letter, TextFontHolder* _font, float _depth, int _forcedWidth = 0, float _forcedHeight = 0., bool _blockTogether = false){
		forcedWidth = _forcedWidth;
		forcedHeight = _forcedHeight;
		setFont(_font);
		setLetter(_letter);		
		depth = _depth;
		blockTogether = _blockTogether;
		useBullet = false;
		setPosition(0,0,0);
	}

	Letter(string _letter, TextFontHolder* _font, float _depth, int _forcedWidth = 0, float _forcedHeight = 0., bool _blockTogether = false){
		wstring utf32result;
		utf8::utf8to32(_letter.begin(), _letter.end(), back_inserter(utf32result));
		Letter(utf32result, _font, _depth, _forcedWidth, _forcedHeight, _blockTogether);
	}
	
	void setLetter(wstring _letter){
		letter = _letter;
	}
	
	void setLetter(string _letter){
		wstring utf32result;
		utf8::utf8to32(_letter.begin(), _letter.end(), back_inserter(utf32result));
		setLetter(utf32result);
	}
	
	string getLetter(){
		string utf8result;
		utf8::utf32to8(letter.begin(), letter.end(), back_inserter(utf8result));
		return utf8result;
	}
	
	wstring getWLetter(){
		return letter;
	}
	
	void setFont(TextFontHolder* _font){
		font = _font;
	}	
	TextFontHolder* getFont(){
		return font;
	}
	void drawBricks(){
		
		float height = getFont()->getCalculatedHeight();
		if(forcedHeight != 0.0){
			height = forcedHeight;
		}
		
		
		glPushMatrix();		
		glTranslated(loc.x, loc.y, loc.z);
		if(useBullet)
			glMultMatrixd(matrix);		
		glTranslated(-getWidth()/2.0,-height/2.0, 0);
	
		//** BOUNDING BOX BEGIN
		//ofDisableAlphaBlending();	
		glPushMatrix();
		ofSetColor(40, 40, 40,100);
		//ofSetColor(0, 0, 255);
		glNormal3d(0.0,0.0,-1.0);
		
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, depth/2.0);
		glVertex3i(getWidth(), -1, depth/2.0);
		glVertex3i(getWidth(), height+1, depth/2.0);
		glVertex3i(-1, height+1, depth/2.0);
		glEnd();
		//ofSetColor(0, 255,0);
		
		glNormal3d(0.0,0.0,1.0);
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, -depth/2.0);
		glVertex3i(getWidth(), -1, -depth/2.0);
		glVertex3i(getWidth(), height+1, -depth/2.0);
		glVertex3i(-1, height+1, -depth/2.0);
		glEnd();
		//ofSetColor(255, 0, 0);
		
		glNormal3d(-1.0,0.0,0.0);
		glBegin(GL_QUADS);
		glVertex3i(getWidth(), -1, -depth/2.0);
		glVertex3i(getWidth(), -1, depth/2.0);
		glVertex3i(getWidth(), height+1, depth/2.0);
		glVertex3i(getWidth(), height+1, -depth/2.0);
		glEnd();
		//ofSetColor(0, 255, 255);
		
		glNormal3d(1.0,0.0,0.0);
		glBegin(GL_QUADS);
		glVertex3i(-1, -1, -depth/2.0);
		glVertex3i(-1, -1, depth/2.0);
		glVertex3i(-1, height+1, depth/2.0);
		glVertex3i(-1, height+1, -depth/2.0);
		glEnd();
		//ofSetColor(255, 0, 255);
		
		glNormal3d(0.0,-1.0,0.0);
		glBegin(GL_QUADS);
		glVertex3i(-1, height+1, -depth/2.0);
		glVertex3i(getWidth(), height+1, -depth/2.0);
		glVertex3i(getWidth(), height+1, depth/2.0);
		glVertex3i(-1, height+1, depth/2.0);
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
			if(useBullet)
				glMultMatrixd(matrix);		
			glTranslated(-getWidth()/2.0,-getFont()->getCalculatedHeight()/2.0, 0);
			//ofSetColor(255,255,255, 255);
			glPushMatrix();
				glTranslated(3, (font->getCalculatedHeight())/2.0+6,  0.0);//-depth/2.0);
				glNormal3d(0.0,0.0,1.0);
				font->renderString((wstring)letter);
			glPopMatrix();
		glPopMatrix();
	}
	
	float getWidth(){
		if(forcedWidth != 0){
			return forcedWidth;
		} else if (blockTogether){
			if(letter == L"\n"){
				return 0.;
			}	
			return font->getWidth(letter + L" ");
		}
		else if(letter == L"\n" || letter == L" "){
			return font->getCharSetWidth(L" ");
		} else {
			/*int c = 0;
			for(int i=0;i<letter.size();i++){
				c+=font->getCharSetWidth(letter[i]);
				if(letter[i] == ' '){
					c+= font->getCharSetWidth("p");
				}
			}*/
			return font->getCharSetWidth((wstring)letter);
		}
	}
	
	float getHeight(){
		if(forcedHeight != 0.0){
			return forcedHeight;
		}
		if(letter == L"\n" || letter == L" "){
			return font->getCalculatedHeight();
		} else {
			return font->getCalculatedHeight();
		}
	}
	
	void setPosition(float x, float y, float z){
		loc.x = x;
		loc.y = y;
		loc.z = z;

		orgLoc.x = x;
		orgLoc.y = y;
		orgLoc.z = z;
	}
	
	ofPoint getPosition(){
		return getLoc();
	}
	
	ofPoint getLoc(){
		return loc;
	}
	
	void translate(float x, float y, float z){
		loc.x = orgLoc.x + x;
		loc.y = orgLoc.y + y;
		loc.z = orgLoc.z + z;
	}
	
	ofPoint getTranslate(){
		return ofPoint(loc.x - orgLoc.x, loc.y - orgLoc.y, loc.z - orgLoc.z);
	}
	
	float getDepth(){
		return depth;
	}

	bool useBullet;

private:
	
	TextFontHolder* font;
	wstring letter;
	ofPoint orgLoc;
	ofPoint loc;
	float depth;
	int forcedWidth;
	float forcedHeight;
	bool blockTogether;

};

#endif
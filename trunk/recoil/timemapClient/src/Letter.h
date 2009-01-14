#ifndef RECOIL_LETTER
#define RECOIL_LETTER

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "TextFontHolder.h"

class Letter : public Particle, TextObject  {
public:	
	GLdouble matrix[16]; 

	Letter(string _letter, TextFontHolder* _font){
		setFont(_font);
		setLetter(_letter);		

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
	void draw(){
		ofEnableAlphaBlending();

		glPushMatrix();
		glTranslated(loc.x, loc.y, loc.z);
		glMultMatrixd(matrix);
		glTranslated(-getWidth()/2.0, -font->getCalculatedHeight()/2.0,0);
		glPushMatrix();
		glTranslated(0, font->getCalculatedHeight()/2.0, 0);
		font->renderString((string)letter);
		glPopMatrix();
		
		
		/** BOUNDING BOX BEGIN
		
		glPushMatrix();
		ofSetColor(255, 0, 0, 100);
		glBegin(GL_QUADS);
		 glVertex3i(0, 0, 0);
		 glVertex3i(getWidth(), 0, 0);
		 glVertex3i(getWidth(), font->getCalculatedHeight(), 0);
		 glVertex3i(0, font->getCalculatedHeight(), 0);
		glEnd();
		glPopMatrix();
		 
		END BOUNDING BOX **/
		
		glPopMatrix();
	}
	
	int getWidth(){
		return font->getCharSetWidth(letter[0]);
	}
	
	int getHeight(){
		return font->getHeight((string)letter);

	}
	
	void setPosition(int x, int y, int z){
		loc.x = x;
		loc.y = y;
		loc.z = z;
	}
	

	
	ofPoint getLoc(){
		return loc;
	}
private:
	
	TextFontHolder* font;
	string letter;
	ofPoint loc;

	

};

#endif
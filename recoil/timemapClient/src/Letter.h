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
		
		//ofCircle(0,0,10);
		
			glTranslated(-getWidth()/2.0,-((font->getCalculatedHeight()/2.0+(font->getFontSize()/2.0))+(font->getStringBoundingBox(getLetter(), 0, 0).y)+(font->getStringBoundingBox(getLetter(), 0, 0).height))/2.0 ,0);
		
			glPushMatrix();
				glTranslated(0, (font->getCalculatedHeight()+(font->getFontSize()/2.0))/2.0 , 0);
				//glTranslated(0, font->getStringBoundingBox(getLetter(), 0, 0).height/2.0, 0);
				font->renderString((string)letter);
			glPopMatrix();
		
		/** BOUNDING BOX BEGIN
		
			glPushMatrix();
				ofSetColor(255, 0, 0, 100);
				glBegin(GL_QUADS);
					glVertex3i(0, 0, 0);
					glVertex3i(getWidth()-1, 0, 0);
					glVertex3i(getWidth()-1, font->getCalculatedHeight(), 0);
					glVertex3i(0, font->getCalculatedHeight(), 0);
				glEnd();
			glPopMatrix();
		 
		//END BOUNDING BOX **/
		
		glPopMatrix();
	}
	
	float getWidth(){
		if(letter == "\n"){
			return font->getCharSetWidth('p');
		} else {
			return font->getCharSetWidth(letter[0]);
		}
	}
	
	float getHeight(){
		if(letter == "\n"){
			return font->getHeight((string)"p");
		} else {
			return font->getHeight((string)letter);
		}
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
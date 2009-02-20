#ifndef RECOIL_WORD
#define RECOIL_WORD

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "Letter.h"


class Word : public Particle, TextObject  {
public:
	vector<Letter> letters;	
	
	Word(string text, TextFontHolder * font, int depth, bool blockTogether = false){
		if(!blockTogether){
			for(int i=0; i<text.size(); i++){
				letters.push_back(Letter(text.substr(i,1), font, depth,0, blockTogether));
			}
		} else {
			letters.push_back(Letter(text, font, depth,0, blockTogether));
		}
	}
	
	void drawText(){
		for(int i=0;i< letters.size(); i++){
			if(letters[i].getLetter() != "\n"){
				letters[i].drawText();
			}
		}
	}
	
	void drawBricks(){
		for(int i=0;i< letters.size(); i++){
			letters[i].drawBricks();
		}
	}
	
	void setPosition(int x, int y, int z){
		int countX=0, countY=0, countZ=0;
		for(int i=0;i<letters.size();i++){
//			letters[i].setPosition(countX+x+letters[i].getWidth()/2.0, countY+y+(letters[i].getFont()->getCalculatedHeight()/2.0+(letters[i].getFont()->getFontSize()/2.0))/2.0+(letters[i].getFont()->getStringBoundingBox(letters[i].getLetter(), 0, 0).y/2.0)+(letters[i].getFont()->getStringBoundingBox(letters[i].getLetter(), 0, 0).height/2.0), countZ+z);
			letters[i].setPosition(countX+x+letters[i].getWidth()/2.0, countY+y+letters[i].getFont()->getCalculatedHeight()/2.0, countZ+z);
			countX += letters[i].getWidth();
		}
	}
	
	float getWidth(){
		float w = 0;
		for(int i=0; i<letters.size(); i++){
			w += letters[i].getWidth();
		}
		return w;
	} 
	
	float getHeight(){
		float h = 0;
		for(int i=0; i<letters.size(); i++){
			if(h < letters[i].getHeight()){
				h = letters[i].getHeight();
			}
		}
		return h;
	}
	
	int getNumberLetters(){
		return letters.size();
	}
	
		
private:
	

};

#endif
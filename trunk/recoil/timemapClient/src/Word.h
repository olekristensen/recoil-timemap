#ifndef RECOIL_WORD
#define RECOIL_WORD

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "Letter.h"


class Word : public Particle, TextObject  {
public:
	vector<Letter> letters;	
	
	Word(string text, TextFontHolder * font){
		for(int i=0; i<text.size(); i++){
			letters.push_back(Letter(text.substr(i,1), font));
		}
	}
	
	void draw(){
		for(int i=0;i< letters.size(); i++){
			letters[i].draw();
		}
	}
	
	void setPosition(int x, int y, int z){
		int countX=0, countY=0, countZ=0;
		for(int i=0;i<letters.size();i++){
			letters[i].setPosition(countX+x+letters[i].getWidth()/2.0, countY+y+letters[i].getFont()->getCalculatedHeight()/2.0, countZ+z);
			countX += letters[i].getWidth();
		}
	}
	
	int getWidth(){
		int w = 0;
		for(int i=0; i<letters.size(); i++){
			w += letters[i].getWidth();
		}
		return w;
	} 
	
	int getHeight(){
		int h = 0;
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
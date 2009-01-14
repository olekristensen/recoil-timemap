#ifndef RECOIL_TEXT
#define RECOIL_TEXT

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "Word.h"

class Text : public Particle, TextObject {
public:
	vector<Word> words;
	
	Text(string text, TextFontHolder * font, int x, int y, int width){
		setText(text, font);		
		setPosition(x,y,0, width);
		lineHeight = 1.0;
	}
	
	void clear(){
		words.clear();
	}
	
	void setText(string text, TextFontHolder * font){
		//Start by erasing the text. Should be edited later to be more inteligent
		this->clear();
		
		
				
		string cur_word = "";
		for(int i=0; i<=text.size(); i++){
			if(i == text.size() || text.at(i) == ' ' || text.at(i) + "" == "\n" ){
				words.push_back(Word(cur_word,  font));
				cur_word = "";
			} else {
				cur_word += text.at(i);
			}
		}			
	}
	
	void setPosition(int x, int y, int z, int width){
		//hack using the same letter for space as of
		int spaceWidth = Letter("p", words[0].letters[0].getFont()).getWidth();
		
		numberLines = 1;
		int countX=x, countY=y,countZ=z;
		for(int i=0;i< words.size(); i++){
			for(int j=0;j<words[i].getNumberLetters();j++){
				if(words[i].letters[j].getLetter() == "\n"){
					countX = x;
					countY += words[i].letters[0].getFont()->getCalculatedHeight();
					numberLines++;
				}
			}
			if((countX-x)+words[i].getWidth()+spaceWidth > width){
				countX = x;
				countY += words[i].letters[0].getFont()->getCalculatedHeight();
				numberLines++;
			}
			words[i].setPosition(countX, countY, countZ);
			countX += words[i].getWidth()+spaceWidth;
		}
	}
	
	void draw(){
		for(int i=0;i< words.size(); i++){
			words[i].draw();
		}
	}
	
	void setLineHeight(float lheight){
		lineHeight = lheight;
	}
	
	int getNumberWords(){
		return words.size();
	}
	int getNumberLetters(){
		int r = 0;
		for(int i=0;i<words.size();i++){
			r+= words[i].getNumberLetters();
		}
		return r;
	}
	
	Letter * getLetter(int index){
		int counter = 0;
		int wordCounter = 0;
		int letterCounter = 0;
		Letter * r;
		while(counter<=index){
			r = &words[wordCounter].letters[letterCounter];
			counter ++;
			letterCounter ++;
			if(letterCounter >= words[wordCounter].getNumberLetters()){
				wordCounter++;
				letterCounter = 0;
			}
		}
		return r;
	}
	
	int getHeight(){
		cout << "numberLines: " << numberLines << endl;
		return getLetter(0)->getFont()->getCalculatedHeight()*(numberLines+1);
	}
	
	
private:
	float lineHeight;
	int numberLines;
	
};

#endif
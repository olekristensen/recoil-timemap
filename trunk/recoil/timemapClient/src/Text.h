#ifndef RECOIL_TEXT
#define RECOIL_TEXT
#define OF_ADDON_USING_OFXBULLET

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "Word.h"
#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/find.hpp"

using namespace std;
using namespace boost;


class Text : public Particle, TextObject {
public:
	vector<Word> words;
	vector<Letter> spacers;
	vector<vector<Word*> > lines;
	
	Text(){
	wordBlocks = false;
	}
	
	void setWordBlocks(bool i){
		wordBlocks = i;
	}
	void constructText(){
		createText(text, font);
		positionText();
		createSpacers();
	}


	//----
	//Creates the word objects (and letter objects)
	void createText(string _text, TextFontHolder * font){		
		//Start by erasing the text. Should be edited later to be more inteligent
		this->clear();
		
		string text = trim_copy(_text);
		//string text = (_text);
		while(find_first(text, "  ")){
			replace_all(text, "  ", " ");
		}
		while(find_first(text, "\n ")){
			replace_all(text, "\n ", "\n");
		}
		while(find_first(text, " \n")){
			replace_all(text, " \n", "\n");
		}
		
		string cur_word = "";
		for(int i=0; i<=text.size(); i++){
			if(i == text.size() || text.at(i) == ' ' || text.at(i) == '\n') {
				if(i < text.size()){
					if(text.at(i) == '\n'){
						cur_word += "\n";
					}
					if(text.at(i) == ' '){
						cur_word += " ";
					}
				}
				words.push_back(Word(cur_word, font, depth, wordBlocks));
				cur_word = "";
			} else {
				cur_word += text.at(i);
			}
		}			
	}
	
	//----
	//Setting up the words to make lines. Created from 0,0,0, and can later be translated
	void positionText(){
		lines.clear();
		lines.push_back(vector<Word*>());
		int countX=0, countY=0,countZ=0;
		for(int i=0;i< words.size(); i++){
			if((countX)+words[i].getWidth() > width){
				countX = 0;
				countY += words[i].letters[0].getFont()->getCalculatedHeight();
				lines.push_back(vector<Word*>());
			}
			words[i].setPosition(countX, countY, countZ);
			lines[lines.size()-1].push_back(&words[i]);
			countX += words[i].getWidth();
			for(int j=0;j<words[i].getNumberLetters();j++){
				if(words[i].letters[j].getLetter() == "\n"){
					countX = 0;
					countY += words[i].letters[0].getFont()->getCalculatedHeight();
				}
			}
		}
	}
	
	void createSpacers(){
		for(int i=0;i<getNumberLines();i++){
		//	spacers.push_back(Letter(" ", font, depth, width - getLineWidth(0)));
			//Letter l = Letter(" ", font, depth, width - getLineWidth(0));
			int numberSpaces = (int)((width - getLineWidth(i))/(float)font->getWidth("p"));
			float w;
			if(numberSpaces == 0){
				numberSpaces = 1;
				w = ((int)(width - getLineWidth(i))%(int)font->getWidth("p"));
			} else {
				w = font->getWidth("p")+((int)(width - getLineWidth(i))%(int)font->getWidth("p"))/(float)numberSpaces;
			}
			//cout<<((int)(width - getLineWidth(i))%(int)font->getWidth("p"))/(float)numberSpaces<<endl;
			for(int u=0;u<numberSpaces;u++){
				Letter l = Letter(" ", font, depth, w);
				l.setPosition(getLineWidth(i)+u*w+w/2.0, lines[i][0]->letters[0].getLoc().y, 0);
				spacers.push_back(l);
			}
		}
	}
	
	void translate(int _x, int _y, int _z){
		for(int i=0;i<getNumberLetters(); i++){
			getLetter(i)->translate(_x, _y, _z);
		}
	}
	
	void drawText(){

		for(int i=0;i< words.size(); i++){
			words[i].drawText();
		}
		for(int i=0;i<spacers.size(); i++){
			
			spacers[i].drawText();
		}
	}
	void drawBricks(){
		for(int i=0;i< words.size(); i++){
			words[i].drawBricks();
		}
		for(int i=0;i<spacers.size(); i++){
			
			spacers[i].drawBricks();
		}
	}

	
	void setLineHeight(float lheight){
		font->setLineHeight(lheight);
	}
	
	int getNumberWords(){
		return words.size();
	}
	int getNumberLetters(){
		int r = 0;
		for(int i=0;i<words.size();i++){
			r+= words[i].getNumberLetters();
		}
		r += spacers.size();
		return r;
	}
	
	Letter * getLetter(int index){
		int counter = 0;
		int wordCounter = 0;
		int letterCounter = 0;
		Letter * r;
		if(index < getNumberLetters()- spacers.size()){
			while(counter<=index){
				r = &words[wordCounter].letters[letterCounter];
				counter ++;
				letterCounter ++;
				if(letterCounter >= words[wordCounter].getNumberLetters()){
					wordCounter++;
					letterCounter = 0;
				}
			}
		} else {
//			cout<<(index-getNumberLetters())- (int)spacers.size() -1<<endl;
			r = &spacers[index-(getNumberLetters()- spacers.size())];
		}
		return r;
	}
	
	float getHeight(){
		return (font->getCalculatedHeight()*getNumberLines());
	}
	
	void setText(string _text){
		text = _text;
	}
	
	void setDepth(int _depth){
		depth = _depth;
	}
	
	void setWidth(int _width){
		width = _width;
	}
	
	int getNumberLines(){
		return lines.size();
	}
	void setFont( TextFontHolder * _font){
		font = _font;
	}
	
	int getLineWidth(int index){
		int c=0;
		for(int i=0;i<lines[index].size();i++){
			c+=lines[index][i]->getWidth();
		}
		return c;
	}
	
	
	void setupBullet(btDiscreteDynamicsWorld * dynamicsWorld, vector<btRigidBody*>* bodies){
		int numLetters = getNumberLetters();
		//bodies = new btRigidBody*[numLetters];
		for(int i=0; i<numLetters; i++){
			Letter * l = getLetter(i);
			btCollisionShape* fallShape = new btBoxShape(btVector3((getLetter(i)->getWidth()-1)/200.0,(l->getFont()->getCalculatedHeight()/200.0),getLetter(i)->getDepth()/200.0));
			btVector3 pos = btVector3(l->getLoc().x/100.0, l->getLoc().y/100.0, l->getLoc().z/100.0);
			btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.3),pos));
			btScalar mass = 1.0;
			btVector3 fallInertia(0,0,0);
			fallShape->calculateLocalInertia(mass,fallInertia);
			btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(mass,fallMotionState,fallShape,fallInertia);
			l->bulletBodie = new btRigidBody(fallRigidBodyCI);
			l->bulletBodie->setDamping(0,0);
			l->bulletBodie->setFriction(.6);
			l->bulletBodie->setDamping(0.99,0.99);

			bodies->push_back(l->bulletBodie);
			//		bodies[i]->setRestitution(.5);
			dynamicsWorld->addRigidBody(l->bulletBodie);
		}	
	}
	void clear(){
		words.clear();
		spacers.clear();
		lines.clear();
		numberLines = 0;
	}
private:
	int numberLines;
	string text;
	int depth;
	int width;
	TextFontHolder * font;
	bool wordBlocks;
};

#endif
#ifndef RECOIL_TEXT
#define RECOIL_TEXT
#define OF_ADDON_USING_OFXBULLET
#define OF_ADDON_USING_OFXUTF8

#include "ofMain.h"
#include "ofAddons.h"

#include "textObject.h"
#include "Word.h"
#include "boost/algorithm/string/replace.hpp"
#include "boost/algorithm/string/trim.hpp"
#include "boost/algorithm/string/find.hpp"

using namespace std;
using namespace boost;


class Text : public TextObject {
public:
	vector<Word> words;
	vector<Letter> spacers;
	vector<vector<Word*> > lines;
	
	Text(){
		wordBlocks = false;
		numberLines = 0;
		depth = 100;
		width = 400;
		friction = 0.25;
		restitution = 0.4;
		damping = 0.0;
		mass = 1.0;
	}
	
	void setWordBlocks(bool _wordBlocks){
		wordBlocks = _wordBlocks;
	}

	bool getWordBlocks(){
		return wordBlocks;
	}
	
	void constructText(){
		cursor = true;
		createText(text, font);
		positionText();
		createSpacers();
	}


	//----
	//Creates the word objects (and letter objects)
	void createText(wstring text, TextFontHolder * font){		
		//Start by erasing the text. Should be edited later to be more inteligent
		this->clear();
		wstring cur_word = L"";
		for(int i=0; i<=text.length(); i++){
			if(i == text.length() || text.substr(i,1) == L" " || text.substr(i,1) == L"\n") {
				if(i < text.length()){
					if(text.substr(i,1) == L"\n"){
						if(wordBlocks){
							words.push_back(Word(cur_word, font, depth, wordBlocks));
							cur_word = L"\n";
						} else {
							cur_word += L"\n";
						}
					}
					if(text.substr(i,1) == L" "){
						cur_word += L" ";
					}
				}
				words.push_back(Word(cur_word, font, depth, wordBlocks));
				cur_word = L"";
			} else {
				cur_word += text.substr(i,1);
			}
		}
	}
	
	//----
	//Setting up the words to make lines. Created from 0,0,0, and can later be translated
	void positionText(){
		lines.clear();
		lines.push_back(vector<Word*>());
		float countX=0.0, countY=0.0,countZ=0.0;
		for(int i=0;i< words.size(); i++){
			if((countX)+words[i].getWidth() > width){
				countX = 0.0;
				countY += words[i].letters[0].getFont()->getCalculatedHeight();
				lines.push_back(vector<Word*>());
			}
			words[i].setPosition(countX, countY, countZ);
			lines[lines.size()-1].push_back(&words[i]);
			countX += words[i].getWidth();
			for(int j=0;j<words[i].getNumberLetters();j++){
				if(words[i].letters[j].getWLetter() == L"\n"){
					countX = 0.0;
					countY += words[i].letters[0].getFont()->getCalculatedHeight();
					lines.push_back(vector<Word*>());
				}
			}
		}
	}
	
	void createSpacers(){
		for(int i=0;i<getNumberLines();i++){
		//	spacers.push_back(Letter(" ", font, depth, width - getLineWidth(0)));
			//Letter l = Letter(" ", font, depth, width - getLineWidth(0));
			int numberSpaces = (int)((width - getLineWidth(i))/(float)font->getWidth(L"   "));
			float w;
			if(numberSpaces == 0){
				numberSpaces = 1;
				w = fmodf((width - getLineWidth(i)),font->getWidth(L"   "));
			} else {
				w = font->getWidth(L"   ")+fmodf((width - getLineWidth(i)),font->getWidth(L"   "))/(float)numberSpaces;
			}
			//cout<<((int)(width - getLineWidth(i))%(int)font->getWidth("p"))/(float)numberSpaces<<endl;
			for(int u=0;u<numberSpaces;u++){
				Letter l = Letter(L"   ", font, depth, w);
				l.setPosition(getLineWidth(i)+u*w+w/2.0, lines[i][0]->letters[0].getLoc().y, 0);
				spacers.push_back(l);
			}
		}
	}
	
	void translate(float _x, float _y, float _z){
		for(int i=0;i<getNumberLetters(); i++){
			getLetter(i)->translate(_x, _y, _z);
		}
	}

	ofPoint getTranslate(){
		return getLetter(0)->getTranslate();
	}

	void drawText(){
		for(int i=0;i< words.size(); i++){
			words[i].drawText();
		}
		for(int i=0;i<spacers.size(); i++){
			
			spacers[i].drawText();
		}
		/**
		if(cursor){
			int cursorPos = 0;
			for(int i = text.size() - 1; i >= 0; i--) {
				if(previousText[i] != text[i])
					cursorPos = i;
			}
			if(cursorPos == (0) || cursorPos > text.size())
				cursorPos = text.size()-1;
			float x,y;
			x = (getLetter(cursorPos))->getLoc().x;
			y = (getLetter(cursorPos))->getLoc().y;
			cout << "cursor at " << text[cursorPos] << " - " << cursorPos << "  " << ofToString(x) << ", " << ofToString(y) << endl;
			ofNoFill();
			ofLine(x, y-(font->getHeight("I")/2.0), x, y+(font->getHeight("I")/2.0));
		}
		 **/
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
	
	float getLineHeight(){
		font->getLineHeight();
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
		wstring utf32result;
		utf8::utf8to32(_text.begin(), _text.end(), back_inserter(utf32result));
		setText(utf32result);
	}
	
	void setText(wstring _text){
		text = _text;
	}
	
	string getText(){
		string utf8result;
		utf8::utf32to8(text.begin(), text.end(), back_inserter(utf8result));
		return utf8result;
	}
	
	void setDepth(int _depth){
		depth = _depth;
	}
	
	float getDepth(){
		return depth;
	}
	
	void setWidth(float _width){
		width = _width;
	}
	
	float getWidth(){
		return width;
	}
	
	void setDamping(float _damping){
		damping = _damping;
	}
	
	float getDamping(){
		return damping;
	}
	
	void setMass(float _mass){
		mass = _mass;
	}
	
	float getMass(){
		return mass;
	}
	
	void setRestitution(float _restitution){
		restitution = _restitution;
	}
	
	float getRestitution(){
		return restitution;
	}
	
	void setFriction(float _friction){
		friction = _friction;
	}
	
	float getFriction(){
		return friction;
	}
		
	int getNumberLines(){
		//cout << "getNumerLines() == " << lines.size() << endl;
		return lines.size();
	}
	
	int getNumberBodies() {
		return bodies.size();
	}
	
	void setFont( TextFontHolder * _font){
		font = _font;
	}
	
	float getFontSize(){
		return font->getFontSize();
	}
	
	float getLineWidth(int index){
		float c=0;
		for(int i=0;i<lines[index].size();i++){
			c+=lines[index][i]->getWidth();
		}
		return c;
	}
	
	
	void setupBullet(btDiscreteDynamicsWorld * _dynamicsWorld){
		dynamicsWorld = _dynamicsWorld;
		int numLetters = getNumberLetters();
		for(int i=0; i<numLetters; i++){
			Letter * l = getLetter(i);
			btCollisionShape* fallShape = new btBoxShape(btVector3((getLetter(i)->getWidth()-1)/200.0,(l->getFont()->getCalculatedHeight()/200.0),getLetter(i)->getDepth()/200.0));
			m_collisionShapes.push_back(fallShape);
			btVector3 pos = btVector3(l->getLoc().x/100.0, l->getLoc().y/100.0, l->getLoc().z/100.0);
			//cout<< l->getLoc().x/100.0 << ", "<<l->getLoc().y/100.0<<endl;
			btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.1),pos));
			btScalar btMass = mass;
			btVector3 fallInertia(0,0,0);
			fallShape->calculateLocalInertia(btMass,fallInertia);
			btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(btMass,fallMotionState,fallShape,fallInertia);
			l->bulletBodie = new btRigidBody(fallRigidBodyCI);
			l->bulletBodie->setFriction(friction);
			l->bulletBodie->setDamping(damping,damping);
			l->bulletBodie->setRestitution(restitution);
			bodies.push_back(l->bulletBodie);
			dynamicsWorld->addRigidBody(l->bulletBodie);
			l->useBullet = true;
		}	
	}
	
	void clear(){
		clearBullet();
		words.clear();
		spacers.clear();
		lines.clear();
		numberLines = 0;
	}
	
	void clearBullet(){
		//cout << "bodies: " << bodies.size() << " collisionshapes: " << m_collisionShapes.size() << endl;
		for (int i=0;i<m_collisionShapes.size();i++) {
			btCollisionShape* shape = m_collisionShapes[i];
			delete shape;
		}
		m_collisionShapes.clear();
		for( int i=0; i < bodies.size(); i++ ) {
			dynamicsWorld->removeRigidBody(bodies.at(i));
		}
		bodies.clear();
	}
		

	void updateBullet(){
	
	int numLetters = getNumberLetters();
	btTransform trans[numLetters];
	btVector3 pos[numLetters];
	btMatrix3x3 basis[numLetters];
	vector<Letter*> l;
		
	for(int i=0; i<numLetters; i++){
		bodies[i]->getMotionState()->getWorldTransform(trans[i]);
		l.push_back(getLetter(i));
		pos[i] = trans[i].getOrigin();
		basis[i] = trans[i].getBasis();		
		bodies[i]->setFriction(friction);
		bodies[i]->setDamping(damping,damping);
		bodies[i]->setRestitution(restitution);
	}
	
#pragma omp parallel for 
	for(int i=0; i<numLetters; i++){
		l[i]->setPosition(pos[i].getX()*100, pos[i].getY()*100, pos[i].getZ()*100);
		l[i]->matrix[0] = basis[i].getRow(0)[0];
		l[i]->matrix[4] = basis[i].getRow(0)[1];
		l[i]->matrix[8] = basis[i].getRow(0)[2];
		l[i]->matrix[12] = 0;
		l[i]->matrix[1] = basis[i].getRow(1)[0];
		l[i]->matrix[5] = basis[i].getRow(1)[1];
		l[i]->matrix[9] = basis[i].getRow(1)[2];
		l[i]->matrix[13] = 0;
		l[i]->matrix[2] = basis[i].getRow(2)[0];
		l[i]->matrix[6] = basis[i].getRow(2)[1];
		l[i]->matrix[10] = basis[i].getRow(2)[2];
		l[i]->matrix[14] = 0;
		l[i]->matrix[3] = 0;
		l[i]->matrix[7] = 0;
		l[i]->matrix[11] = 0;
		l[i]->matrix[15] = 1;
	}
}
	
private:
	int numberLines;
	wstring text;
	float depth;
	float width;
	float friction;
	float restitution;
	float damping;
	float mass;
	TextFontHolder * font;
	bool wordBlocks;
	vector<btRigidBody*> bodies;
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	btDiscreteDynamicsWorld * dynamicsWorld;
	bool cursor;
};

#endif
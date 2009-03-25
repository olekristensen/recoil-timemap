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
	vector<Letter> scaffolding;
	vector<vector<Word*> > lines;
	
	Text(){
		wordBlocks = false;
		hasBeenAnimated = false;
		numberLines = 0;
		depth = 100;
		width = 400;
		friction = 0.25;
		restitution = 0.4;
		damping = 0.0;
		mass = 1.0;
		inertiaX = inertiaY = inertiaZ = 0.0;
		bulletResting = false;
	}
	
	void setWordBlocks(bool _wordBlocks){
		wordBlocks = _wordBlocks;
	}

	bool getWordBlocks(){
		return wordBlocks;
	}
	
	void constructText(bool useSpacers){
		hasBeenAnimated = false;
		cursor = true;
		bulletResting = false;

		createText(text, font);
		positionText();
		if(useSpacers){
			createSpacers();
		}
	}


	//----
	//Creates the word objects (and letter objects)
	void createText(wstring text, TextFontHolder * font){		
		//Start by erasing the text. Should be edited later to be more inteligent
		this->clear();
		wstring cur_word = L"";
		if(text.length() > 0){
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
	}
	
	//----
	//Setting up the words to make lines. Created from 0,0,0, and can later be translated
	void positionText(){
		if(getNumberLetters() > 0){
		lines.clear();
		lines.push_back(vector<Word*>());
		float countX=0.0, countY=0.0,countZ=0.0;
		int wordsInLine = 0;
		for(int i=0;i< words.size(); i++){
			if((countX)+words[i].getWidth() > width && wordsInLine > 0){
				countX = 0.0;
				countY += words[i].letters[0].getFont()->getCalculatedHeight();
				lines.push_back(vector<Word*>());
				wordsInLine = 0;
			}
			words[i].setPosition(countX, countY, countZ);
			lines[lines.size()-1].push_back(&words[i]);
			wordsInLine++;
			countX += words[i].getWidth();
			for(int j=0;j<words[i].getNumberLetters();j++){
				if(words[i].letters[j].getWLetter() == L"\n"){
					countX = 0.0;
					countY += words[i].letters[0].getFont()->getCalculatedHeight();
					lines.push_back(vector<Word*>());
					wordsInLine = 0;
				}
			}
		}
		}
	}
	
	void createSpacers(){
		for(int i=0;i<getNumberLines();i++){
		//	spacers.push_back(Letter(" ", font, depth, width - getLineWidth(0)));
			//Letter l = Letter(" ", font, depth, width - getLineWidth(0));
			int numberSpaces = (int)((width - getLineWidth(i))/(float)font->getWidth(L"     "));
			//cout << numberSpaces << endl;
			float w;
			if(numberSpaces == 0){
				numberSpaces = 1;
				w = fmodf((width - getLineWidth(i)),font->getWidth(L"     "));
			} else if (numberSpaces < 0){
				numberSpaces = 0;
			} else {
				w = font->getWidth(L"     ")+fmodf((width - getLineWidth(i)),font->getWidth(L"     "))/(float)numberSpaces;
			}
			for(int u=0;u<numberSpaces;u++){
				Letter l = Letter(L"     ", font, depth, w, 0);
				l.setPosition(getLineWidth(i)+(u*w)+(w/2.0), (font->getCalculatedHeight()*(float)i)+(font->getCalculatedHeight()/2.0), 0);
				spacers.push_back(l);
			}
		}
	}
	
	void createScaffolding() {
		scaffoldingOn = true;
		if(!hasBeenAnimated){
		scaffolding.clear();
		float screenHeight = ofGetWidth()/3.0;
		float height = getHeight();
		float y = getTranslate().y;
		float x = getTranslate().x;
		int numberOfLines = (int)((screenHeight - (height+y))/(float)font->getCalculatedHeight());
		if(numberOfLines == 0) 
			numberOfLines = 1;
		int numberSpaces = (int)(width/(float)font->getWidth(L"                    "));
		float w = font->getWidth(L"                    ")+fmodf(width,font->getWidth(L"                    "))/(float)numberSpaces;
		float h = ((screenHeight - (height+y))/(float)numberOfLines);
		for(int i=0;i<numberOfLines;i++){
			for(int u=0;u<numberSpaces;u++){
				if(i%2 == 0) {
					Letter l = Letter(L"                    ", font, depth, w, h);
					l.setPosition((u*w)+(w/2.0)+x,height+(h*(float)i)+(h/2.0)+y, 0);
					scaffolding.push_back(l);
				} else {
					if(u == 0){
						Letter l = Letter(L"          ", font, depth, w/2.0, h);
						l.setPosition((u*w)+(w/4.0)+x,height+(h*(float)i)+(h/2.0)+y, 0);
						scaffolding.push_back(l);
						Letter l2 = Letter(L"          ", font, depth, w/2.0, h);
						l2.setPosition((((numberSpaces)*w)+x)-(w/4.0),height+(h*(float)i)+(h/2.0)+y, 0);
						scaffolding.push_back(l2);
					} else {
						Letter l = Letter(L"                    ", font, depth, w, h);
						l.setPosition((u*w)+x,height+(h*(float)i)+(h/2.0)+y, 0);
						scaffolding.push_back(l);
					}
				}
			}
		}
		}
	}
	
	void removeScaffolding(){
		scaffoldingOn = false;
	}
	
	void translate(float _x, float _y, float _z){
		if(!hasBeenAnimated){
			for(int i=0;i<getNumberLetters(); i++){
				getLetter(i)->translate(_x, _y, _z);
			}
		}
	}

	ofPoint getTranslate(){
		if(getNumberLetters() > 0){
			return getLetter(0)->getTranslate();
		} else {
			return ofPoint(0,0,0);
		}
	}

	void drawText(){
		for(int i=0;i< words.size(); i++){
			words[i].drawText();
		}
		for(int i=0;i<spacers.size(); i++){
			
			spacers[i].drawText();
		}
		for(int i=0;i<scaffolding.size(); i++){
			
			scaffolding[i].drawText();
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
		for(int i=0;i<scaffolding.size(); i++){
			
			scaffolding[i].drawBricks();
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
		r += scaffolding.size();
		return r;
	}
	
	Letter * getLetter(int index){
		int counter = 0;
		int wordCounter = 0;
		int letterCounter = 0;
		Letter * r;
		if(index < getNumberLetters()- (spacers.size()+scaffolding.size())){
			while(counter<=index){
				r = &words[wordCounter].letters[letterCounter];
				counter ++;
				letterCounter ++;
				if(letterCounter >= words[wordCounter].getNumberLetters()){
					wordCounter++;
					letterCounter = 0;
				}
			}
		} else if (index < getNumberLetters() - scaffolding.size()) {
			r = &spacers[index-(getNumberLetters()- (spacers.size()+scaffolding.size()))];
		} else {
			r = &scaffolding[index-(getNumberLetters()-scaffolding.size())];
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
	
	void setInertia(float _inertiaX, float _inertiaY, float _inertiaZ){
		inertiaX = _inertiaX;
		inertiaY = _inertiaY;
		inertiaZ = _inertiaZ;
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
	
	void setWorldWrapX(bool _worldWrapX){
		worldWrapX = _worldWrapX;
	}
	
	bool getWorldWrapX(){
		return worldWrapX;
	}
	
	void setWorldWrapY(bool _worldWrapY){
		worldWrapY = _worldWrapY;
	}
	
	bool getWorldWrapY(){
		return worldWrapY;
	}
	
	void setWorldConstrainZ(bool _worldConstrainZ){
		worldConstrainZ = _worldConstrainZ;
	}
	
	bool getWorldConstrainZ(){
		return worldConstrainZ;
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
	
	
	void setupBullet(btDiscreteDynamicsWorld * _dynamicsWorld, bool disableDeactivation){
		if(bodies.size() == 0){
			hasBeenAnimated = true;
			dynamicsWorld = _dynamicsWorld;
			int numLetters = getNumberLetters();
			for(int i=0; i<numLetters; i++){
				Letter * l = getLetter(i);
				btCollisionShape* fallShape = new btBoxShape(btVector3((getLetter(i)->getWidth()-1)/200.0,(getLetter(i)->getHeight()/200.0),getLetter(i)->getDepth()/200.0));
				m_collisionShapes.push_back(fallShape);
				btVector3 pos = btVector3(l->getLoc().x/100.0, l->getLoc().y/100.0, l->getLoc().z/100.0);
				//cout<< l->getLoc().x/100.0 << ", "<<l->getLoc().y/100.0<<endl;
				btDefaultMotionState* fallMotionState = new btDefaultMotionState(btTransform(btQuaternion(0,0,0,0.1),pos));
				btScalar btMass = mass;
				btVector3 fallInertia(inertiaX,inertiaY,inertiaZ);
				fallShape->calculateLocalInertia(btMass,fallInertia);
				btRigidBody::btRigidBodyConstructionInfo fallRigidBodyCI(btMass,fallMotionState,fallShape,fallInertia);
				l->bulletBodie = new btRigidBody(fallRigidBodyCI);
				l->bulletBodie->setFriction(friction);
				l->bulletBodie->setDamping(damping,damping);
				l->bulletBodie->setRestitution(restitution);
				//l->bulletBodie->setCcdMotionThreshold(0.0001);
				if(disableDeactivation){
					l->bulletBodie->setActivationState(DISABLE_DEACTIVATION);
				}
				bodies.push_back(l->bulletBodie);
				dynamicsWorld->addRigidBody(l->bulletBodie);
				l->useBullet = true;
			}
		} else {
			for( int i=0; i < bodies.size(); i++ ) {
				dynamicsWorld->addRigidBody(bodies.at(i));
			}
		}
	}
	
	void clear(){
		clearBullet();
		words.clear();
		spacers.clear();
		lines.clear();
		scaffolding.clear();
		numberLines = 0;
	}
	
	btRigidBody * getBulletBody(int _index) {
		return bodies[_index];
	}
	
	void clearBullet(){
		//cout << "bodies: " << bodies.size() << " collisionshapes: " << m_collisionShapes.size() << endl;
		for (int i=0;i<m_collisionShapes.size();i++) {
			btCollisionShape* shape = m_collisionShapes[i];
			if(shape){
				// crashes
				//delete shape;
			}
		}
		m_collisionShapes.clear();
		for( int i=0; i < bodies.size(); i++ ) {
			dynamicsWorld->removeRigidBody(bodies.at(i));
		}
		bodies.clear();
	}
	
	void restBullet(){
		bulletResting = true;
	}
	
	bool getResting(){
		return bulletResting;
	}
		
	void pauseBullet(){
		//cout << "bodies: " << bodies.size() << " collisionshapes: " << m_collisionShapes.size() << endl;
		for( int i=0; i < bodies.size(); i++ ) {
			dynamicsWorld->removeRigidBody(bodies.at(i));
		}
	}
		
	void updateBullet(bool multiTreaded){
		if(scaffoldingOn == false){
		int scaffoldingSize = scaffolding.size();
		//cout << "getNumberLetters() = " << ofToString( getNumberLetters() ) << " spacers.size() = " << spacers.size() << " scaffolding.size() " << scaffoldingSize << endl;
		Letter * scaffoldingBrick;
		for(int i=0;i<scaffoldingSize;i++){
			cout << i+(getNumberLetters()-scaffoldingSize) << endl;
			if( m_collisionShapes.size() > 0){
				btCollisionShape* shape = m_collisionShapes[i+(getNumberLetters()-scaffoldingSize)];
				m_collisionShapes.remove(shape);
				scaffoldingBrick = getLetter(i+(getNumberLetters()-scaffoldingSize));
				dynamicsWorld->removeRigidBody(scaffoldingBrick->bulletBodie);
			}
		}
		if(bodies.size() > 0){
			bodies.erase( bodies.begin()+( (getNumberLetters()-scaffoldingSize)), bodies.end());
		}
		scaffolding.clear();
		}
		
		if (hasBeenAnimated){
			
	int numLetters = getNumberLetters();
	btTransform trans[numLetters];
	btMotionState * motionState[numLetters];
	btVector3 pos[numLetters];
	btMatrix3x3 basis[numLetters];
	vector<Letter*> l;

	for(int i=0; i<numLetters; i++){
		motionState[i] = bodies[i]->getMotionState();
		motionState[i]->getWorldTransform(trans[i]);
		l.push_back(getLetter(i));
		pos[i] = trans[i].getOrigin();
		basis[i] = trans[i].getBasis();	
	}
#pragma omp num_treads(12)
#pragma omp parallel if(multiTreaded)
			{
#pragma omp for nowait
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
		if(worldWrapX || worldWrapY) {
			if(worldWrapX && pos[i].getX()*100 < -75.0){
				pos[i].setX(0.5+(ofGetHeight()*3.0)/100.0);
			}
			if(worldWrapX && pos[i].getX()*100 > (ofGetHeight()*3.0)+75.0){
				pos[i].setX(-0.5);
			}
			if(worldWrapY && pos[i].getY()*100 < -75.0){
				pos[i].setY(0.5+(ofGetWidth()/3.0)/100.0);
			}
			if(worldWrapY && pos[i].getY()*100 > (ofGetWidth()/3.0)+75.0){
				pos[i].setY(-0.5);
			}
		}
		if(worldConstrainZ) {
			pos[i].setZ(0.0);
		}
		trans[i].setOrigin(pos[i]);
		delete motionState[i];
		motionState[i] = new btDefaultMotionState(trans[i]);
		bodies[i]->setMotionState(motionState[i]);
		bodies[i]->setFriction(friction);
		bodies[i]->setDamping(damping,damping);
		bodies[i]->setRestitution(restitution);
		if(bulletResting){
			if(damping > 0.98) {
				//bodies[i]->clearForces();
				bodies[i]->setCollisionFlags( btCollisionObject::CF_KINEMATIC_OBJECT);
			} else {
				damping *= 1.05;
			}

		}
	}
		}
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
	btScalar mass;
	float inertiaX, inertiaY, inertiaZ;
	TextFontHolder * font;
	bool wordBlocks;
	vector<btRigidBody*> bodies;
	btAlignedObjectArray<btCollisionShape*>	m_collisionShapes;
	btDiscreteDynamicsWorld * dynamicsWorld;
	bool cursor;
	bool worldWrapX;
	bool worldWrapY;
	bool worldConstrainZ;
	bool hasBeenAnimated;
	bool scaffoldingOn;
	bool bulletResting;
};

#endif
#define OF_ADDON_USING_OFXOSC

#include "ofMain.h"
#include "ofAddons.h"

#ifndef _OFX_SHARED_H_
#define _OFX_SHARED_H_

class SharedVariable{
	enum{SHARED_BOOL, SHARED_INT, SHARED_FLOAT, SHARED_STRING, SHARED_RGBA};
public:
	
	int * valueInt;
	float * valueFloat;
	bool * valueBool;
	string * valueString;
	//ofRGBA * valueColor;
	
	
	int oldInt;
	float oldFloat;
	bool oldBool;
	string oldString;
	bool allowChange;
//	ofRGBA oldColor;
	
	string id;
	
	int type;	
	
	SharedVariable(int * val, string name, bool _allowChange = true){
		valueInt = val;
		type = SHARED_INT;
		allowChange = _allowChange;
		id = name;
		oldInt = *val;
	}
	SharedVariable(float * val, string name,bool _allowChange= true){
		valueFloat = val;
		type = SHARED_FLOAT;
		allowChange = _allowChange;
		id = name;
		oldFloat = *val;
	}
	SharedVariable(string * val, string name, bool _allowChange= true){
		valueString= val;
		type = SHARED_STRING;
		allowChange = _allowChange;
		id = name;
		oldString = *val;
	}
	SharedVariable(bool * val, string name,bool _allowChange= true){
		valueBool= val;
		type = SHARED_BOOL;
		allowChange = _allowChange;
		id = name;
		oldBool = *val;
	}	
/*	SharedVariable(ofRGBA * val, string name,bool _allowChange= true){
		valueColor = val;
		type = SHARED_RGBA;
		allowChange = _allowChange;
		id = name;
		oldColor = *val;
	}
*/	
	void sendValue(ofxOscSender * sender){
		ofxOscMessage m;
		switch (type) {
			case SHARED_BOOL:
				m.setAddress( ("/sharedvariable/"+id).c_str());
				m.addIntArg(*valueBool);
				sender->sendMessage( m );
				break;
			case SHARED_FLOAT:
				m.setAddress( ("/sharedvariable/"+id).c_str());
				m.addFloatArg(*valueFloat);
				sender->sendMessage( m );
				break;				
			case SHARED_STRING:
				m.setAddress( ("/sharedvariable/"+id).c_str());
				m.addStringArg((*valueString).c_str());
				sender->sendMessage( m );
				break;
			case SHARED_INT:
				m.setAddress( ("/sharedvariable/"+id).c_str());
				m.addIntArg(*valueInt);
				sender->sendMessage( m );
				break;
	/*		case SHARED_RGBA:
				m.setAddress( ("/sharedvariable/"+id).c_str());
				m.addFloatArg(valueColor->r);
				m.addFloatArg(valueColor->g);
				m.addFloatArg(valueColor->b);
				m.addFloatArg(valueColor->a);
				sender->sendMessage( m );
				break;*/
			default:
				break;
		}		
	}
	
	void update(ofxOscSender * sender){
		switch (type) {
			case SHARED_BOOL:
				if(oldBool != *valueBool){
					sendValue(sender);
				}
				oldBool = *valueBool;
				break;
			case SHARED_FLOAT:
				if(oldFloat != *valueFloat){
					sendValue(sender);
				}
				oldFloat = *valueFloat;
				break;				
			case SHARED_STRING:
				if(oldString != *valueString){
					sendValue(sender);
				}
				oldString = *valueString;
				break;
			case SHARED_INT:
				if(oldInt != *valueInt){
					sendValue(sender);
				}
				oldInt = *valueInt;
				break;
		/*	case SHARED_RGBA:
				if(oldColor.r != valueColor->r || oldColor.g || valueColor->g || oldColor.b != valueColor->b || oldColor.a != valueColor->a  ){
					sendValue(sender, toolName);
				}
				oldColor = *valueColor;
				break;*/
			default:
				break;
		}
		
	}
	
	void handleOsc(ofxOscMessage *m){
		//cout <<  m->getAddress() << "   TYPE: " << m->getArgType(0) << " " << m->getArgTypeName(0) << " " << OFXOSC_TYPE_FLOAT << endl;
		if ( m->getAddress() == "/sharedvariable/"+id ){
			if(type == SHARED_BOOL){
				if(m->getArgType(0) == OFXOSC_TYPE_FLOAT){
					//avioding crashes when isadora sends a boolean as a float
					// cout <<  m->getAddress() << " = " << ((0.5 < m->getArgAsFloat(0))?true:false) << endl;
					*valueBool = ((0.5 < m->getArgAsFloat(0))?true:false);
					oldBool = ((0.5 < m->getArgAsFloat(0))?true:false);
				} else {
					// cout <<  m->getAddress() << " = " <<  m->getArgAsInt32(0) << endl;
					*valueBool = m->getArgAsInt32(0);
					oldBool = m->getArgAsInt32(0);
				}
			} else if(type == SHARED_INT){
				if(m->getArgType(0) == OFXOSC_TYPE_FLOAT){
					//avioding crashes when isadora sends an int as a float
					// cout <<  m->getAddress() << " = " << lrintf(m->getArgAsFloat(0)) << endl;
					*valueInt = lrintf(m->getArgAsFloat(0));
					oldInt = lrintf(m->getArgAsFloat(0));
				} else {
					//cout <<  m->getAddress() << " = " <<  m->getArgAsInt32(0) << endl;
					*valueInt = m->getArgAsInt32(0);
					oldInt = m->getArgAsInt32(0);
				}
			} else if(type == SHARED_STRING){
				//cout <<  m->getAddress() << " = " << m->getArgAsString(0) << endl;
				string str = m->getArgAsString(0);
				// hack for max msp empty strings
				if(str == "bang")
					str = "";
				*valueString = str;
				oldString = str;
			} else if(type == SHARED_FLOAT){
				// cout <<  m->getAddress() << " = " <<  m->getArgAsFloat(0) << endl;
				*valueFloat = ((-0.001 < m->getArgAsFloat(0) && m->getArgAsFloat(0) < 0.001)?0.0:m->getArgAsFloat(0)); //avoiding errros when isadora sends -2.38419e-08 in stead of zero
				oldFloat = ((-0.001 < m->getArgAsFloat(0) && m->getArgAsFloat(0) < 0.001)?0.0:m->getArgAsFloat(0));
			}/*else if(type == SHARED_RGBA){
				valueColor->r = m->getArgAsFloat(0);
				valueColor->g = m->getArgAsFloat(1);
				valueColor->b = m->getArgAsFloat(2);
				valueColor->a = m->getArgAsFloat(3);
				cout<< m->getArgAsFloat(0)<<" "<< m->getArgAsFloat(1)<<" "<< m->getArgAsFloat(2)<<endl;
				oldColor = *valueColor;
			}*/			
		}
	}	
};

#endif
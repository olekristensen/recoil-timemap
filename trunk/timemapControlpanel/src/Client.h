//Handles communication to clients

#define OF_ADDON_USING_OFXOSC

#include "ofMain.h"
#include "ofAddons.h"

#ifndef _OFX_CLIENT_H_
#define _OFX_CLIENT_H_


class Client{
public:
	string ip;
	int port;
	int id;
	ofxOscSender sender;
	static int next_id;
	ofSimpleApp* app;
	Client(){};
	Client(ofSimpleApp* _app, string _ip, int _port){
		sender.setup(_ip.c_str(), _port);
		ip = _ip;
		port = _port;
		id = next_id;
		next_id++;
		app = _app;
	};

};

#endif
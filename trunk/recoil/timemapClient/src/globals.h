/*
 *  globals.h
 *  Recoil TimeMap Screens.openFrameworks
 *
 *  Created by recoil on 25/03/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef _RECOIL_GLOBALS
#define _RECOIL_GLOBALS

#define RENDER_WIDTH 2400
#define RENDER_HEIGHT 600

class Globals { 
public: 
    
	struct { 
		int            width; 
		int            height; 
		float         invWidth; 
		float         invHeight; 
		float         aspectRatio; 
		float         invAspectRatio; 
	} window; 

	Globals() { 
		window.width         = RENDER_WIDTH; 
		window.height         = RENDER_HEIGHT; 
		window.invWidth         = 1.0f/window.width; 
		window.invHeight      = 1.0f/window.height; 
		window.aspectRatio      = window.width / window.height; 
		window.invAspectRatio   = 1.0f/window.aspectRatio; 
	} 
    
}; 

extern Globals globals;

#endif
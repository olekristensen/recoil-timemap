#ifndef RECOIL_TEXTFONT
#define RECOIL_TEXTFONT

#define OF_ADDON_USING_OFSHADER
#define OF_ADDON_USING_OFXUTF8

#include "ofMain.h"
#include "ofAddons.h"

#include <FTGL/ftgl.h>
#include <FTGL/FTGLPolygonFont.h>
//#include <FTGL/FTFont.h>
//#include <FTGL/FTBBox.h>


class TextFontHolder  {
public:	
    
	ofShader shader;
	//TextFontHolder(){}
	TextFontHolder();
	TextFontHolder(string file);
	TextFontHolder(string file, float _fontSize);
		
	void renderString(string text){
		drastring(text, 0, 0);
	}
	
	void renderString(wstring text){
		drastring(text, 0, 0);
	}
	
	float getWidth(string text){
		return stringWidth(text);
	}
	
	float getWidth(wstring text){
		return stringWidth(text);
	}
	
	float getCharSetWidth(wstring character){
		return getWidth(character);
		return 0;
	}
	
	float getCharSetWidth(string character){
		return getWidth(character);
		return 0;
	}
	
	float getHeight(string text){
		return stringHeight(text);
	}
	
	float getHeight(wstring text){
		return stringHeight(text);
	}
	
	float getCalculatedHeight(){
		return getLineHeight() + 2*border;
	}
	
	float getFontSize(){
		return fontSize;
	}
		
	~TextFontHolder();
	
	// 			-- default, non-full char set, anti aliased:
	void 		loadFont(string filename, float fontsize);
	void 		loadFont(string filename, float fontsize, bool _bAntiAliased, bool _bFullCharacterSet);
	
	bool		bLoadedOk;
	bool 		bAntiAlised;
	bool 		bFullCharacterSet;
	
  	float 		getLineHeight();
  	void 		setLineHeight(float height);
	float 		stringWidth(string s);
	float 		stringWidth(wstring s);
	float 		stringHeight(string s);
	float 		stringHeight(wstring s);

	ofRectangle    getStringBoundingBox(string s, float x, float y);
	ofRectangle    getStringBoundingBox(wstring s, float x, float y);
	
	void 		drastring(string s, float x, float y);
	void 		drastring(wstring s, float x, float y);

	int 		nCharacters;
	
	GLuint		*	texNames;		// textures for each character
	
protected:
	
	float 			lineHeight;
	charProps 		* 	cps;			// properties for each character
	
	float				fontSize;
	
	void 			drawChar(int c, float x, float y);
	int 			ofNextPow2(int a);
	float				border, visibleBorder;
	
	FTGLPolygonFont * font;
	

};
#endif
#ifndef RECOIL_TEXTFONT
#define RECOIL_TEXTFONT

#define OF_ADDON_USING_OFSHADER

#include "ofMain.h"
#include "ofAddons.h"

     


class TextFontHolder  {
public:	
    
	ofShader shader;
	//TextFontHolder(){}
	TextFontHolder();
	TextFontHolder(string file);
	TextFontHolder(string file, int _fontSize);
		
	void renderString(string text){
		drawString(text, 0, 0);
		shader.setShaderActive(false);
	}
	
	float getWidth(string text){
		return stringWidth(text);
	}
	
	float getCharSetWidth(int character){
		return cps[(unsigned char) character - NUM_CHARACTER_TO_START].setWidth;
	}
	
	float getHeight(string text){
		return stringHeight(text);
	}

	float getCalculatedHeight(){
		return getLineHeight() + 2*border;
	}
	
	int getFontSize(){
		return fontSize;
	}
		
	~TextFontHolder();
	
	// 			-- default, non-full char set, anti aliased:
	void 		loadFont(string filename, int fontsize);
	void 		loadFont(string filename, int fontsize, bool _bAntiAliased, bool _bFullCharacterSet);
	
	bool		bLoadedOk;
	bool 		bAntiAlised;
	bool 		bFullCharacterSet;
	
  	float 		getLineHeight();
  	void 		setLineHeight(float height);
	float 		stringWidth(string s);
	float 		stringHeight(string s);
		ofRectangle    getStringBoundingBox(string s, float x, float y);
	
	void 		drawString(string s, float x, float y);
	int 		nCharacters;
	
	GLuint		*	texNames;		// textures for each character
	
protected:
	
	float 			lineHeight;
	charProps 		* 	cps;			// properties for each character
	
	int				fontSize;
	
	void 			drawChar(int c, float x, float y);
	int 			ofNextPow2(int a);
	int				border, visibleBorder;
	
	

};
#endif
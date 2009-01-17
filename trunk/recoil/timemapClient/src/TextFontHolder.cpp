#include "TextFontHolder.h"

//--------------------------


#if defined (TARGET_WIN32) || defined (TARGET_LINUX)
#include <ft2build.h>
#include <freetype/freetype.h>
#include <freetype/ftglyph.h>
#include <freetype/ftoutln.h>
#include <freetype/fttrigon.h>
#endif

#if defined (TARGET_OSX)
#include "ft2build.h"
#include "freetype.h"
#include "ftglyph.h"
#include "ftoutln.h"
#include "fttrigon.h"
#endif





//------------------------------------------------------------------
TextFontHolder::TextFontHolder(){	
	bLoadedOk = false;
	fontSize = 0;
}

//------------------------------------------------------------------

TextFontHolder::TextFontHolder(string file, int _fontSize = 20){
	bLoadedOk = false;
	loadFont(file, _fontSize, true, true);
}

//------------------------------------------------------------------
TextFontHolder::~TextFontHolder(){
	
}


//------------------------------------------------------------------
void TextFontHolder::loadFont(string filename, int fontsize){
	loadFont(filename, fontsize, true, true);
}
//------------------------------------------------------------------
void TextFontHolder::loadFont(string filename, int fontsize, bool _bAntiAliased, bool _bFullCharacterSet){
	font = new FTGLPolygonFont(ofToDataPath(filename).c_str());	
  	bLoadedOk = true;	
	font->FaceSize(fontsize);
}

//-----------------------------------------------------------
int TextFontHolder::ofNextPow2 ( int a )
{
	int rval=1;
	while(rval<a) rval<<=1;
	return rval;
}


//-----------------------------------------------------------
void TextFontHolder::setLineHeight(float _newLineHeight) {
	lineHeight = _newLineHeight;
}

//-----------------------------------------------------------
float TextFontHolder::getLineHeight(){
	return lineHeight;
}

//-----------------------------------------------------------
void TextFontHolder::drawChar(int c, float x, float y) {
	
}


//-----------------------------------------------------------
float TextFontHolder::stringWidth(string c) {
    ofRectangle rect = getStringBoundingBox(c, 0,0);
    return rect.width;
}


ofRectangle TextFontHolder::getStringBoundingBox(string c, float x, float y){
	float llx, lly, llz, urx, ury, urz;
	font->BBox(c.c_str(), llx, lly, llz, urx, ury, urz);
	return ofRectangle(llx, lly,  urx-llx, ury-lly);
}



//-----------------------------------------------------------
float TextFontHolder::stringHeight(string c) {
    ofRectangle rect = getStringBoundingBox(c, 0,0);
    return rect.height;
}

//=====================================================================
void TextFontHolder::drawString(string c, float x, float y) {
	glPushMatrix();
	glScaled(1.0, -1.0, 1.0);
	font->Render(c.c_str());
	glPopMatrix();
}




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
	fontSize = 0.0;
}

//------------------------------------------------------------------

TextFontHolder::TextFontHolder(string file, float _fontSize = 20){
	bLoadedOk = false;
	fontSize = _fontSize;
	loadFont(file, _fontSize, true, true);
}

//------------------------------------------------------------------
TextFontHolder::~TextFontHolder(){
	
}


//------------------------------------------------------------------
void TextFontHolder::loadFont(string filename, float _fontsize){
	loadFont(filename, _fontsize, true, true);
}
//------------------------------------------------------------------
void TextFontHolder::loadFont(string filename, float _fontSize, bool _bAntiAliased, bool _bFullCharacterSet){
	font = new FTGLPolygonFont(ofToDataPath(filename).c_str());	
  	bLoadedOk = true;
	fontSize = _fontSize;
	font->FaceSize(_fontSize);
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
float TextFontHolder::stringWidth(string c) {
	return font->Advance(c.c_str());
}

float TextFontHolder::stringWidth(wstring c) {
	return font->Advance(c.c_str());
}


ofRectangle TextFontHolder::getStringBoundingBox(string c, float x, float y){
	wstring utf32result;
	utf8::utf8to32(c.begin(), c.end(), back_inserter(utf32result));
	
	return getStringBoundingBox(utf32result, x, y);
}

ofRectangle TextFontHolder::getStringBoundingBox(wstring c, float x, float y){
	float llx, lly, llz, urx, ury, urz;
	font->BBox(c.c_str(), llx, lly, llz, urx, ury, urz);
	return ofRectangle(llx, lly,  urx-llx, ury-lly);
}


//-----------------------------------------------------------
float TextFontHolder::stringHeight(string c) {
    ofRectangle rect = getStringBoundingBox(c, 0,0);
    return rect.height;
}

float TextFontHolder::stringHeight(wstring c) {
    ofRectangle rect = getStringBoundingBox(c, 0,0);
    return rect.height;
}

//=====================================================================
void TextFontHolder::drastring(string c, float x, float y) {
	
	wstring utf32result;
	utf8::utf8to32(c.begin(), c.end(), back_inserter(utf32result));
	
	drastring(utf32result, x,y);
}

void TextFontHolder::drastring(wstring c, float x, float y) {
	glPushMatrix();
	glScaled(1.0, -1.0, 1.0);
	font->Render(c.c_str());
	glPopMatrix();
}




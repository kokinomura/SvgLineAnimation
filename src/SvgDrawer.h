#pragma once

#include "ofMain.h"
#include "ofxSvg.h"
#include "ofxPath.h"

class SvgDrawer {
public:
    void load(string fileName);
    void update();
    void draw();
    
    void setLength(float length);
    void setSpeed(float speed);
    
    ofRectangle getViewBox();
    
private:
    ofxSVG _svg;
    
    float _length;
    float _speed;
    
    float _beginPosition = 0;
    
    vector<ofxPolyline> _outlines;
    vector<ofColor> _colors;
    vector<float> _widths;
    
    void _setBeginPosition();
};
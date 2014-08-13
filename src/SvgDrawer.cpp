#include "SvgDrawer.h"

void SvgDrawer::load(string fileName) {
    
    _svg.load(fileName);
    
    _outlines.clear();
    _colors.clear();
    _widths.clear();
    
    for (int i = 0; i < _svg.getNumPath(); i++){
        ofxPath p = _svg.getPathAt(i);
        p.setPolyWindingMode(OF_POLY_WINDING_ODD);
        
        // ofPath has only one ofPolyline in ofxSvg
        const vector<ofxPolyline>& lines = p.getOutline();
        if (lines.size() < 1) {
            continue;
        }
        ofxPolyline line = lines[0];
        if (line.size() == 0) {
            continue;
        }
        
        float spacing = 1.0;
        ofxPolyline polyline = line.getResampledAndVerticesBySpacing(spacing);
        
        _outlines.push_back(polyline);
        _colors.push_back(p.getStrokeColor());
        _widths.push_back(p.getStrokeWidth());
    }
    _setBeginPosition();
}

//--------------------------------------------------------
void SvgDrawer::update() {
    float speedScale = 0.005;
    _beginPosition += speedScale * _speed;
}

//--------------------------------------------------------
void SvgDrawer::draw() {
    for (int i = 0; i < (int)_outlines.size(); i++){
        ofxPolyline & line = _outlines[i];
        
        ofSetColor(_colors[i]);
        
        if (_beginPosition > 1.0) {
            _setBeginPosition();
        }
        int beginPoint = _beginPosition * line.size();
        beginPoint = MAX(0, beginPoint);
        
        int endPoint = (_beginPosition+_length) * line.size();
        endPoint = MIN(endPoint, line.size()-1);
        
        line.setStrokeWidth(_widths[i]);
        line.draw(beginPoint, endPoint);
    }
}

//--------------------------------------------------------
void SvgDrawer::setLength(float length) {
    _length = length;
}

//--------------------------------------------------------
void SvgDrawer::setSpeed(float speed) {
    _speed = speed;
}

//--------------------------------------------------------
ofRectangle SvgDrawer::getViewBox() {
    return _svg.getViewBox();
}

#pragma mark - Private Methods

//--------------------------------------------------------
void SvgDrawer::_setBeginPosition() {
    _beginPosition = -_length;
}
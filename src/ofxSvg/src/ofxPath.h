//
//  ofxPath.h
//
//  Created by Koki Nomura on 2014/07/22.
//
//

#pragma once

#include "ofPath.h"
#include "ofPolyline.h"

typedef enum {
    OFXSVG_STROKE_LINECAP_BUTT,
    OFXSVG_STROKE_LINECAP_SQUARE,
    OFXSVG_STROKE_LINECAP_ROUND
} ofxSVGStrokeLinecap;

typedef enum {
    OFXSVG_STROKE_LINEJOIN_MITER,
    OFXSVG_STROKE_LINEJOIN_BEVEL,
    OFXSVG_STROKE_LINEJOIN_ROUND
} ofxSVGStrokeLinejoin;

class ofxPolyline : public ofPolyline {
public:
    
    void draw();
    void draw(int begin, int end);
    
    void setStrokeWidth(float w) {
        width = w;
    }
    float getStrokeWidth() {
        return width;
    }
    void setStrokeLinecap(ofxSVGStrokeLinecap linecap) {
        strokeLinecap = linecap;
    }
    void setStrokeLinejoin(ofxSVGStrokeLinejoin linejoin) {
        strokeLinejoin = linejoin;
    }
    ofxSVGStrokeLinecap getStrokeLinecap(){
        return strokeLinecap;
    }
    ofxSVGStrokeLinejoin getStrokeLinejoin(){
        return strokeLinejoin;
    }
    ofxPolyline getResampledAndVerticesBySpacing(float spacing);

private:
    void createMesh(ofMesh &mesh, int begin, int end);
    void createMeshMiterJoint(ofMesh &mesh, int begin, int end);
    void createMeshBevelJoint(ofMesh &mesh, int begin, int end);    
    void createMeshRoundJoint(ofMesh &mesh, int begin, int end);    
    
    pair<ofVec2f, ofVec2f> jointBegin(ofVec2f p1, ofVec2f p2, float width);
    pair<ofVec2f, ofVec2f> jointEnd(ofVec2f p1, ofVec2f p2, float width);
    
    pair<ofVec2f, ofVec2f> miterJoint(ofVec2f p1, ofVec2f p2, ofVec2f p3, float width);
    pair<vector<ofVec2f>, vector<ofVec2f> > bevelJoint(ofVec2f p1, ofVec2f p2, ofVec2f p3, float width);
    pair<vector<ofVec2f>, vector<ofVec2f> > roundJoint(ofVec2f p1, ofVec2f p2, ofVec2f p3, float width);
    
    ofVec2f intersect(ofVec2f p1, ofVec2f p2, ofVec2f v1, ofVec2f v2);
    
    float width;
    ofxSVGStrokeLinecap strokeLinecap;
    ofxSVGStrokeLinejoin strokeLinejoin;
};

class ofxPath : public ofPath {
public:
    void setStrokeLinecap(ofxSVGStrokeLinecap linecap) {
        strokeLinecap = linecap;
    }
    void setStrokeLinejoin(ofxSVGStrokeLinejoin linejoin) {
        strokeLinejoin = linejoin;
    }
    ofxSVGStrokeLinecap getStrokeLinecap(){
        return strokeLinecap;
    }
    ofxSVGStrokeLinejoin getStrokeLinejoin(){
        return strokeLinejoin;
    }
    vector<ofxPolyline> & getOutline() {
        vector<ofPolyline> &polylines = ofPath::getOutline();
        for (int i=0; i<polylines.size(); i++) {
            ofxPolyline xpolyline;
            xpolyline.addVertices(polylines[i].getVertices());
            xpolyline.setRightVector(polylines[i].getRightVector());
            xpolyline.setStrokeLinecap(strokeLinecap);
            xpolyline.setStrokeLinejoin(strokeLinejoin);
            xpolylines.push_back(xpolyline);
        }
        return xpolylines;
    }
private:
    vector<ofxPolyline> xpolylines;
    ofxSVGStrokeLinecap strokeLinecap;
    ofxSVGStrokeLinejoin strokeLinejoin;
};
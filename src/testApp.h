#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#include "SvgDrawer.h"

class testApp : public ofBaseApp{
    
public:
    void setup();
    void update();
    void draw();
    
    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);
    
    void readSvg(string fileName);
    void exportVideo();
    
    void speedChanged(float & speed);
    void lengthChanged(float & length);

    // SVG
    SvgDrawer drawer;
    vector<string> svgFileNames;
    
    // animations
    const int fps = 30;
    int frame = 0;
    int fileNumber = 0;
    
    int xOffset = 0;
    int yOffset = 0;
    
    // GUI
    ofxPanel gui;
    ofxGuiGroup lineGroup;
    ofxFloatSlider speed;
    ofxFloatSlider length;
    ofxGuiGroup frameGroup;
    ofxFloatSlider duration;
    ofxLabel exportButton;
    bool bShow = true;
    
    // Export
    bool isExporting = false;
    ofImage imageSaver;
    ofPixels pixels;
    ofFbo fbo;
};

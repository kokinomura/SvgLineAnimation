//
//  myButton.h
//  svgExample
//
//  Created by Koki Nomura on 2014/06/20.
//
//

#pragma once

#include "ofxGui.h"

class myButton : public ofxLabel {
public:

    ofEvent<ofMouseEventArgs> pressed;
    
    myButton* setup(string labelName, string label) {
        ofxLabel::setup(labelName, label);
        return this;
    }
    
    bool mousePressed(ofMouseEventArgs &args) {
        if (getShape().inside(args.x, args.y)) {
            ofNotifyEvent(pressed, args, this);
            return true;
        }
    }
};
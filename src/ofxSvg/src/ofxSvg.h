#pragma once

//#include "ofMain.h"
#include "svgtiny.h"
#include "ofxPath.h"
#include "ofTypes.h"

class ofxSVG {
	public: ~ofxSVG();


		float getWidth() const {
			return width;
		}
		float getHeight() const {
			return height;
		}
        ofRectangle getViewBox() const {
            return viewBox;
        }
		void load(string path);
		void draw();

		int getNumPath(){
			return paths.size();
		}
		ofxPath & getPathAt(int n){
			return paths[n];
		}

	private:

		float width, height;
        ofRectangle viewBox;

		vector <ofxPath> paths;

		void setupDiagram(struct svgtiny_diagram * diagram);
		void setupShape(struct svgtiny_shape * shape, ofxPath & path);

};

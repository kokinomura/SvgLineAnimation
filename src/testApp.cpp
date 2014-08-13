#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(fps);
    ofEnableSmoothing();

    // GUI
    speed.addListener(this, &testApp::speedChanged);
    length.addListener(this, &testApp::lengthChanged);
    
    gui.setup();
    
    lineGroup.setup("Line");
    lineGroup.add(speed.setup("Speed", 2.0, 0.1, 5.0));
    lineGroup.add(length.setup("Length", 0.5, 0.0, 1.0));
    gui.add(&lineGroup);
    
    frameGroup.setup("Frame");
    frameGroup.add(duration.setup("Duration [sec]", 1.0, 0.1, 5.0));
    gui.add(&frameGroup);
    
    gui.add(exportButton.setup("Export video", ""));
}


//--------------------------------------------------------------
void testApp::update(){
    if (svgFileNames.size() == 0) {
        return;
    }    
    
    drawer.update();

    frame++;
    float framesPerFile = fps * duration;
    int newFileNumber = int(frame / framesPerFile) % svgFileNames.size();
    
    if (newFileNumber != fileNumber) {
        float time = ofGetElapsedTimef();
        fileNumber = newFileNumber;
        readSvg(svgFileNames[fileNumber]);
        cout << "read      : " << ofGetElapsedTimef()-time << endl;
    }
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(255);
    if (svgFileNames.size() == 0) {
        ofSetColor(0);
        ofDrawBitmapString("Drag and drop svg files here!", 400, 400);
    } else {
        fbo.begin();
        ofClear(255, 255, 255, 0);
        drawer.draw();
        fbo.end();
        
        ofPushMatrix();
        ofSetColor(255);
        fbo.draw(xOffset, yOffset);
        ofPopMatrix();
        
        if (isExporting) {
            if (frame > fps * duration * svgFileNames.size()) {
                isExporting = false;
                return;
            }
            fbo.getTextureReference().readToPixels(pixels);
            imageSaver.setFromPixels(pixels);
            imageSaver.saveImage(ofToString(frame) + ".png");
            imageSaver.draw(xOffset, yOffset);
            return;
        }
    }
    
    if (bShow) {
        gui.draw();
    }
}

//--------------------------------------------------------------
void testApp::readSvg(string fileName) {
    
    drawer.load(fileName);
    
    ofRectangle viewBox = drawer.getViewBox();
    
    if (viewBox.width > ofGetWidth() || viewBox.height > ofGetHeight()) {
        ofSetWindowShape(viewBox.width, viewBox.height);        
    }
    
    // For export
    fbo.allocate(viewBox.width, viewBox.height);
    fbo.begin();
    ofClear(255);
    fbo.end();
    imageSaver.setUseTexture(true);
    imageSaver.allocate(viewBox.width, viewBox.height, OF_IMAGE_COLOR_ALPHA);
    pixels.allocate(viewBox.width, viewBox.height, OF_PIXELS_RGBA);
    
    // Animation
    xOffset = (ofGetWidth() - viewBox.width) / 2;
    yOffset = (ofGetHeight() - viewBox.height) / 2;
}

//--------------------------------------------------------------
void testApp::exportVideo() {
    frame = 0;
    isExporting = true;
}

//--------------------------------------------------------------
void testApp::speedChanged(float &speed) {
    drawer.setSpeed(speed * speed);
}

//--------------------------------------------------------------
void testApp::lengthChanged(float &length) {
    drawer.setLength(length);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
	if (key == 'h') {
        bShow = !bShow;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
	
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if (exportButton.getShape().inside(x, y)) {
        exportVideo();
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
	
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){
	
}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){
	svgFileNames = dragInfo.files;
    readSvg(svgFileNames[0]);
}

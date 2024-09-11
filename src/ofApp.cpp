#include "ofApp.h"
#include "Trees.hpp"
#include <stdio.h>
#include <math.h>

Tree *tree;
CircleTreeDrawer *drawer;
LeafTreeDrawer *leafDrawer;
TreeAnimator *animator;
int frameRate = 120;

ofFbo drawBuffer;

int getRetinaScale() {
    auto window = dynamic_cast<ofAppGLFWWindow*>(ofGetWindowPtr());
    if (window) {
        return window->getPixelScreenCoordScale();
    }
    return 1;  // Default to 1.0 if no retina display
}

int bufferWidth;
int bufferHeight;

//--------------------------------------------------------------
void ofApp::setup(){
    TreeGenerator generator = TreeGenerator(5, 200);
    tree = generator.generateTree();
    
    drawer = new CircleTreeDrawer(tree);
    leafDrawer = new LeafTreeDrawer(tree);
    
    animator = new TreeAnimator(tree);
        
    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree);
    NodeAnimator *nodeAnimator =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v + 1; },
                                           [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                           [](float v, float d) -> float { return 0.1 + cosf(d) * 0.1; }
                                           )
                     );
    
    animatorInstaller.visitAll(nodeAnimator);
    
    ofSetColor(200,200,220,150);
    ofFill();
    
//    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    ofSetCircleResolution(200);
    
    ofSetFrameRate(frameRate);
    
    ofSetBackgroundAuto(false);
    
    int scale = getRetinaScale();
    
    bufferWidth = ofGetWidth() * scale;
    bufferHeight = ofGetHeight() * scale;
    
    drawBuffer.allocate(bufferWidth, bufferHeight);
    
    drawBuffer.begin();
//    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    ofClear(0, 0, 0);
    drawBuffer.end();
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate);
}

//--------------------------------------------------------------
void ofApp::draw(){
    drawBuffer.begin();
//    ofDisableBlendMode();
//    ofClear(0, 0, 0);

    ofSetCircleResolution(200);
//    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    ofTranslate(bufferWidth / 2, bufferHeight / 2);
    
    leafDrawer->visitAll();
//    drawer->visitAll();
    drawBuffer.end();
    
    drawBuffer.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}

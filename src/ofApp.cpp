#include "ofApp.h"
#include "Trees.hpp"
#include <stdio.h>
#include <math.h>

Tree *tree;
CircleTreeDrawer *drawer;
CircleTreeAnimator *animator;
int frameRate = 60;

//--------------------------------------------------------------
void ofApp::setup(){
    TreeGenerator generator = TreeGenerator(5, 200);
    tree = generator.generateTree();
    
    drawer = new CircleTreeDrawer(tree);
    
    animator = new CircleTreeAnimator(tree);
        
    CircleTreeAnimatorInstaller animatorInstaller = CircleTreeAnimatorInstaller(tree);
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
    
    ofSetColor(200,200,220);
    ofFill();
    
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    
    ofSetCircleResolution(200);
    
    ofSetFrameRate(frameRate);
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate);
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    
    drawer->visitAll();
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

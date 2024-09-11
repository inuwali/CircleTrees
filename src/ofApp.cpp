#include "ofApp.h"
#include "Trees.hpp"
#include <stdio.h>

Tree *tree;
CircleTreeDrawer *drawer;
CircleTreeAnimator *animator;

//--------------------------------------------------------------
void ofApp::setup(){
    TreeGenerator generator = TreeGenerator(3, 200);
    tree = generator.generateTree();
    
//    cout << "ofApp::setup() tree ROOT CHILD COUNT: " << tree->root->children.size() << "\n";

    drawer = new CircleTreeDrawer(tree);
    
    animator = new CircleTreeAnimator(tree);
    
//    cout << "Tree: " << tree << "\n";
    
    CircleTreeAnimatorInstaller animatorInstaller = CircleTreeAnimatorInstaller(tree);
    NodeAnimator *nodeAnimator =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v + 0.1; },
                                           nullptr,
                                           nullptr)
                     );
    
    animatorInstaller.visitAll(nodeAnimator);
    
    ofSetColor(200,200,220);
    ofFill();
    
    ofSetCircleResolution(200);    
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / ofGetFrameRate());
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
    
//    cout << "draw() tree: " << drawer->tree << "\n";
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

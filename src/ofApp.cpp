#include "ofApp.h"
#include "Trees.hpp"
#include <stdio.h>
#include <math.h>

Tree *tree;
TreeAnimator *animator;
TreeRenderer *renderer;
int frameRate = 120;

ofFbo drawBuffer;
ofFbo drawBuffer2;

int getRetinaScale() {
    auto window = dynamic_cast<ofAppGLFWWindow*>(ofGetWindowPtr());
    if (window) {
        return window->getPixelScreenCoordScale();
    }
    return 1;  // Default to 1.0 if no retina display
}

int bufferWidth;
int bufferHeight;
int screenScale;
int windowWidth;
int windowHeight;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSetRandomSeed(ofGetSystemTimeMillis());
    
    windowWidth = 1500;
    windowHeight = 1000;
    screenScale = getRetinaScale();
    ofSetWindowShape(windowWidth * screenScale, windowHeight * screenScale);

    TreeGenerator generator = TreeGenerator(6, windowHeight / 8);
    tree = generator.generateTree();
        
    animator = new TreeAnimator(tree);
    
    std::vector<NodeAnimator *> allAnimators = {
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return sin(d/2+v/100) * 90 - 45; },
                                               [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return cos(d/5+v/50) * 30 - 15; },
                                               [](float v, float d) -> float { return 0.3 + sinf(d/2) * 0.1; },
                                               [](float v, float d) -> float { return 0 + cosf(d/30) * 0.3; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return sin(d+v/320) * 180 - 90; },
                                               [](float v, float d) -> float { return 0.5 + sinf(sqrt(d)) * 0.1; },
                                               [](float v, float d) -> float { return 0.2 + cosf(d/10) * 0.3; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 1; },
                                               [](float v, float d) -> float { return 0.5 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.2 + cosf(d/10) * 0.3; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 0.5; },
                                               [](float v, float d) -> float { return 0.5 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.2 + cosf(d/10) * 0.3; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v - 0.7; },
                                               [](float v, float d) -> float { return 0.5 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.2 + cosf(d/10) * 0.3; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v - 0.1; },
                                               nullptr,
                                               nullptr
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 0.1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; },
                                               [](float v, float d) -> float { return 0.1 + sinf(d) * 0.4; }
                                               )
                         )
    };
    
    AnimatorChooser chooser = [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
        int numAnimators = animators.size();
//        if (node->children.empty()) {
//            return animators[2];
//        } else {
//            return animators[ofRandom(3)];
////            return animators[depth % 3];
//        }
        if (depth % 2 == 0) {
            return animators[ofRandom(8)];
        } else {
            return nullptr;
        }
////        return animators[depth % 3];
//        if (depth == 2) {
//            return animators[ofRandom(3)];
//        } else {
//            return animators[depth % 3];
//        }
//        return animators[depth %3 + 3];
//        return animators[3 + ofRandom(3)];
//        return animators[6];
//        return animators[ofRandom(5)];
    };
    
    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree,
                                                                    allAnimators,
                                                                    chooser);

    animatorInstaller.visitAll();
    
    renderer = new TreeRenderer(tree);
    
    ofSetCircleResolution(200);
//    ofEnableBlendMode(OF_BLENDMODE_SCREEN);

    ofSetFrameRate(frameRate);
        
    bufferWidth = ofGetWidth() * screenScale;
    bufferHeight = ofGetHeight() * screenScale;

    drawBuffer.allocate(bufferWidth, bufferHeight);
    drawBuffer.begin();
    ofClear(0, 0, 0);
    drawBuffer.end();
    
    drawBuffer2.allocate(bufferWidth, bufferHeight);
    drawBuffer2.begin();
    ofClear(0, 0, 0);
    drawBuffer2.end();

//    ofSetColor(200,200,220,200);
//        ofSetColor(255, 0, 0, 50);
    ofFill();
    ofBackground(255, 255, 255);

}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
    RenderedTree rendered = renderer->render();
    
    drawBuffer.begin();
    ofTranslate(ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    RenderedTreeDrawer::drawAsPoints(rendered);
    drawBuffer.end();
    drawBuffer.draw(0, 0);
    
    drawBuffer2.begin();
    ofClear(0, 0, 0);
    ofTranslate(3*ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    RenderedTreeDrawer::drawAsLines(rendered);
    drawBuffer2.end();
    drawBuffer2.draw(0, 0);
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

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
int screenScale;
int windowWidth;
int windowHeight;

//--------------------------------------------------------------
void ofApp::setup(){
    ofSeedRandom();
    
    windowWidth = 1500;
    windowHeight = 1000;
    screenScale = getRetinaScale();
    ofSetWindowShape(windowWidth * screenScale, windowHeight * screenScale);

    TreeGenerator generator = TreeGenerator(5, windowHeight / 6);
    tree = generator.generateTree();
    
    drawer = new CircleTreeDrawer(tree);
    leafDrawer = new LeafTreeDrawer(tree);
    
    animator = new TreeAnimator(tree);
        
    NodeAnimator *nodeAnimator1 =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v + 0.1; },
                                           [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                           [](float v, float d) -> float { return 0.1 + cosf(d/2) * 0.1; }
                                           )
                     );

    NodeAnimator *nodeAnimator2 =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v - 0.4; },
                                           [](float v, float d) -> float { return 0.3 + sinf(d/2) * 0.1; },
                                           [](float v, float d) -> float { return 0 + cosf(d*3) * 0.3; }
                                           )
                     );

    NodeAnimator *nodeAnimator3 =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v + 0.5; },
                                           [](float v, float d) -> float { return 0.5 + sinf(sqrt(d)) * 0.1; },
                                           [](float v, float d) -> float { return 0.2 + cosf(d*2) * 0.3; }
                                           )
                     );

    
    AnimatorChooser chooser = [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
//        if (node->children.empty()) {
//            return animators[2];
//        } else {
//            return animators[depth % 3];
//        }
        return animators[ofRandom(3)];
//        return animators[depth % 3];
    };
    
    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree,
                                                                    {nodeAnimator1, nodeAnimator2, nodeAnimator3},
                                                                    chooser);

    animatorInstaller.visitAll();
    
    ofSetColor(200,200,220,200);
    ofFill();
    ofBackground(0, 0, 0);
    
    ofSetCircleResolution(200);
    
    ofSetFrameRate(frameRate);
        
    bufferWidth = ofGetWidth() * screenScale;
    bufferHeight = ofGetHeight() * screenScale;

    drawBuffer.allocate(bufferWidth, bufferHeight);
    drawBuffer.begin();
    ofClear(0, 0, 0);
    drawBuffer.end();
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Leaves 👇🏻
    drawBuffer.begin();
    ofTranslate(ofGetWidth() / 3, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    leafDrawer->visitAll();
    drawBuffer.end();
    
    drawBuffer.draw(0, 0);
    // Leaves ☝🏻
    
    // Circles 👇🏻
    ofTranslate(ofGetWidth() / 6 * 5, ofGetHeight() / 2);
    ofScale(screenScale / 2, screenScale / 2);

    drawer->visitAll();
    // Circles ☝🏻
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

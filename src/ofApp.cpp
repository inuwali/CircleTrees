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

//--------------------------------------------------------------
void ofApp::setup(){
    ofSeedRandom();
    
    screenScale = getRetinaScale();
    ofSetWindowShape(2048 * screenScale, 1024 * screenScale);

    TreeGenerator generator = TreeGenerator(4, 200);
    tree = generator.generateTree();
    
    drawer = new CircleTreeDrawer(tree);
    leafDrawer = new LeafTreeDrawer(tree);
    
    animator = new TreeAnimator(tree);
        
    NodeAnimator *nodeAnimator1 =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v + 0.5; },
                                           [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                           [](float v, float d) -> float { return 0.1 + cosf(d) * 0.1; }
                                           )
                     );

    NodeAnimator *nodeAnimator2 =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v - 1; },
                                           [](float v, float d) -> float { return 0.3 + sinf(d/2) * 0.1; },
                                           [](float v, float d) -> float { return -0.3 + cosf(d*3) * 0.6; }
                                           )
                     );

    NodeAnimator *nodeAnimator3 =
    new NodeAnimator(
                     NodeAnimatorFunctions(nullptr,
                                           nullptr,
                                           [](float v, float d) -> float { return v + 2; },
                                           [](float v, float d) -> float { return 0.5 + sinf(v) * 0.1; },
                                           [](float v, float d) -> float { return 0.2 + cosf(d*10) * 0.3; }
                                           )
                     );

    
    AnimatorChooser chooser = [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
//        if (node->children.empty()) {
//            return animators[2];
//        } else {
//            return animators[depth % 3];
//        }
        return animators[ofRandom(3)];
    };
    
    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree,
                                                                    {nodeAnimator1, nodeAnimator2, nodeAnimator3},
                                                                    chooser);

    animatorInstaller.visitAll();
    
    ofSetColor(200,200,220,200);
    ofFill();
    
    ofSetCircleResolution(200);
    
    ofSetFrameRate(frameRate);
        
    bufferWidth = ofGetWidth() * screenScale;
    bufferHeight = ofGetHeight() * screenScale;

    // COMMENT OUT IF YOU WANT CIRCLES 👇🏻
    drawBuffer.allocate(bufferWidth, bufferHeight);
    
//    ofSetBackgroundAuto(false);
    drawBuffer.begin();
    ofClear(0, 0, 0);
    drawBuffer.end();
    // COMMENT OUT IF YOU WANT CIRCLES ☝🏻
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
    // Leaves 👇🏻
    drawBuffer.begin();
    ofTranslate(ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    leafDrawer->visitAll();
    drawBuffer.end();
    
    drawBuffer.draw(0, 0);
    // Leaves ☝🏻
    
    // Circles 👇🏻
    ofTranslate(ofGetWidth() / 4 * 3, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);

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

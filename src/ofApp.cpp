#include "ofApp.h"
#include "Trees.hpp"
#include <stdio.h>
#include <math.h>
#include <sstream>  // For std::stringstream

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

uint64_t randomSeed;

inline float sawtoothWave(float frequency, float t, double amplitude = 1.0, double phase = 0.0, double offset = 0.0) {
//    float period = 1 / frequency;
//    int periodFrames = (int)(period * frameRate);
//    int frame = (int)t * frameRate;
//    int val = frame % periodFrames;
//    return (float)val/(float)periodFrames;
    
    double period = 1.0 / frequency;
    double timeShift = phase / (2 * PI * frequency); // Convert phase shift from radians to time
    double timeInPeriod = fmod(t + timeShift, period);
    
    // Adjust for negative time values
    if (timeInPeriod < 0)
        timeInPeriod += period;
    
    // Calculate the sawtooth wave value
    double value = (2 * amplitude / period) * timeInPeriod - amplitude + offset;
    
    return value;
};

inline double triangleWave(double t, double frequency = 1.0, double amplitude = 1.0, double phase = 0.0, double offset = 0.0) {
    // Generate the sawtooth wave value
    double sawValue = sawtoothWave(t, frequency, amplitude, phase, 0.0);
    
    // Normalize sawValue to range [-1, 1]
    double normalizedSaw = sawValue / amplitude;
    
    // Calculate the triangle wave value
    double triangleValue = (2 * amplitude) * (1.0 - fabs(normalizedSaw)) - amplitude + offset;
    
//    cout << triangleValue << "\n";
    
    return triangleValue;
}

float diagWave(float frequency, float t, double amplitude = 1.0, double phase = 0.0, double offset = 0.0) {
    return fabs(sawtoothWave(frequency, t, amplitude, phase, offset) - 0.5) ;
}

//--------------------------------------------------------------
void ofApp::setup(){
    randomSeed = ofGetSystemTimeMillis();
    ofSetRandomSeed(randomSeed);
    
    cout << "SEED: " << randomSeed << "\n";
    
    windowWidth = 2000;
    windowHeight = 1000;
    screenScale = getRetinaScale();
    ofSetWindowShape(windowWidth * screenScale, windowHeight * screenScale);

    TreeGenerator generator = TreeGenerator(5, windowHeight / 8);
    tree = generator.generateTree();
        
    animator = new TreeAnimator(tree);
    
    std::vector<NodeAnimator *> allAnimators = {
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 1; },
//                                               nullptr,
//                                               [](float v, float d) -> float { return ; },
                                               nullptr,
                                               [](float v, float d) -> float { return 0.3 + triangleWave(d*1.1, 1) * 0.4; }
//                                               [](float v, float d) -> float { return 0.3 + sin(d * 2*PI) * 0.4; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + (6*sin(4*d/10) + 14*cos(6*sqrt(4*d/10))) * 0.1; },
                                               [](float v, float d) -> float { return 0.4 + (6*sin(4*d/15) + 14*cos(6*sqrt(4*d/15))) * 0.05; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return (6*sin(4*d) + 14*cos(6*sqrt(4*d))) * 10; },
                                               [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + abs(sin(d)); },
                                               [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; }
                                               )
                         ),
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
//        if (depth % 2 == 0) {
//            return animators[ofRandom(8)];
//        } else {
//            return animators[3];
//        }
////        return animators[depth % 3];
//        if (depth == 2) {
//            return animators[ofRandom(3)];
//        } else {
//            return animators[depth % 3];
//        }
//        return animators[depth %3 + ofRandom(3)];
//        return animators[3 + ofRandom(3)];
//        return animators[6];
//        return animators[ofRandom(2) * 3];
//        return animators[0];
        return animators[ofRandom(numAnimators)];
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
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    ofTranslate(ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    RenderedTreeDrawer::drawAsPoints(rendered);
    drawBuffer.end();
    
    drawBuffer.draw(0, 0);
    
    drawBuffer2.begin();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
//    ofClear(0, 0, 0);
    ofTranslate(3*ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
//    RenderedTreeDrawer::drawAsFatPoints(rendered);
    RenderedTreeDrawer::drawAsPoints(rendered);
//    RenderedTreeDrawer::drawAsCircles(rendered);
//    RenderedTreeDrawer::drawAsLines(rendered);
    drawBuffer2.end();
    
    drawBuffer2.draw(0, 0);
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
    std::stringstream ss;  // Create a stringstream object
    
    // Use the << operator to concatenate values into the stringstream
    ss << "/Users/owen/Screenshots/openFrameworks/screenshot-" << randomSeed << ".png";
    
    // Convert the stringstream to a std::string
    std::string filename = ss.str();

    if (key == 's') {
        ofImage screenImage;
        screenImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        screenImage.save(filename);  // Save the screenshot
        ofLog() << "Screenshot saved!";
    }
    if (key == 'l') {
        ofPixels pixels;
        drawBuffer.readToPixels(pixels);
        ofImage image;
        image.setFromPixels(pixels);
        image.save("/Users/owen/Screenshots/openFrameworks/screenshot.png");
//        ofImage screenImage;
//        screenImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
//        screenImage.save("/Users/owen/Desktop/screenshot.png");  // Save the screenshot
        ofLog() << "Screenshot saved!";
    }
    if (key == 'r') {
        ofImage screenImage;
        screenImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        screenImage.save("/Users/owen/Desktop/screenshot.png");  // Save the screenshot
        ofLog() << "Screenshot saved!";
    }
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

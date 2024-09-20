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

std::vector<ColorChooser> colorChoosers;
std::vector<ColorChooser> legacyColorChoosers;
std::vector<BinaryChooser> drawChoosers;

uint64_t randomSeed;

int screenshotCount = 0;

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
//    ofSetRandomSeed(334659288);

    cout << "SEED: " << randomSeed << "\n";
    
    windowWidth = 2000;
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
                                               [](float v, float d) -> float { return v + 1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; },
                                               [](float v, float d) -> float { return 0.3 + sawtoothWave(d*1.1, 1) * 0.4; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 1; },
                                               nullptr,
                                               [](float v, float d) -> float { return 0.3 + triangleWave(d*1.1, 1) * 0.4; }
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
    
    std::vector<NodeAnimator *> legacyAnimators = {
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 0.5; },
                                               [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d) * 0.1; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v - 1; },
                                               [](float v, float d) -> float { return 0.3 + sinf(d/2) * 0.1; },
                                               [](float v, float d) -> float { return -0.3 + cosf(d*3) * 0.6; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 2; },
                                               [](float v, float d) -> float { return 0.5 + sinf(v) * 0.1; },
                                               [](float v, float d) -> float { return 0.2 + cosf(d*10) * 0.3; }
                                               )
                         ),
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 2.5; },
                                               [](float v, float d) -> float { return 0.5 + sinf(d*2) * 0.4; },
                                               [](float v, float d) -> float { return 0.2 + cosf(d*5) * 0.4; }
                                               )
                         )
    };
    
    drawChoosers = {
        [](RenderedTreeNode node) { return true; },
        [](RenderedTreeNode node) { return node.maxBranchDepth == node.depth; }, // Just leaves
        [](RenderedTreeNode node) {
            int distFromLeaf = node.maxBranchDepth - node.depth;
            return distFromLeaf % 3 == 0;
        },
        [](RenderedTreeNode node) {
            int distFromLeaf = node.maxBranchDepth - node.depth;
            return distFromLeaf < 3;
        }
    };

    AnimatorChooser chooser = [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
        int numAnimators = animators.size();
//        if (node->children.empty()) {
//            return animators[2];
//        } else {
//            return animators[ofRandom(3)];
////            return animators[depth % 3];
//        }
//        if (depth % 2 != 0) {
//            return animators[4];
//        } else {
//            return animators[9];
//        }
////        return animators[depth % 3];
//        if (depth == 3) {
//            return animators[ofRandom(2)];
//        } else {
//            return animators[(depth+2) % 4];
//        }
//        return animators[depth %3 + ofRandom(3)];
//        return animators[3 + ofRandom(3)];
//        return animators[6];
//        return animators[ofRandom(2) * 3];
//        return animators[0];
        return animators[ofRandom(numAnimators)];
//        return animators[9 + ofRandom(4)];
//        return animators[10];
    };
    
    AnimatorChooser legacyChooser = [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
        int numAnimators = animators.size();
//        if (node->children.empty()) {
//            return animators[3];
//        } else {
//            return animators[depth % numAnimators];
//        }
//        return animators[abs(numAnimators - depth) % numAnimators];
        return animators[depth % numAnimators];
//        return animators[2 + (depth + 1) % 2];
    };
    
//    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree,
//                                                                    legacyAnimators,
//                                                                    legacyChooser);
    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree,
                                                                    allAnimators,
                                                                    chooser);

    animatorInstaller.visitAll();

    colorChoosers = {
        [](RenderedTreeNode node) -> ofColor { return node.color; },
        [](RenderedTreeNode node) -> ofColor {
            if (node.maxBranchDepth - node.depth == 0) {
                return ofColor::fromHsb(150, 240, 230, 100);
            } else if (node.maxBranchDepth - node.depth == 1) {
                return ofColor::fromHsb(170, 230, 250, 150);
            } else if (node.maxBranchDepth - node.depth == 2) {
                return ofColor::fromHsb(190, 200, 200, 200);
            } else {
                return ofColor::fromHsb(25, 255, 240, 255);
            }
        },
        [](RenderedTreeNode node) -> ofColor {
            if (node.maxBranchDepth - node.depth == 0) {
                return ofColor::fromHsb(90, 240, 120, 140);
            } else if (node.maxBranchDepth - node.depth == 1) {
                return ofColor::fromHsb(200, 230, 150, 150);
            } else if (node.maxBranchDepth - node.depth == 2) {
                return ofColor::fromHsb(220, 200, 170, 200);
            } else {
                return ofColor::fromHsb(240, 255, 190, 170);
            }
        },
        [](RenderedTreeNode node) -> ofColor {
            if (node.maxBranchDepth - node.depth == 0) {
                return ofColor::fromHsb(90, 240, 230, 200);
            } else {
                return ofColor::fromHsb(0, 0, 0, 0);
            }
        },
        [](RenderedTreeNode node) -> ofColor {
            int distFromLeaf = node.maxBranchDepth - node.depth;
            if (distFromLeaf < 2) {
                return ofColor::fromHsb(50, 100, 230, 230);
            } else {
                return ofColor::fromHsb(45, 60, 255, 200);
            }
        }
    };
    
    legacyColorChoosers = {
        [](RenderedTreeNode node) -> ofColor {
            int maxDepth = node.maxBranchDepth;
            int currentDepth = node.depth;
            if (maxDepth - currentDepth == 0) {
                return ofColor(255, 200, 200, 200);
            } else if (maxDepth - currentDepth == 1) {
                return ofColor(255, 200, 0, 220);
            } else if (maxDepth - currentDepth == 2) {
                return ofColor(255, 0, 0, 240);
            } else {
                return ofColor(255, 120, 100);
            }
        }
    };
    
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
    ofBackground(0, 0, 0);
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
    RenderedTree rendered = renderer->render();
    RenderedTreeDrawer drawer1 = RenderedTreeDrawer(rendered, legacyColorChoosers[0], drawChoosers[0]);
    RenderedTreeDrawer drawer2 = RenderedTreeDrawer(rendered, colorChoosers[4], drawChoosers[1]);

    drawBuffer.begin();
    ofEnableBlendMode(OF_BLENDMODE_SCREEN);
    ofTranslate(ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    drawer1.drawAsPoints(rendered);
    drawBuffer.end();
    
    drawBuffer.draw(0, 0);
    
    drawBuffer2.begin();
    ofEnableBlendMode(OF_BLENDMODE_ADD);
//    ofClear(0, 0, 0);
    ofTranslate(3*ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
//    drawer2.drawAsFatPoints(rendered);
    drawer2.drawAsPoints(rendered);
//    drawer2.drawAsCircles(rendered);
//    drawer2.drawAsLines(rendered);
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
    ss << "/Users/owen/Screenshots/openFrameworks/screenshot-" << randomSeed;
    if (screenshotCount > 0) {
        ss << "-" << screenshotCount;
    }
    ss << ".png";
    
    screenshotCount += 1;
    
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

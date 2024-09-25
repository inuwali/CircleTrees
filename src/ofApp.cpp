#include "ofApp.h"
#include "ofJson.h"
#include "Trees.hpp"
#include "Parameters.hpp"
#include <stdio.h>
#include <math.h>
#include <sstream>  // For std::stringstream

uint64_t fileToLoad = 0;

TreesParameters setupParameters() {
    TreesParameters result = TreesParameters();
    
    result.treeDepth = 5;
    result.animatorChooserIndex = 0;
    
    TreeRenderParameters renderParams1 = TreeRenderParameters();
    renderParams1.drawChooserIndex = 0;
    renderParams1.colorChooserIndex = 1;
    renderParams1.blendMode = OF_BLENDMODE_SCREEN;
    TreeRenderParameters renderParams2 = TreeRenderParameters();
    renderParams2.drawChooserIndex = 2;
    renderParams2.colorChooserIndex = 2;
    renderParams2.blendMode = OF_BLENDMODE_SCREEN;
    
    result.renderParameters1 = renderParams1;
    result.renderParameters2 = renderParams2;
    
    HSBFloats bg = HSBFloats();
    bg.hue = 36;
    bg.saturation = 20;
    bg.brightness = 235;
    bg.alpha = 255;
    result.backgroundColor = bg;
    
    return result;
}

Tree *tree;
TreeAnimator *animator;
TreeRenderer *renderer;
int frameRate = 120;

TreesParameters params;

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
std::vector<BinaryChooser> drawChoosers;
std::vector<AnimatorChooser> animatorChoosers;

uint64_t randomSeed;

int screenshotCount = 0;

inline float sawtoothWave(float frequency, float t, double amplitude = 1.0, double phase = 0.0, double offset = 0.0) {
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

int randInt(int max) {
    return of::random::uniform(0, max);
}

//--------------------------------------------------------------
void ofApp::setup() {
    params = TreesParameters();
    
    if (fileToLoad > 0) {
        std::stringstream ss;  // Create a stringstream object
        
        ss << "/Users/owen/Programming/OpenFrameworks/CircleTrees/Artifacts/" << fileToLoad << "-params.json" ;
        std::string paramsJsonFilename = ss.str();
        
        params = TreesParameters::fromFile(paramsJsonFilename);
    }
    
    if (params.randomSeed > 0) {
        randomSeed = params.randomSeed;
    } else {
        randomSeed = params.timestamp;
        params.randomSeed = randomSeed;
    }
    
    of::random::seed(randomSeed);

    if (fileToLoad == 0) {
        params = setupParameters();
    }

    windowWidth = 2000;
    windowHeight = 1000;
    screenScale = getRetinaScale();
    ofSetWindowShape(windowWidth * screenScale, windowHeight * screenScale);

    TreeGenerator generator = TreeGenerator(params.treeDepth, windowHeight / 8);
    tree = generator.generateTree();
        
    animator = new TreeAnimator(tree);
    
    std::vector<NodeAnimator *> allAnimators = {
        // 0-1: sawtooth and square
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
        // 2-3: some sqrt stuff
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
        // 4: abs
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + abs(sin(d)); },
                                               [](float v, float d) -> float { return 0.4 + sinf(d) * 0.1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; }
                                               )
                         ),
        // 5-7: including v in the trig functions; manifests as pendulum effect
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
        // 8-10: Pretty standard functions
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
        // 11: Only simple counterclockwise rotation
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v - 0.1; },
                                               nullptr,
                                               nullptr
                                               )
                         ),
        // 12: Pretty standard function
        new NodeAnimator(
                         NodeAnimatorFunctions(nullptr,
                                               nullptr,
                                               [](float v, float d) -> float { return v + 0.1; },
                                               [](float v, float d) -> float { return 0.1 + cosf(d/20) * 0.1; },
                                               [](float v, float d) -> float { return 0.1 + sinf(d) * 0.4; }
                                               )
                         ),
        // 12-15: LEGACY animators below; pretty standard stuff.
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

    animatorChoosers = {
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            int numAnimators = animators.size();
            if (node->children.empty()) {
                return animators[4];
            } else {
                return animators[depth % 3 + 3];
            }
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            int numAnimators = animators.size();
            if (node->children.empty()) {
                return animators[4];
            } else {
                return animators[randInt(3)];
            }
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            if (depth % 2 != 0) {
                return animators[4];
            } else {
                return animators[9];
            }
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[depth % 3];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            if (depth == 3) {
                return animators[randInt(2)];
            } else {
                return animators[(depth+2) % 4];
            }
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[depth % 3 + randInt(3)];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[3 + randInt(3)];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[6];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[randInt(2) * 3];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[(randInt(3)+1) * 2];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            int numAnimators = animators.size();
            return animators[randInt(numAnimators)];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[9 + randInt(4)];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[10];
        },
        // LEGACY choosers below
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            int numAnimators = animators.size();
            return animators[depth % numAnimators];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            int numAnimators = animators.size();
            if (node->children.empty()) {
                return animators[3];
            } else {
                return animators[depth % numAnimators];
            }
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[2 + (depth + 1) % 2];
        },
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            int numAnimators = animators.size();
            return animators[abs(numAnimators - depth) % numAnimators];
        }
    };
        
    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree,
                                                                    allAnimators,
                                                                    animatorChoosers[params.animatorChooserIndex]);

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
        },
        // LEGACY original(ish) chooser below
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
    ofBackground(ofColor::fromHsb(params.backgroundColor.hue, params.backgroundColor.saturation, params.backgroundColor.brightness, params.backgroundColor.alpha));
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
    RenderedTree rendered = renderer->render();
    RenderedTreeDrawer drawer1 = RenderedTreeDrawer(rendered, colorChoosers[params.renderParameters1.colorChooserIndex], drawChoosers[params.renderParameters1.drawChooserIndex]);
    RenderedTreeDrawer drawer2 = RenderedTreeDrawer(rendered, colorChoosers[params.renderParameters2.colorChooserIndex], drawChoosers[params.renderParameters2.drawChooserIndex]);

    drawBuffer.begin();
    ofEnableBlendMode(params.renderParameters1.blendMode);
    ofTranslate(ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    drawer1.drawAsPoints(rendered);
    drawBuffer.end();
    
    drawBuffer.draw(0, 0);
    
    drawBuffer2.begin();
    ofEnableBlendMode(params.renderParameters2.blendMode);
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
    ss << "/Users/owen/Programming/OpenFrameworks/CircleTrees/Artifacts/" << params.timestamp << "-screenshot" ;
    if (screenshotCount > 0) {
        ss << "-" << screenshotCount;
    }
    ss << ".png";
        
    // Convert the stringstream to a std::string
    std::string screenshotFilename = ss.str();
    
    std::stringstream ss2;  // Create a stringstream object

    ss2 << "/Users/owen/Programming/OpenFrameworks/CircleTrees/Artifacts/" << params.timestamp << "-params.json" ;
    std::string paramsJsonFilename = ss2.str();
    
    if (key == 's') {
        if (screenshotCount == 0) {
            ofSavePrettyJson(paramsJsonFilename, params.jsonRepresentation());
        }

        ofImage screenImage;
        screenImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
        screenImage.save(screenshotFilename);  // Save the screenshot
        ofLog() << "Screenshot saved!";
        
        screenshotCount += 1;
    }
//    if (key == 'l') {
//        ofPixels pixels;
//        drawBuffer.readToPixels(pixels);
//        ofImage image;
//        image.setFromPixels(pixels);
//        image.save("/Users/owen/Screenshots/openFrameworks/screenshot.png");
////        ofImage screenImage;
////        screenImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
////        screenImage.save("/Users/owen/Desktop/screenshot.png");  // Save the screenshot
//        ofLog() << "Screenshot saved!";
//    }
//    if (key == 'r') {
//        ofImage screenImage;
//        screenImage.grabScreen(0, 0, ofGetWidth(), ofGetHeight());
//        screenImage.save("/Users/owen/Desktop/screenshot.png");  // Save the screenshot
//        ofLog() << "Screenshot saved!";
//    }
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

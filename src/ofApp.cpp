#include "ofApp.h"
#include "ofJson.h"
#include "Trees.hpp"
#include "Parameters.hpp"
#include <stdio.h>
#include <math.h>
#include <sstream>  // For std::stringstream

#include "ofxImGui.h"

// 0-1: sawtooth and square
// 2-3: some sqrt stuff
// 4: abs
// 5-7: including v in the trig functions; manifests as pendulum effect
// 8-10: Pretty standard functions
// 11: Only simple counterclockwise rotation
// 12: Pretty standard function
// 12-15: LEGACY animators below; pretty standard stuff.

typedef enum {
    POINTS,
    LINES,
    CIRCLES,
    DOTS,
    SQUARES
} DrawStyle;

uint64_t fileToLoad = 0;

int bufferWidth;
int bufferHeight;
int screenScale;
int windowWidth = 1000;
int windowHeight = 1000;
int numTrees = 1;

DrawStyle drawStyle = POINTS;

bool running = false;
uint64_t frameNum = 0;

TreesParameters setupParameters() {
    TreesParameters result = TreesParameters();
    
    result.treeDepth = 4;
    result.animatorChooserIndex = 1;
    
    TreeRenderParameters renderParams1 = TreeRenderParameters();
    renderParams1.drawChooserIndex = 0;
    renderParams1.colorChooserIndex = 7;
    renderParams1.blendMode = OF_BLENDMODE_DISABLED;
    
    TreeRenderParameters renderParams2 = TreeRenderParameters();
    renderParams2.drawChooserIndex = 0;
    renderParams2.colorChooserIndex = 6;
    renderParams2.blendMode = OF_BLENDMODE_DISABLED;
    
    result.renderParameters1 = renderParams1;
    result.renderParameters2 = renderParams2;
    
    HSBFloats bg = HSBFloats();
    bg.hue = 36;
    bg.saturation = 0;
    bg.brightness = 255;
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

ofParameter<int> param1;
ofxImGui::Gui gui;

int getRetinaScale() {
    auto window = dynamic_cast<ofAppGLFWWindow*>(ofGetWindowPtr());
    if (window) {
        return window->getPixelScreenCoordScale();
    }
    return 1;  // Default to 1.0 if no retina display
}

uint64_t randomSeed;

int screenshotCount = 0;

//--------------------------------------------------------------
void ofApp::setup() {
    reset();
    
    // Dear ImGui
    gui.setup();
}

void ofApp::reset() {
    frameNum = 0;
    
    params = TreesParameters();
    
    if (fileToLoad > 0) {
        std::stringstream ss;  // Create a stringstream object
        
        ss << "/Users/owen/Programming/OpenFrameworks/CircleTrees/Artifacts/" << fileToLoad << "-params.json" ;
        std::string paramsJsonFilename = ss.str();
        
        ofFile jsonFile(paramsJsonFilename);
        if (jsonFile.exists()) {
            params = TreesParameters::fromFile(paramsJsonFilename);
        } else {
            fileToLoad = 0;
        }
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
    
    //    windowWidth = 2000;
    windowWidth = 1000;
    //    windowHeight = 1000;
    screenScale = getRetinaScale();
    ofSetWindowShape(windowWidth * screenScale, windowHeight * screenScale);
    
    TreeGenerator generator = TreeGenerator(params.treeDepth, windowHeight / 8);
    tree = generator.generateTree();
    
    animator = new TreeAnimator(tree);
    
    TreeAnimatorInstaller animatorInstaller = TreeAnimatorInstaller(tree,
                                                                    legacyRenderObjects.animators,
                                                                    legacyRenderObjects.animatorChoosers[params.animatorChooserIndex]);
    
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
    
    if (numTrees > 1) {
        drawBuffer2.allocate(bufferWidth, bufferHeight);
        drawBuffer2.begin();
        ofClear(0, 0, 0);
        drawBuffer2.end();
    }
    
    //    ofSetColor(200,200,220,200);
    //        ofSetColor(255, 0, 0, 50);
    ofFill();
    ofBackground(ofColor::fromHsb(params.backgroundColor.hue, params.backgroundColor.saturation, params.backgroundColor.brightness, params.backgroundColor.alpha));
}

//--------------------------------------------------------------
void ofApp::update(){
    if (running) {
        animator->visitAll(frameNum / (float)frameRate, true);
        frameNum += 1;
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    gui.begin();
    
    static uint64_t inputFileToLoad = 0;
    ImGui::Begin("ofxImGui example-simple");
    bool pressed = ImGui::Button(running ? "Pause" : "Resume");
    bool doReset = ImGui::Button("Reset");
//    bool newFileToLoad = ImGui::InputInt("File Number", &inputFileToLoad);
    bool newFileToLoad = ImGui::InputScalar("File Number", ImGuiDataType_U64, &inputFileToLoad);
    ImGui::End();
    
//    ImGui::ShowDemoWindow();
    
    gui.end();
    
    if (newFileToLoad) {
        fileToLoad = inputFileToLoad;
    }
    
    if (doReset) {
        reset();
        return;
    }
    
    if (pressed) {
        running = !running;
    }

    RenderedTree rendered = renderer->render();
    RenderedTreeDrawer drawer1 = RenderedTreeDrawer(rendered, legacyRenderObjects.colorSchemes[params.renderParameters1.colorSchemeIndex], legacyRenderObjects.colorChoosers[params.renderParameters1.colorChooserIndex], legacyRenderObjects.drawChoosers[params.renderParameters1.drawChooserIndex]);
    RenderedTreeDrawer drawer2 = RenderedTreeDrawer(rendered, legacyRenderObjects.colorSchemes[params.renderParameters2.colorSchemeIndex], legacyRenderObjects.colorChoosers[params.renderParameters2.colorChooserIndex], legacyRenderObjects.drawChoosers[params.renderParameters2.drawChooserIndex]);

    { // Buffer 1
        drawBuffer.begin();
        ofEnableBlendMode(params.renderParameters1.blendMode);
        if (numTrees > 1) {
            ofEnableBlendMode(params.renderParameters2.blendMode);
            ofClear(0, 0, 0);
            ofTranslate(ofGetWidth() / 4, ofGetHeight() / 2);
        } else {
            ofTranslate(ofGetWidth() / 2, ofGetHeight() / 2);
        }
        ofScale(screenScale, screenScale);
        
        switch (drawStyle) {
            case POINTS:
                drawer1.drawAsPoints(rendered);
                break;
            case LINES:
                ofClear(0, 0, 0);
                drawer1.drawAsLines(rendered);
                break;
            case CIRCLES:
                ofClear(0, 0, 0);
                drawer1.drawAsCircles(rendered);
                break;
            case DOTS:
                ofClear(0, 0, 0);
                drawer1.drawAsFatPoints(rendered);
                break;
            case SQUARES:
                ofClear(0, 0, 0);
                drawer1.drawAsSquares(rendered);
                break;
        }
        drawBuffer.end();
    }
    
    drawBuffer.draw(0, 0);
    
    if (numTrees > 1) {
        drawBuffer2.begin();
        ofEnableBlendMode(params.renderParameters2.blendMode);
        //    ofClear(0, 0, 0);
        ofTranslate(3*ofGetWidth() / 4, ofGetHeight() / 2);
        //    ofTranslate(ofGetWidth() / 4 + 400, ofGetHeight() / 2);
        ofScale(screenScale, screenScale);
        //    drawer2.drawAsFatPoints(rendered);
        drawer2.drawAsPoints(rendered);
        //    drawer2.drawAsCircles(rendered);
        //    drawer2.drawAsLines(rendered);
        drawBuffer2.end();
        
        drawBuffer2.draw(0, 0);
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    gui.exit();
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

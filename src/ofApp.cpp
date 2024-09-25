#include "ofApp.h"
#include "ofJson.h"
#include "Trees.hpp"
#include <stdio.h>
#include <math.h>
#include <sstream>  // For std::stringstream

struct HSBFloats {
    float hue;
    float saturation;
    float brightness;
    float alpha;
    
    HSBFloats(): hue(255), saturation(255), brightness(255), alpha(255) {}
    
    static HSBFloats fromJson(ofJson json) {
        HSBFloats result = HSBFloats();
        
        try {
            result.hue = json["hue"];
        } catch (const ofJson::type_error& e) { }
        try {
            result.saturation = json["saturation"];
        } catch (const ofJson::type_error& e) { }
        try {
            result.brightness = json["brightness"];
        } catch (const ofJson::type_error& e) { }
        try {
            result.alpha = json["alpha"];
        } catch (const ofJson::type_error& e) { }

        return result;
    }
    
    ofJson jsonRepresentation() {
        ofJson json;
        
        json = {{"hue", hue}};
        json += {"saturation", saturation};
        json += {"brightness", brightness};
        json += {"alpha", alpha};
        
        return json;
    }
};

struct TreeRenderParameters {
    int drawChooserIndex;
    int colorChooserIndex;
    ofBlendMode blendMode;
    
    TreeRenderParameters():
    drawChooserIndex(0),
    colorChooserIndex(0),
    blendMode(OF_BLENDMODE_DISABLED) {}
    
    TreeRenderParameters(std::string filePath):
    drawChooserIndex(0),
    colorChooserIndex(0),
    blendMode(OF_BLENDMODE_DISABLED) {}
    
    static TreeRenderParameters fromJson(ofJson json) {
        TreeRenderParameters params = TreeRenderParameters();
        
        try {
            params.drawChooserIndex = json["drawChooserIndex"];
        } catch (const ofJson::type_error& e) { }
        try {
            params.colorChooserIndex = json["colorChooserIndex"];
        } catch (const ofJson::type_error& e) { }
        try {
            params.blendMode = json["blendMode"];
        } catch (const ofJson::type_error& e) { }
        
        return params;
    }
    
    ofJson jsonRepresentation() {
        ofJson json;
        
        json = {{"drawChooserIndex", drawChooserIndex}};
        json += {"colorChooserIndex", colorChooserIndex};
        json += {"blendMode", blendMode};
        
        return json;
    }
};


struct TreesParameters {
    uint64_t randomSeed;
    int treeDepth;
    int animatorChooserIndex;
    TreeRenderParameters renderParameters1;
    TreeRenderParameters renderParameters2;
    HSBFloats backgroundColor;
    
    TreesParameters():
    randomSeed(0),
    treeDepth(3),
    animatorChooserIndex(0),
    renderParameters1(TreeRenderParameters()),
    renderParameters2(TreeRenderParameters()),
    backgroundColor(HSBFloats()) {}
    
    TreesParameters(std::string filePath):
    randomSeed(0),
    treeDepth(3),
    animatorChooserIndex(0),
    renderParameters1(TreeRenderParameters()),
    renderParameters2(TreeRenderParameters()),
    backgroundColor(HSBFloats()) {
    }
    
    static TreesParameters fromFile(std::string filePath) {
        TreesParameters params = TreesParameters();
        
        ofJson json = ofLoadJson(filePath);
        
        try {
            params.randomSeed = json["randomSeed"];
        } catch (const ofJson::type_error& e) { }
        try {
            params.treeDepth = json["treeDepth"];
        } catch (const ofJson::type_error& e) { }
        try {
            params.animatorChooserIndex = json["animatorChooserIndex"];
        } catch (const ofJson::type_error& e) { }
        try {
//            ofJson rps = json["renderParameters"];
//            params.renderParameters1 = TreeRenderParameters::fromJson(rps.at(0));
            params.renderParameters1 = TreeRenderParameters::fromJson(json["renderParameters1"]);
            params.renderParameters2 = TreeRenderParameters::fromJson(json["renderParameters2"]);
//            params.renderParameters = json["renderParameters"];
//            for (auto& rp : json["renderParameters"].items()) {
//                params.renderParameters.push_back(rp.value().template get<TreeRenderParameters>());
//            }
        } catch (const ofJson::type_error& e) { }
        try {
            params.backgroundColor = HSBFloats::fromJson(json["backgroundColor"]);
        } catch (const ofJson::type_error& e) { }
        
        return params;
    }
    
    ofJson jsonRepresentation() {
        ofJson json;
        
        json = {{"randomSeed", randomSeed}};
        json += {"treeDepth", treeDepth};
        json += {"animatorChooserIndex", animatorChooserIndex};
        json += {"renderParameters1", renderParameters1.jsonRepresentation()};
        json += {"renderParameters2", renderParameters2.jsonRepresentation()};
        json += {"backgroundColor", backgroundColor.jsonRepresentation()};
        json += {"renderParameters", {renderParameters1.jsonRepresentation(), renderParameters2.jsonRepresentation()}};

        return json;
    }
};

Tree *tree;
TreeAnimator *animator;
TreeRenderer *renderer;
int frameRate = 120;

TreesParameters params1;
TreesParameters params2;

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
std::vector<AnimatorChooser> animatorChoosers;

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

int randInt(int max) {
    return of::random::uniform(0, max);
}

//--------------------------------------------------------------
void ofApp::setup() {
    ofJson test = ofLoadJson("/Users/owen/Desktop/test.json");
    
    cout << test["a"] << "\n";
    
//    TreesParameters testP = readFrom("/Users/owen/Desktop/testout.json");
    TreesParameters testP = TreesParameters::fromFile("/Users/owen/Desktop/testout.json");
    cout << testP.randomSeed << "\n";
    
    randomSeed = ofGetSystemTimeMillis();
    randomSeed = 1;
    of::random::seed(randomSeed);

//    ofJson obj;
////    obj += ofJson::object_t::value_type("randomSeed", randomSeed);
////    obj.push_back({{"randomSeed", randomSeed}});
////    obj = {{"randomSeed", randomSeed}};
//    obj = {{"", ""}};
//    obj += {"randomSeed", randomSeed};
//    obj += {"treeDepth", 6};
//    
//    ofSaveJson("/Users/owen/Desktop/testout.json", obj);

    cout << "SEED: " << randomSeed << "\n";
    
    params1.randomSeed = randomSeed;
    params1.treeDepth = 6;
    params1.animatorChooserIndex = 5;
    
    TreeRenderParameters renderParams1 = TreeRenderParameters();
    renderParams1.drawChooserIndex = 0;
    renderParams1.colorChooserIndex = 1;
    renderParams1.blendMode = OF_BLENDMODE_ADD;
    TreeRenderParameters renderParams2 = TreeRenderParameters();
    renderParams2.drawChooserIndex = 2;
    renderParams2.colorChooserIndex = 5;
    renderParams2.blendMode = OF_BLENDMODE_SCREEN;
    
    params1.renderParameters1 = renderParams1;
    params1.renderParameters2 = renderParams2;

    ofSavePrettyJson("/Users/owen/Desktop/paramsout.json", params1.jsonRepresentation());
    
    HSBFloats bgColor;
    bgColor.hue = 0;
    bgColor.saturation = 0;
    bgColor.brightness = 255;
    bgColor.alpha = 255;
    
    params1.backgroundColor = bgColor;

    windowWidth = 2000;
    windowHeight = 1000;
    screenScale = getRetinaScale();
    ofSetWindowShape(windowWidth * screenScale, windowHeight * screenScale);

    TreeGenerator generator = TreeGenerator(params1.treeDepth, windowHeight / 8);
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
                         ),
        // LEGACY animators below
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
                                                                    animatorChoosers[params1.animatorChooserIndex]);

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
    ofBackground(ofColor::fromHsb(params1.backgroundColor.hue, params1.backgroundColor.saturation, params1.backgroundColor.brightness, params1.backgroundColor.alpha));
}

//--------------------------------------------------------------
void ofApp::update(){
    animator->visitAll(ofGetFrameNum() / (float)frameRate, true);
}

//--------------------------------------------------------------
void ofApp::draw(){
    RenderedTree rendered = renderer->render();
    RenderedTreeDrawer drawer1 = RenderedTreeDrawer(rendered, colorChoosers[params1.renderParameters1.colorChooserIndex], drawChoosers[params1.renderParameters1.drawChooserIndex]);
    RenderedTreeDrawer drawer2 = RenderedTreeDrawer(rendered, colorChoosers[params1.renderParameters2.colorChooserIndex], drawChoosers[params1.renderParameters2.drawChooserIndex]);

    drawBuffer.begin();
    ofEnableBlendMode(params1.renderParameters1.blendMode);
    ofTranslate(ofGetWidth() / 4, ofGetHeight() / 2);
    ofScale(screenScale, screenScale);
    drawer1.drawAsPoints(rendered);
    drawBuffer.end();
    
    drawBuffer.draw(0, 0);
    
    drawBuffer2.begin();
    ofEnableBlendMode(params1.renderParameters2.blendMode);
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

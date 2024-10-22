//
//  RenderObjects.cpp
//  CircleTree
//
//  Created by Owen Mathews on 10/21/24.
//

#include "RenderObjects.hpp"
#include "Trees.hpp"

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

RenderObjects legacyRenderObjects = {
    // ANIMATORS
    {
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
                                               [](float v, float d) -> float { return v - 5; },
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
        // 13-16: LEGACY animators below; pretty standard stuff.
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
    },
    // COLOR SCHEMES
    {
        // 1: Orange, blue, purple
        {
            ofColor::fromHsb(150, 240, 230, 100),
            ofColor::fromHsb(170, 230, 250, 150),
            ofColor::fromHsb(190, 200, 200, 200),
            ofColor::fromHsb(25, 255, 240, 255)
        },
        // 2: Cranberry, magenta, purple, green
        {
            ofColor::fromHsb(90, 240, 120, 140),
            ofColor::fromHsb(200, 230, 150, 150),
            ofColor::fromHsb(220, 200, 170, 200),
            ofColor::fromHsb(240, 255, 190, 170)
        },
        // 3: Green
        {
            ofColor::fromHsb(90, 240, 230, 200)
        },
        // 4: Reds?
        {
            ofColor::fromHsb(50, 100, 230, 230),
            ofColor::fromHsb(45, 60, 255, 200)
        },
        // 5: LEGACY original(ish) chooser (RGB format)
        {
            ofColor(255, 200, 200, 200),
            ofColor(255, 200, 0, 220),
            ofColor(255, 0, 0, 240),
            ofColor(255, 120, 100)
        },
        // 6: Green/brown
        {
            ofColor::fromHsb(50, 240, 120, 240),
            ofColor::fromHsb(45, 210, 200, 170),
            ofColor::fromHsb(15, 250, 100, 200),
            ofColor::fromHsb(30, 255, 190, 170)
        },
        // 7: Blues
        {
            ofColor::fromHsb(170, 240, 120, 240),
            ofColor::fromHsb(170, 210, 200, 170),
            ofColor::fromHsb(170, 120, 250, 200),
            ofColor::fromHsb(170, 200, 220, 170)
        }
    },
    // COLOR CHOOSERS
    {
        // 0
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor { return node.color; },
        // 1-7: LEGACY color choosers, hard coded
        // 1: Orange, blue, purple
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor {
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
        // 2: Cranberry, magenta, purple, green
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor {
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
        // 3: Only renders the last node; kind of redundant with draw choosers now
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor {
            if (node.maxBranchDepth - node.depth == 0) {
                return ofColor::fromHsb(90, 240, 230, 200);
            } else {
                return ofColor::fromHsb(0, 0, 0, 0);
            }
        },
        // 4: Reds?
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor {
            int distFromLeaf = node.maxBranchDepth - node.depth;
            if (distFromLeaf < 2) {
                return ofColor::fromHsb(50, 100, 230, 230);
            } else {
                return ofColor::fromHsb(45, 60, 255, 200);
            }
        },
        // 5: LEGACY original(ish) chooser
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor {
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
        },
        // 6: Green/brown
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor {
            if (node.maxBranchDepth - node.depth == 0) {
                return ofColor::fromHsb(50, 240, 120, 240);
            } else if (node.maxBranchDepth - node.depth == 1) {
                return ofColor::fromHsb(45, 210, 200, 170);
            } else if (node.maxBranchDepth - node.depth == 2) {
                return ofColor::fromHsb(15, 250, 100, 200);
            } else {
                return ofColor::fromHsb(30, 255, 190, 170);
            }
        },
        // 7: Blues
        [](RenderedTreeNode node, ColorScheme colorScheme) -> ofColor {
            if (node.maxBranchDepth - node.depth == 0) {
                return ofColor::fromHsb(170, 240, 120, 240);
            } else if (node.maxBranchDepth - node.depth == 1) {
                return ofColor::fromHsb(170, 210, 200, 170);
            } else if (node.maxBranchDepth - node.depth == 2) {
                return ofColor::fromHsb(170, 120, 250, 200);
            } else {
                return ofColor::fromHsb(170, 200, 220, 170);
            }
        }
    },
    // DRAW CHOOSERS
    {
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
    },
    // ANIMATOR CHOOSERS
    {
        // 0-12: A bunch of things
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
        // 13-16: LEGACY choosers
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
        },
        // 17+: New choosers
        [](TreeNode *node, int depth, std::vector<NodeAnimator *> animators) -> NodeAnimator* {
            return animators[11];
        }
    }
};


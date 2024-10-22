//
//  RenderObjects.hpp
//  CircleTree
//
//  Created by Owen Mathews on 10/21/24.
//

#ifndef RenderObjects_hpp
#define RenderObjects_hpp

#include <stdio.h>
#include "ofMain.h"
#include "ofApp.h"
//#include "Trees.hpp"

struct RenderedTreeNode;
class NodeAnimator;
struct TreeNode;

typedef std::vector<ofColor> ColorScheme;

typedef ofColor (*ColorChooser)(RenderedTreeNode, ColorScheme);
typedef bool (*BinaryChooser)(RenderedTreeNode);
typedef NodeAnimator* (*AnimatorChooser)(TreeNode *, int, std::vector<NodeAnimator *>);

struct RenderObjects {
    std::vector<NodeAnimator *> animators;
    std::vector<std::vector<ofColor>> colorSchemes;
    std::vector<ColorChooser> colorChoosers;
    std::vector<BinaryChooser> drawChoosers;
    std::vector<AnimatorChooser> animatorChoosers;
};

extern RenderObjects legacyRenderObjects;

extern RenderObjects v1RenderObjects;

#endif /* RenderObjects_hpp */

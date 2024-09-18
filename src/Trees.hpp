//
//  Trees.hpp
//  CircleTree
//
//  Created by Owen Mathews on 9/10/24.
//

#ifndef Trees_hpp
#define Trees_hpp

#include <stdio.h>
#include <vector>
#include <math.h>
#include "ofApp.h"
#include "ofNode.h"


struct BranchParameters {
public:
    float aspect;
    float branchAngle;
    float terminusAngle;
    float size;
    float offset;
    
    BranchParameters();
    BranchParameters(float aspect, float branchAngle, float terminusAngle, float size, float offset);
};

typedef float (*AnimatorFunction)(float, float);
typedef BranchParameters (*BranchParametersAnimatorFunction)(BranchParameters, int, float);

struct NodeAnimatorFunctions {
    AnimatorFunction aspect;
    AnimatorFunction branchAngle;
    AnimatorFunction terminusAngle;
    AnimatorFunction size;
    AnimatorFunction offset;
    
    NodeAnimatorFunctions();
    
    NodeAnimatorFunctions(AnimatorFunction aspect,
                          AnimatorFunction branchAngle,
                          AnimatorFunction terminusAngle,
                          AnimatorFunction size,
                          AnimatorFunction offset);
};

class TreeNode;

class NodeAnimator {
public:
    NodeAnimatorFunctions f_speed;
    
    NodeAnimator(NodeAnimatorFunctions speed);
    
    void applyTo(TreeNode *node, float dt);
};

class TreeNode {
    public:
    BranchParameters parameters;
    std::vector<TreeNode *> children;
    NodeAnimator *animator;
    
    TreeNode(BranchParameters parameters);
    
    int inverseDepth() {
        int result = 0;
        TreeNode *node = this;
        while (!node->children.empty()) {
            result++;
            node = node->children[0];
        }
        return result;
    }
};

class Tree {
public:
    TreeNode *root;
    float size;
    
    Tree(float size);
    Tree(float size, TreeNode *root);
};

struct RenderedTreeNode {
public:
    ofNode node;
    ofPoint position;
    float size;
    ofVec2f velocity;
    int depth;
    int minBranchDepth;
    int maxBranchDepth;
    ofColor color;
    std::vector<RenderedTreeNode> children;

    RenderedTreeNode(ofPoint position, float size, ofVec2f velocity, int depth, int maxBranchDepth, int minBranchDepth, ofColor color):
    node(ofNode()),
    position(position),
    size(size),
    velocity(velocity),
    depth(depth),
    maxBranchDepth(maxBranchDepth),
    minBranchDepth(minBranchDepth),
    color(color),
    children(std::vector<RenderedTreeNode>()) {}
};

struct RenderedTree {
    RenderedTreeNode root;
    
    RenderedTree(RenderedTreeNode root): root(root) {}
};

template <typename Data, typename UpData>
class TreeVisitor {
public:
    Tree *tree;
    
    TreeVisitor(Tree *tree): tree(tree) {}
    
    void visitAll(Data initialData, UpData initialUpData) {
        visitHelper(tree->root, 0, initialData, initialUpData);
    }
        
    UpData visitHelper(TreeNode *node, int currentDepth, Data data, UpData upData) {
        visitNode(node, currentDepth, data);
        
        Data newData = modifyData(currentDepth, node, data);
        UpData newUpData = modifyUpData(currentDepth, node, upData);
        UpData reducedData = newUpData;
        
        for (TreeNode *child: node->children) {
            preVisit(child, currentDepth + 1, newData);
            reducedData = reduceUpData(visitHelper(child, currentDepth + 1, newData, newUpData), reducedData);
            visitNodeUp(node, currentDepth + 1, newData, reducedData);
            postVisit(child, currentDepth + 1, newData);
        }
        
        return reducedData;
    }
    
    virtual void preVisit(TreeNode *node, int currentDepth, Data data) {
    }
    
    virtual void visitNode(TreeNode *node, int currentDepth, Data data) {
    }
    
    virtual void visitNodeUp(TreeNode *node, int currentDepth, Data data, UpData upData) {
    }
    
    virtual void postVisit(TreeNode *node, int currentDepth, Data data) {
    }
    
    virtual Data modifyData(int currentDepth, TreeNode *node, Data data) {
        return data;
    }
    
    virtual UpData modifyUpData(int currentDepth, TreeNode *node, UpData data) {
        return data;
    }
    
    virtual UpData reduceUpData(UpData a, UpData b) {
        return a;
    }
};

class TreeRenderer {
public:
    Tree *tree;
    
    TreeRenderer(Tree *tree): tree(tree) {
    }
    
    RenderedTree render() {
        return RenderedTree(renderSubtree(tree->root, nullptr, ofMatrix4x4(), ofVec4f(0, 0, 0, 1), 0));
    }
    
    RenderedTreeNode renderSubtree(TreeNode *node, TreeNode *parent, ofMatrix4x4 currentMatrix, ofVec4f currentPoint, int currentDepth) {
        if (parent != nullptr) {
            currentMatrix.glRotate(node->parameters.terminusAngle, 0, 0, 1);
            currentMatrix.glTranslate(ofVec3f(0, -tree->size/2 - node->parameters.offset * tree->size / 2, 0));
            currentMatrix.glScale(node->parameters.size, node->parameters.size, 1);
            currentMatrix.glRotate(node->parameters.branchAngle, 0, 0, 1);
        }
        
        ofVec4f point = ofVec4f(0, 0, 0, 1);
        point = currentMatrix.preMult(point);

        ofColor color = ofColor(0, 0, 0, 255);
        switch (currentDepth) {
            case 0: color = ofColor(255, 0, 0, 255); break;
            case 1: color = ofColor(0, 255, 0, 255); break;
            case 2: color = ofColor(0, 0, 255, 255); break;
        }

        RenderedTreeNode renderedNode = RenderedTreeNode(point, currentMatrix.getScale().x * tree->size, ofVec2f(0, 0), currentDepth, currentDepth, currentDepth, color);

        std::vector<RenderedTreeNode> children = std::vector<RenderedTreeNode>();
        int maxBranchDepth = 0;
        int minBranchDepth = 1000000000;
        for (TreeNode *child: node->children) {
            RenderedTreeNode childTreeRoot = renderSubtree(child, node, currentMatrix, point, currentDepth + 1);
            renderedNode.minBranchDepth = min(minBranchDepth, childTreeRoot.minBranchDepth);
            renderedNode.maxBranchDepth = max(maxBranchDepth, childTreeRoot.maxBranchDepth);
            children.push_back(childTreeRoot);
        }
        
        renderedNode.children = children;
        
        return renderedNode;
    }
};

class RenderedTreeDrawer {
public:
    static void drawAsLines(RenderedTree tree) {
        drawSubtreeLines(tree.root, nullptr);
    }
    
    static void drawSubtreeLines(RenderedTreeNode node, RenderedTreeNode *parent) {
        ofSetColor(node.color);
        if (parent != nullptr) {
            ofDrawLine(parent->position.x, parent->position.y, node.position.x, node.position.y);
        }
        for (RenderedTreeNode child: node.children) {
            drawSubtreeLines(child, &node);
        }
    }
    
    static void drawAsCircles(RenderedTree tree) {
        drawSubtreeCircles(tree.root, nullptr);
    }
    
    static void drawSubtreeCircles(RenderedTreeNode node, RenderedTreeNode *parent) {
        ofSetColor(node.color);
        ofDrawEllipse(node.position.x, node.position.y, node.size, node.size);
        for (RenderedTreeNode child: node.children) {
            drawSubtreeCircles(child, &node);
        }
    }
    
    static void drawAsPoints(RenderedTree tree) {
        drawSubtreePoints(tree.root, nullptr);
    }
    
    static void drawSubtreePoints(RenderedTreeNode node, RenderedTreeNode *parent) {
        ofSetColor(node.color);
        if (node.maxBranchDepth - node.depth == 0) {
            ofSetColor(ofColor::fromHsb(150, 240, 230, 100));
        } else if (node.maxBranchDepth - node.depth == 1) {
            ofSetColor(ofColor::fromHsb(170, 230, 250, 150));
        } else if (node.maxBranchDepth - node.depth == 2) {
            ofSetColor(ofColor::fromHsb(190, 200, 200, 200));
        } else {
            ofSetColor(ofColor::fromHsb(25, 255, 240, 255));
        }

        ofDrawLine(node.position.x, node.position.y, node.position.x+0.5, node.position.y+0.5);
        for (RenderedTreeNode child: node.children) {
            drawSubtreePoints(child, &node);
        }
    }
    
    static void drawAsFatPoints(RenderedTree tree) {
        drawSubtreeFatPoints(tree.root, nullptr);
    }
    
    static void drawSubtreeFatPoints(RenderedTreeNode node, RenderedTreeNode *parent) {
        ofSetColor(node.color);
        if (node.maxBranchDepth - node.depth == 0) {
            ofSetColor(ofColor::fromHsb(150, 240, 230, 100));
        } else if (node.maxBranchDepth - node.depth == 1) {
            ofSetColor(ofColor::fromHsb(170, 230, 250, 150));
        } else if (node.maxBranchDepth - node.depth == 2) {
            ofSetColor(ofColor::fromHsb(190, 200, 200, 200));
        } else {
            ofSetColor(ofColor::fromHsb(25, 255, 240, 255));
        }
        
        ofDrawEllipse(node.position.x, node.position.y, 3, 3);
        for (RenderedTreeNode child: node.children) {
            drawSubtreeFatPoints(child, &node);
        }
    }

};

class TreeAnimator: public TreeVisitor<float, bool> {
public:
    TreeAnimator(Tree *tree): TreeVisitor(tree) {
    }
    
    void visitNode(TreeNode *node, int currentDepth, float dt) {
        if (node->animator != nullptr) {
            node->animator->applyTo(node, dt);
        }
    }
};

typedef NodeAnimator* (*AnimatorChooser)(TreeNode *, int, std::vector<NodeAnimator *>);

class TreeAnimatorInstaller: public TreeVisitor<bool, bool> {
    std::vector<NodeAnimator *> animators;
    AnimatorChooser animatorChooser;
    
public:
    TreeAnimatorInstaller(Tree *tree,
                          std::vector<NodeAnimator *> animators,
                          AnimatorChooser chooser):
    TreeVisitor(tree), animators(animators), animatorChooser(chooser) {
    }
    
    void visitAll() {
        TreeVisitor::visitAll(true, true);
    }
    
    void visitNode(TreeNode *node, int currentDepth, bool data) {
        if (currentDepth != 0) {
            node->animator = animatorChooser(node, currentDepth, animators);
        }
    }
};

class TreeGenerator {
public:
    int depth;
    int size;
    
    TreeGenerator(int depth, int size): depth(depth), size(size) {
    }
    
    Tree *generateTree() {
        TreeNode *root = generateHelper(depth - 1, BranchParameters(), true);
        return new Tree(size * 2, root);
    }
    
    TreeNode *generateHelper(int remainingDepth, BranchParameters parameters, bool initial) {
        TreeNode *node = new TreeNode(parameters);
        
        float scale = 0.4;
        if (remainingDepth > 0) {
            float numChildren;
            if (initial) {
                numChildren = 4;
                for (int i = 0; i < numChildren; i++) {
                    node->children.push_back(generateHelper(remainingDepth - 1, BranchParameters(1, 0, (float)i * 360.0 / numChildren, scale, 0), false));
                }
            } else {
                numChildren = (float)remainingDepth;
//                numChildren = 3;
                for (int i = 1; i <= numChildren; i++) {
                    float a = (float)i * 360.0 / (numChildren * 2) - 360.0 / numChildren;
                    node->children.push_back(generateHelper(remainingDepth - 1, BranchParameters(1, 0, a, scale, 0), false));
                }
            }
        }
        return node;
    }
};

#endif /* Trees_hpp */

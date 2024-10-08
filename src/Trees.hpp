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
#include "ofApp.h"


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
    ofPoint position;
    float scale;
    ofVec2f velocity;
    int depth;
    int minBranchDepth;
    int maxBranchDepth;
    ofColor color;
    std::vector<RenderedTreeNode> children;

    RenderedTreeNode(ofPoint position, float scale, ofVec2f velocity, int depth, int maxBranchDepth, int minBranchDepth, ofColor color):
    position(position),
    scale(scale),
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

class CircleTreeDrawer: public TreeVisitor<bool, bool> {
public:
    CircleTreeDrawer(Tree *tree): TreeVisitor(tree) {
    }
    
    void visitAll() {
        TreeVisitor::visitAll(true, true);
    }
    
    void preVisit(TreeNode *node, int currentDepth, bool data) {
        ofPushMatrix();
        
        ofRotateDeg(node->parameters.terminusAngle);
        ofTranslate(0, -tree->size/2 - node->parameters.offset * tree->size / 2);
        ofScale(node->parameters.size);
        ofRotateDeg(node->parameters.branchAngle);
    }
    
    void visitNode(TreeNode *node, int currentDepth, bool data) {
        ofDrawEllipse(0, 0, tree->size, tree->size);
    }
    
    void postVisit(TreeNode *node, int currentDepth, bool data) {
        ofPopMatrix();
    }
};

class LeafTreeDrawer: public TreeVisitor<float, int> {
public:
    LeafTreeDrawer(Tree *tree): TreeVisitor(tree) {
    }
    
    void visitAll() {
        TreeVisitor::visitAll(1, -1);
    }
    
    void preVisit(TreeNode *node, int currentDepth, float currentScale) {
        ofPushMatrix();
        
        ofRotateDeg(node->parameters.terminusAngle);
        ofTranslate(0, -tree->size/2 - node->parameters.offset * tree->size / 2);
        ofScale(node->parameters.size);
        ofRotateDeg(node->parameters.branchAngle);
    }
    
    void visitNode(TreeNode *node, int currentDepth, float currentScale) {
//        if (node->children.size() == 0) {
//            ofPushMatrix();
//            ofScale(1.0/currentScale);
//            ofDrawLine(0, 0, 1, 1);
//            ofPopMatrix();
//        }
    }
    
    void visitNodeUp(TreeNode *node, int currentDepth, float currentScale, int maxDepth) {
//        cout << node->inverseDepth() << ":" << currentDepth << ":" << maxDepth << "\n";
        
//        ofColor c;
        if (maxDepth - currentDepth == 0) {
//            c = ofColor(0.5, 0.8, 0.9);
//            ofSetColor(ofColor_(ofColor::fromHsb(0.5, 0.8, 0.9)));
            ofSetColor(ofColor::fromHsb(150, 240, 230, 200));
        } else if (maxDepth - currentDepth == 1) {
            ofSetColor(ofColor::fromHsb(30, 255, 250, 240));
        } else if (maxDepth - currentDepth == 2) {
            ofSetColor(255, 0, 0, 145);
        } else {
            ofSetColor(255, 200, 200, 100);
        }
        

//        if (maxDepth - currentDepth < 1) {
            ofPushMatrix();
            ofScale(1.0/currentScale);
//            ofSetColor(255, 0, 0, 255);
            ofDrawLine(0, 0, 1, 1);
            //        ofDrawCircle(0, 0, 10, 10);
            ofPopMatrix();
//        }
    }
    
    void postVisit(TreeNode *node, int currentDepth, float currentScale) {
        ofPopMatrix();
    }
    
    float modifyData(int currentDepth, TreeNode *node, float currentScale) {
        return currentScale * node->parameters.size;
    }
    
    int modifyUpData(int currentDepth, TreeNode *node, int upData) {
        return upData + 1;
    }
    
    int reduceUpData(int a, int b) {
        return max(a, b);
    }
};



class BranchTreeDrawer: public TreeVisitor<TreeNode *, int> {
public:
    BranchTreeDrawer(Tree *tree): TreeVisitor(tree) {
    }
    
    void visitAll() {
        TreeVisitor::visitAll(nullptr, -1);
    }
    
    void preVisit(TreeNode *node, int currentDepth, float currentScale) {
        ofPushMatrix();
        
        ofRotateDeg(node->parameters.terminusAngle);
        ofTranslate(0, -tree->size/2 - node->parameters.offset * tree->size / 2);
        ofScale(node->parameters.size);
        ofRotateDeg(node->parameters.branchAngle);
    }
    
    void visitNode(TreeNode *node, int currentDepth, TreeNode *parentNode) {
        //        if (node->children.size() == 0) {
        //            ofPushMatrix();
        //            ofScale(1.0/currentScale);
        //            ofDrawLine(0, 0, 1, 1);
        //            ofPopMatrix();
        //        }
    }
    
    void visitNodeUp(TreeNode *node, int currentDepth, TreeNode *parentNode, int maxDepth) {
//        //        cout << node->inverseDepth() << ":" << currentDepth << ":" << maxDepth << "\n";
//        
//        //        ofColor c;
//        if (maxDepth - currentDepth == 0) {
//            //            c = ofColor(0.5, 0.8, 0.9);
//            //            ofSetColor(ofColor_(ofColor::fromHsb(0.5, 0.8, 0.9)));
//            ofSetColor(ofColor::fromHsb(150, 240, 230, 200));
//        } else if (maxDepth - currentDepth == 1) {
//            ofSetColor(ofColor::fromHsb(30, 255, 250, 240));
//        } else if (maxDepth - currentDepth == 2) {
//            ofSetColor(255, 0, 0);
//        } else {
//            ofSetColor(255, 200, 200, 100);
//        }
//        
//        
//        //        if (maxDepth - currentDepth < 1) {
//        ofPushMatrix();
//        ofScale(1.0/currentScale);
//        ofSetColor(255, 0, 0, 255);
//        ofDrawLine(0, 0, 1, 1);
//        //        ofDrawCircle(0, 0, 10, 10);
//        ofPopMatrix();
//        //        }
    }
    
    void postVisit(TreeNode *node, int currentDepth, TreeNode *parentNode) {
        ofPopMatrix();
    }
    
//    TreeNode *parentNode modifyData(int currentDepth, TreeNode *node, TreeNode *parentNode) {
//        return parentNode;
//    }
    
    int modifyUpData(int currentDepth, TreeNode *node, int upData) {
        return upData + 1;
    }
    
    int reduceUpData(int a, int b) {
        return max(a, b);
    }
};

class TreeRenderer: public TreeVisitor<RenderedTreeNode, RenderedTreeNode> {
    RenderedTree *renderedTree;
    
    TreeRenderer(Tree *tree): TreeVisitor(tree), renderedTree(nullptr) {
    }
    
//    void visitAll
    
    void preVisit(TreeNode *node, int currentDepth, int data) {
        ofPushMatrix();
        
        ofRotateDeg(node->parameters.terminusAngle);
        ofTranslate(0, -tree->size/2 - node->parameters.offset * tree->size / 2);
        ofScale(node->parameters.size);
        ofRotateDeg(node->parameters.branchAngle);
    }

    void visitNode(TreeNode *node, int currentDepth, int data) {
        ofMatrix4x4 matrix = ofMatrix4x4(ofGetCurrentMatrix(ofMatrixMode::OF_MATRIX_MODELVIEW));
        ofVec4f point = ofVec4f(0, 0, 0, 1);
        point = matrix * point;
        
        RenderedTreeNode renderedNode = RenderedTreeNode(ofPoint(point.x, point.y), matrix.getScale().x, ofVec2f(0, 0), currentDepth, 0, 0, ofColor(255, 255, 255, 255));
        
        if (currentDepth == 0) {
            renderedTree = new RenderedTree(renderedNode);
        }
    }
    
    void visitNodeUp(TreeNode *node, int currentDepth, int data, RenderedTreeNode upData) {
        
    }
    
    void postVisit(TreeNode *node, int currentDepth, int data) {
        ofPopMatrix();
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
    
    void preVisit(TreeNode *node, int currentDepth, bool data) {
    }
    
    void visitNode(TreeNode *node, int currentDepth, bool data) {
//        if (currentDepth != 0) {
            node->animator = animatorChooser(node, currentDepth, animators);
//        }
    }
    
    void postVisit(TreeNode *node, int currentDepth, bool data) {
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
            if (initial) {
                for (int i = 0; i < 8; i++) {
                    node->children.push_back(generateHelper(remainingDepth - 1, BranchParameters(1, 0, (float)i * 360.0 / 8.0, scale, 0), false));
                }
            } else {
                for (int i = 1; i <= 4; i++) {
                    float a = (float)i * 360.0 / ((float)remainingDepth * 2) - 360.0 / (float)remainingDepth;
                    node->children.push_back(generateHelper(remainingDepth - 1, BranchParameters(1, 0, a, scale, 0), false));
                }
            }
        }
        return node;
    }
};

#endif /* Trees_hpp */

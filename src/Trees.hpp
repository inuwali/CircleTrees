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
};

class Tree {
public:
    TreeNode *root;
    float size;
    
    Tree(float size);
    Tree(float size, TreeNode *root);
};

template <typename Data>
class TreeVisitor {
public:
    Tree *tree;
    
    TreeVisitor(Tree *tree): tree(tree) {}
    
    void visitAll(Data initialData) {
//        cout << "TreeVisitor::visitAll() tree: " << tree << "\n";
//        cout << "TreeVisitor::visitAll() tree ROOT CHILD COUNT: " << tree->root->children.size() << "\n";
        visitHelper(tree->root, 0, initialData);
    }
        
    void visitHelper(TreeNode *node, int currentDepth, Data data) {
        visitNode(node, currentDepth, data);
        
        Data newData = modifyData(currentDepth, data);
        
        for (TreeNode *child: node->children) {
            preVisit(child, currentDepth + 1, newData);
            visitHelper(child, currentDepth + 1, newData);
            postVisit(child, currentDepth + 1, newData);
        }
    }
    
    virtual void preVisit(TreeNode *node, int currentDepth, Data data) {
    }
    
    virtual void visitNode(TreeNode *node, int currentDepth, Data data) {
    }
    
    virtual void postVisit(TreeNode *node, int currentDepth, Data data) {
    }
    
    virtual Data modifyData(int currentDepth, Data data) {
        return data;
    }
};

class CircleTreeDrawer: public TreeVisitor<bool> {
public:
    CircleTreeDrawer(Tree *tree): TreeVisitor(tree) {
    }
    
    void visitAll() {
        TreeVisitor::visitAll(true);
    }
    
    void preVisit(TreeNode *node, int currentDepth, bool data) {
        ofPushMatrix();
        
        //print("Applying params: " + node.parameters.terminusAngle + ", " + node.parameters.offset + ", " + node.parameters.size + ", " + node.parameters.branchAngle + "\n");
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

class CircleTreeAnimator: public TreeVisitor<float> {
public:
    CircleTreeAnimator(Tree *tree): TreeVisitor(tree) {
    }
    
    void preVisit(TreeNode *node, int currentDepth, float dt) {
    }
    
    void visitNode(TreeNode *node, int currentDepth, float dt) {
        if (node->animator != nullptr) {
            node->animator->applyTo(node, dt);
        }
    }
    
    void postVisit(TreeNode *node, int currentDepth, float dt) {
    }
};

class CircleTreeAnimatorInstaller: public TreeVisitor<NodeAnimator *> {
public:
    CircleTreeAnimatorInstaller(Tree *tree): TreeVisitor(tree) {
    }
    
    void preVisit(TreeNode *node, int currentDepth, NodeAnimator *animator) {
    }
    
    void visitNode(TreeNode *node, int currentDepth, NodeAnimator *animator) {
        if (currentDepth != 0) {
            node->animator = animator;
        }
    }
    
    void postVisit(TreeNode *node, int currentDepth, NodeAnimator *animator) {
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
                for (int i = 0; i < 6; i++) {
                    node->children.push_back(generateHelper(remainingDepth - 1, BranchParameters(1, 0, (float)i * 360.0 / 6.0, scale, 0), false));
                }
            } else {
                for (int i = 1; i < 4; i++) {
                    float a = (float)i * 360.0 / 6.0 - 360.0 / 3.0;
                    node->children.push_back(generateHelper(remainingDepth - 1, BranchParameters(1, 0, a, scale, 0), false));
                }
            }
        }
        return node;
    }
};

#endif /* Trees_hpp */

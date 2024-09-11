//
//  Trees.cpp
//  CircleTree
//
//  Created by Owen Mathews on 9/10/24.
//

#include "Trees.hpp"
#include <vector>

float animConstant(float v, float d) { return v; }

BranchParameters::BranchParameters():
aspect(0),
branchAngle(0),
terminusAngle(0),
size(0),
offset(0)
{}

BranchParameters::BranchParameters(float aspect, float branchAngle, float terminusAngle, float size, float offset):
aspect(aspect),
branchAngle(branchAngle),
terminusAngle(terminusAngle),
size(size),
offset(offset)
{}

NodeAnimatorFunctions::NodeAnimatorFunctions() {
    aspect = &animConstant;
    branchAngle = &animConstant;
    terminusAngle = &animConstant;
    size = &animConstant;
    offset = &animConstant;
}

NodeAnimatorFunctions::NodeAnimatorFunctions(AnimatorFunction aspect,
                                             AnimatorFunction branchAngle,
                                             AnimatorFunction terminusAngle,
                                             AnimatorFunction size,
                                             AnimatorFunction offset) :
aspect(aspect),
branchAngle(branchAngle),
terminusAngle(terminusAngle),
size(size),
offset(offset)
{
    if (aspect == nullptr) {
        this->aspect = &animConstant;
    }
    if (branchAngle == nullptr) {
        this->branchAngle = &animConstant;
    }
    if (terminusAngle == nullptr) {
        this->terminusAngle = &animConstant;
    }
    if (size == nullptr) {
        this->size = &animConstant;
    }
    if (offset == nullptr) {
        this->offset = &animConstant;
    }
}

NodeAnimator::NodeAnimator(NodeAnimatorFunctions speed) {
    f_speed = speed;
}

void NodeAnimator::applyTo(TreeNode *node, float dt) {
    BranchParameters speed = node->parameters;
    
    speed.aspect = f_speed.aspect(speed.aspect, dt);
    speed.branchAngle = f_speed.branchAngle(speed.branchAngle, dt);
    speed.terminusAngle = f_speed.terminusAngle(speed.terminusAngle, dt);
    speed.size = f_speed.size(speed.size, dt);
    speed.offset = f_speed.offset(speed.offset, dt);
    
    node->parameters.aspect = speed.aspect;
    node->parameters.branchAngle = speed.branchAngle;
    node->parameters.terminusAngle = speed.terminusAngle;
    node->parameters.size = speed.size;
    node->parameters.offset = speed.offset;
}

TreeNode::TreeNode(BranchParameters parameters):
parameters(parameters),
animator(nullptr),
children(std::vector<TreeNode*>())
{
}

Tree::Tree(float size):
root(new TreeNode(BranchParameters())),
size(100)
{
}

Tree::Tree(float size, TreeNode *root):
root(root),
size(size)
{
}

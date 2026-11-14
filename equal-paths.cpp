#ifndef RECCHECK
//if you want to add any #includes like <iostream> you must do them here (before the next endif)

#endif

#include "equal-paths.h"
using namespace std;

namespace {
bool checkDepth(Node* node, int depth, int& leafDepth)
{
    if (node == nullptr) return true;
    if (node->left == nullptr && node->right == nullptr) {
        if (leafDepth == -1) leafDepth = depth;
        return depth == leafDepth;
    }
    return checkDepth(node->left, depth + 1, leafDepth) &&
           checkDepth(node->right, depth + 1, leafDepth);
}
}

bool equalPaths(Node* root)
{
    int leafDepth = -1;
    return checkDepth(root, 0, leafDepth);
}

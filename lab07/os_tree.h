#ifndef LAB07_OS_TREE_H
#define LAB07_OS_TREE_H
#include <algorithm>

// os augmented avl tree
struct Node {
    int height;
    int rank;
    Node *left, *right;
};

int get_height(const Node* root);
int calc_height(const Node* root);
void update_height(Node* root);

#endif //LAB07_OS_TREE_H
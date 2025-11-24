#ifndef LAB07_OS_TREE_H
#define LAB07_OS_TREE_H
#include <algorithm>
#include <commandline.h>
#include <Profiler.h>

namespace lab07
{
    // os augmented avl tree
    struct Node {
        int height;
        int size;
        int key;
        Node *left, *right;
    };

    void update_node(Node* root);
    int get_height(const Node* root);

    Node *rotate_left(Node *root);
    Node *rotate_right(Node *root);

    Node* BuildTree(int l, int r);
    Node* insert(Node* root);

    void demonstrate(int size);
    void performance(Profiler &profiler);

}

#endif //LAB07_OS_TREE_H
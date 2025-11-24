#ifndef LAB07_OS_TREE_H
#define LAB07_OS_TREE_H
#include <algorithm>
#include <commandline.h>
#include <Profiler.h>
#include <random>

namespace lab07
{
    // os augmented avl tree
    struct Node {
        int height;
        int size;
        int key;
        Node *left, *right;
    };

    void update_node(Node* root, Operation* op = nullptr);
    int get_height(const Node* root);
    int get_bf(const Node* root);

    Node *rotate_left(Node *root);
    Node *rotate_right(Node *root);

    Node* build_tree(int l, int r, Operation* op = nullptr);
    Node* os_select(Node* root, int ith, Operation* op = nullptr);
    Node* bst_delete(Node* root, int key, Operation* op);
    Node* os_delete(Node* root, int ith, Operation* op = nullptr);
    Node* rebalance(Node* root);

    void delete_tree(Node** root);
    void pretty_print(const Node* root, int depth = 0);
    void demonstrate(int l, int r);
    void performance(Profiler &profiler);

}

#endif //LAB07_OS_TREE_H
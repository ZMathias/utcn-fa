#include "os_tree.h"

int get_height(const Node* root) {
    if (root == nullptr) {
        return -1;
    }

    return root->height;
}

int get_size(const Node* root) {
    return root ? root->rank : 0;
}

void update_node(Node* root) {
    if (root == nullptr) {
        return;
    }

    root->height = 1 + std::max(get_height(root->left), get_height(root->right));

    root->rank = 1 + get_size(root->left) + get_size(root->right);
}

Node *rotate_left(Node *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->right == nullptr) {
        return root;
    }

    // we take the node and put its parent as the right child
    // take the roots right child and put it in place of the parents left child
    Node* parent = root;
    Node* target = parent->right;

    parent->right = target->left;
    target->left = parent;

    update_node(root);
    update_node(target);

    return target;
}

Node *rotate_right(Node *root) {
    if (root == nullptr) {
        return nullptr;
    }

    if (root->left == nullptr) {
        return root;
    }

    // we take the node and put its parent as the right child
    // take the roots right child and put it in place of the parents left child
    Node* parent = root;
    Node* target = parent->left;

    parent->left = target->right;
    target->right = parent;

    update_node(root);
    update_node(target);

    return target;
}


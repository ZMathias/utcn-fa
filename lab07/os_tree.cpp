#include "os_tree.h"

int get_height(const Node* root) {
    if (root == nullptr) {
        return -1;
    }

    return root->height;
}

int calc_height(const Node* root) {
    if (root == nullptr) {
        return -1;
    }

    return std::max(calc_height(root->left), calc_height(root->right)) + 1;
}

void update_height(Node* root) {
    if (root == nullptr) {
        return;
    }

    update_height(root->left);
    update_height(root->right);

    root->height = std::max(get_height(root->left), get_height(root->right)) + 1;
}
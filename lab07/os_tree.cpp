#include "os_tree.h"

namespace lab07
{
    Node* create_node(int key) {
        Node* node = new Node;

        node->key = key;
        node->height = 0;
        node->size = 1;
        node->left = node->right = nullptr;

        return node;
    }

    int get_height(const Node* root) {
        if (root == nullptr) {
            return -1;
        }

        return root->height;
    }

    int get_size(const Node* root) {
        return root ? root->size : 0;
    }

    void update_node(Node* root) {
        if (root == nullptr) {
            return;
        }

        root->height = 1 + std::max(get_height(root->left), get_height(root->right));

        root->size = 1 + get_size(root->left) + get_size(root->right);
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

    Node* insert(Node* root, int key) {
        // we use binary search insertion but we have to update height
        // rebalance when we unwind from recursion
        if (root == nullptr) {
            return nullptr;
        }

        if (key == root->key) {
            return root;
        }

        if (key < root->key) {
            root->left = insert(root->left, key);
        } else {
            root->right = insert(root->right, key);
        }

        // after coming back from recursion, update the height of current node
        // this is done bottom up assuming the children heights are correct.
        // since the recursion stops always at a leaf node we always reach a nullptr

        return root;
    }

    // builds a tree with each key from the interval [l, r]
    Node* build_tree(const int l, const int r) {
        if (r > l) {
            return nullptr;
        }

        const int mid = (l + r) / 2;
        Node *node = create_node(mid);
        node->size = r - l + 1; // update order statistics

        node->left = build_tree(l, mid - 1);
        node->right = build_tree(mid + 1, r);

        // when recursion unwinds we update the height of each node
        update_node(&node);

        return node;
    }



    void demonstrate(int size) {

    }

    void performance(Profiler& profiler) {

    }

}


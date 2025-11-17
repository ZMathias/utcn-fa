#include "trees.h"

#include <cstdio>

void traverse_rec(const BNodeT* root, const int type) {
    if (root == nullptr) {
        return;
    }

    if (type == PREORDER) {
        printf("%d ", root->value);
    }

    traverse_rec(root->left, type);

    if (type == INORDER) {
        printf("%d ", root->value);
    }

    traverse_rec(root->right, type);

    if (type == POSTORDER) {
        printf("%d ", root->value);
    }
}


void traverse_it(BNodeT* root, int type) {
    int dir = 1;
    BNodeT* node = root;

    do {
        switch (dir) {
        case 1:
            {
                if (type == PREORDER) {
                    printf("%d ", node->value);
                }

                if (node->left != nullptr) {
                    node = node->left;
                } else {
                    dir = 2;
                }

                break;
            }

        case 2:
            {
                if (type == INORDER) {
                    printf("%d ", node->value);
                }

                if (node->right != nullptr) {
                    node = node->right;
                    dir = 1;
                } else {
                    dir = 3;
                }
                break;
            }
        case 3:
            {
                if (type == POSTORDER) {
                    printf("%d ", node->value);
                }

                if (node->parent != nullptr) {
                    if (node == node->parent->left) {
                        dir = 2;
                    }

                    node = node->parent;
                }
                break;
            }
        default:
            {

            }
        }
    } while (node != root || dir != 3);

    if (type == POSTORDER) {
        printf("%d ", node->value);
    }
}

void pretty_print_r1(int *parent, int size, int node, int depth) {
    constexpr int spaces_per_depth = 2;

    for (int i = 0; i < size; i++) {
        if (parent[i] == node) {
            for (int x = 0; x < depth * spaces_per_depth; x++) {
                putchar(' ');
            }
            printf("%d\n", i);
            pretty_print_r1(parent, size, i, depth + 1);
        }
    }
}

void pretty_print(int *parent, int size) {
    pretty_print_r1(parent, size, -1, 0);
}

void pretty_print(BNodeT* root, const int depth) {
    if (root == nullptr) {
        return;
    }

    // indentation according to depth
    // 2 spaces per unit depth
    constexpr int spaces_per_depth = 2;
    for (int i = 0; i  < depth * spaces_per_depth; i++) {
        putchar(' ');
    }

    printf("%d\n", root->value);
    pretty_print(root->left, depth + 1);
    pretty_print(root->right, depth + 1);
}

void pretty_print_rotated(BNodeT* root, const int depth) {
    if (root == nullptr) {
        return;
    }

    constexpr int spaces_per_depth = 2;
    pretty_print_rotated(root->right, depth + 1);

    for (int i = 0; i < depth * spaces_per_depth; i++) {
        putchar(' ');
    }

    printf("%d\n", root->value);
    pretty_print_rotated(root->left, depth + 1);
}

void pretty_print(MNodeT* root, const int depth) {
    if (root == nullptr) {
        return;
    }

    // indentation according to depth
    // 2 spaces per unit depth
    constexpr int spaces_per_depth = 2;
    for (int i = 0; i  < depth * spaces_per_depth; i++) {
        putchar(' ');
    }

    printf("%d\n", root->value);
    for (int i = 0; i < root->size; i++) {
        pretty_print(root->children[i], depth + 1);
    }
}
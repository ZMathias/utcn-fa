#include "trees.h"

BNodeT* create_bnode(int value) {
    auto* b_root = new BNodeT;
    b_root->left=nullptr;
    b_root->right=nullptr;
    b_root->value = value;
    b_root->parent = nullptr;

    return b_root;
}

MNodeT* create_mnode(const int value) {
    auto* node = new MNodeT;
    node->size = 0;
    node->value = value;

    return node;
}

bool add_child(MNodeT *node, MNodeT* child) {
    if (node == nullptr || child == nullptr)
        return false;

    if (node->size >= 100) {
        return false;
    }

    node->children[node->size++] = child;
    return true;
}

BNodeT* build_tree_w(const std::vector<int>& trav, int &pos) {
    if (pos >= trav.size()) {
        return nullptr;
    }

    if (trav[pos] == -1) {
        pos++; // consume -1
        return nullptr;
    }

    BNodeT* root = create_bnode(trav[pos++]);
    root->left = build_tree_w(trav, pos);
    root->right = build_tree_w(trav, pos);

    if (root->left != nullptr) {
        root->left->parent = root;
    }

    if (root->right != nullptr) {
        root->right->parent = root;
    }

    return root;
}

BNodeT* build_tree(const std::vector<int>& trav) {
    int i = 0;
    return build_tree_w(trav, i);
}

MNodeT* transform_r1_to_r2(const int* parent, int size) {
    if (parent == nullptr) {
        return nullptr;
    }

    MNodeT** nodes = new MNodeT*[size];
    //MNodeT* nodes[size];

    for (int i = 0; i < size; i++) {
        nodes[i] = create_mnode(i + 1);
        nodes[i]->size = 0;
    }

    MNodeT* root = nullptr;
    for (int i = 0; i < size; i++) {
        if (parent[i] == -1) {
            root = nodes[i];
            continue;
        }
        // add node as child to its parent node
        add_child(nodes[parent[i] - 1], nodes[i]);
    }

    delete[] nodes;
    return root;
}

BNodeT* transform_r2_to_r3(const MNodeT* m_root) {
    if (m_root == nullptr) {
        return nullptr;
    }

    const auto b_root = create_bnode(m_root->value);

    if (m_root->size == 0) {
        return b_root;
    }

    b_root->left = transform_r2_to_r3(m_root->children[0]);
    if (b_root->left != nullptr) {
        b_root->left->parent = b_root;
    }

    BNodeT* curr_bnode = b_root->left;

    for (int i = 1; i < m_root->size; i++) {
        // transform child
        curr_bnode -> right = transform_r2_to_r3(m_root->children[i]);

        if (curr_bnode->right != nullptr) {
            curr_bnode->right->parent = curr_bnode; // for binary representation it should point to the structural parent, not the logical parent for traversals
        }

        curr_bnode = curr_bnode->right;
    }

    return b_root;
}

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
    if (root == nullptr) {
        return;
    }
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
            printf("%d\n", i + 1);
            pretty_print_r1(parent, size, i + 1, depth + 1);
        }
    }
}

void pretty_print(int *parent, int size) {
    if (parent == nullptr) {
        return;
    }

    pretty_print_r1(parent, size, -1, 0);
}

void pretty_print_bin(BNodeT* root, const int depth, int type) {
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
    pretty_print_bin(root->left, depth + 1, type);
    pretty_print_bin(root->right, depth + type, type);
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

void pretty_print(MNodeT* root) {
    pretty_print(root, 0);
}

void pretty_print(BNodeT* root, int type) {
    switch (type) {
    case BINARY:
        {
            pretty_print_bin(root, 0, 1);
            break;
        }

    case MULTIWAY_BINARY:
        {
            pretty_print_bin(root, 0, 0);
            break;
        }
        default:
        {
            break;
        }
    }
}

void delete_tree(MNodeT** root) {
    if (root == nullptr || (*root) == nullptr) {
        return;
    }

    for (int i = 0; i < (*root)->size; i++) {
        delete_tree((*root)->children + i);
    }

    delete (*root);
    *root = nullptr;
}

void delete_tree(BNodeT** root) {
    if (root == nullptr || (*root) == nullptr) {
        return;
    }

    delete_tree(&(*root)->left);
    delete_tree(&(*root)->right);

    delete (*root);
    *root = nullptr;
}
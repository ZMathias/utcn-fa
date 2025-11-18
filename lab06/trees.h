#ifndef LAB06_TREES_H
#define LAB06_TREES_H

#include <cstdio>
#include <vector>

enum directions
{
    PREORDER, INORDER, POSTORDER
};

enum tree_type
{
    MULTIWAY_BINARY = 0,
    BINARY = 1,
};

struct BNodeT
{
    int value = 0;
    BNodeT* left = nullptr, *right = nullptr;
    BNodeT* parent = nullptr;
};

struct MNodeT
{
    int value = 0;
    MNodeT* children[100];
    int size = 0;
};

BNodeT* create_bnode(int value);
MNodeT* create_mnode(int value);
bool add_child(MNodeT *node, MNodeT* child);

BNodeT* build_tree(const std::vector<int>& trav);

MNodeT* transform_r1_to_r2(const int* parent, int size);
BNodeT* transform_r2_to_r3(const MNodeT* m_root);

void traverse_rec(const BNodeT* root, int type = PREORDER);
void traverse_it(BNodeT* root, int type = PREORDER);

void pretty_print(int *parent, int size);
void pretty_print(MNodeT* root);
void pretty_print(BNodeT* root, int type);

void delete_tree(MNodeT** root);
void delete_tree(BNodeT** root);

#endif //LAB06_TREES_H
#ifndef LAB06_TREES_H
#define LAB06_TREES_H

enum directions
{
    PREORDER, INORDER, POSTORDER
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
    MNodeT* children[100] = {};
    int size = 0;
};

void traverse_rec(const BNodeT* root, int type = PREORDER);
void traverse_it(BNodeT* root, int type = PREORDER);
void pretty_print(int *parent, int size);
void pretty_print(BNodeT* root, int depth = 0);
void pretty_print(MNodeT* root, int depth = 0);
void pretty_print_rotated(BNodeT* root, int depth = 0);

#endif //LAB06_TREES_H
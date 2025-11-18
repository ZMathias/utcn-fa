#include <iostream>
#include <string>
#include <sstream>

#include "trees.h"

/*
 * Author: Zágoni Mátyás
 * Group: 30421
 *
 * This project implements lab-assignment number 6 (Multi-way trees)
 * There are several overloaded pretty_print functions for printing each representation (R1, R2, R3 and an additional classical binary tree)
 * It contains both transformations T1, T2
 * It contains both recursive and iterative versions of binary tree traversal functions.
 * It also uses a binary tree building algorithm from preorder traversal using -1 placeholders for null nodes for deterministic tree building
 *
 * Iterative and recursive traversals are compared and user is prompted for parent array which should be indexed from 1
 *
 */

BNodeT* scan_tree(const std::string& input) {
    std::istringstream ss(input);

    std::vector<int> preorder;

    int x;
    while (ss >> x) {
        preorder.push_back(x);
    }

    return build_tree(preorder);
}

std::vector<int> build_array(const std::string& line) {
    std::istringstream ss(line);

    std::vector<int> parent;

    int x;
    while (ss >> x) {
        parent.push_back(x);
    }

    return parent;
}

bool demo_traversals(BNodeT* root) {
    pretty_print(root, BINARY);

    printf("\n\nPreorder:\nrec: ");
    traverse_rec(root, PREORDER);
    printf("\nit: ");
    traverse_it(root, PREORDER);

    printf("\n\nInorder:\nrec: ");
    traverse_rec(root, INORDER);
    printf("\nit: ");
    traverse_it(root, INORDER);

    printf("\n\nPostorder:\nrec: ");
    traverse_rec(root, POSTORDER);
    printf("\nit: ");
    traverse_it(root, POSTORDER);

    delete_tree(&root);

    return true;
}



int main() {
    std::string line;

    // parent array
    printf("Enter parent array of tree: ");
    std::getline(std::cin, line);

    // build parent vector from input
    auto parent = build_array(line);

    // pretty print R1 parent array representation
    printf("\nPretty Print R1:\n");
    pretty_print(parent.data(), static_cast<int>(parent.size()));

    // T1
    MNodeT* m_root = transform_r1_to_r2(parent.data(), parent.size());


    // pretty print R2 multiway representation
    printf("\nPretty print R2:\n");
    pretty_print(m_root);

    // T2
    BNodeT* bm_root = transform_r2_to_r3(m_root);

    // pretty print R3 binary multiway representation
    printf("\nPretty print R3:\n");
    pretty_print(bm_root, MULTIWAY_BINARY);

    printf("Enter preorder with -1 as empty nodes (finish with -1 aswell): ");
    std::getline(std::cin, line);

    BNodeT* b_root = scan_tree(line);

    demo_traversals(b_root);

    delete_tree(&m_root);
    delete_tree(&bm_root);
    delete_tree(&b_root);
    return 0;
}
#include "os_tree.h"

/*
 * Implementation of an order statistics augmented AVL tree
 * The build_tree according to the master theorem is of O(n)
 * os_select is of O(logn) because it traverses along the height of the tree
 * os_delete is of O(logn) aswell
 * The actual complexities of the code are a bit higher due to the AVL tree balancing act
 * But that is of a constant around 1.45
 */

namespace lab07
{
    int random_number(int lo, int hi) {
        static std::mt19937 gen(std::random_device{}());
        std::uniform_int_distribution<int> distrib(lo, hi);
        return distrib(gen);
    }

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

    int get_bf(const Node* root) {
        if (root == nullptr) {
            return 0;
        }

        int h_l = get_height(root->left);
        int h_r = get_height(root->right);

        return h_r - h_l;
    }


    int get_size(const Node* root, Operation* op = nullptr) {
        if (op) op->count();
        return root ? root->size : 0;
    }

    void update_height(Node* root) {
        root->height = 1 + std::max(get_height(root->left), get_height(root->right));
    }

    void update_node(Node* root, Operation* op) {
        if (root == nullptr) {
            return;
        }

        root->height = 1 + std::max(get_height(root->left), get_height(root->right));

        if (op) op->count();
        root->size = 1 + get_size(root->left, op) + get_size(root->right, op);
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

    // builds a tree with each key from the interval [l, r]
    Node* build_tree(const int l, const int r, Operation* op) {
        if (l > r) {
            return nullptr;
        }

        if (op) op->count(3);
        const int mid = (l + r) / 2;
        Node *node = create_node(mid);
        node->size = r - l + 1; // update order statistics

        node->left = build_tree(l, mid - 1, op);
        node->right = build_tree(mid + 1, r, op);

        // when recursion unwinds we update the height of each node
        //update_node(node, op);
        update_height(node);

        return node;
    }

    Node* os_select(Node* root, int ith, Operation* op) {
        // we want the ith inorder element efficiently
        if (root == nullptr) {
            return nullptr;
        }

        int rank;

        if (root->left == nullptr) {
            if (op) op->count();
            rank = 1;
        }
        else {
            if (op) op->count(2);
            rank = root->left->size + 1;
        }

        if (ith == rank)
            return root;

        if (ith < rank)
            return os_select(root->left, ith, op);

        return os_select(root->right, ith - rank, op);

    }

    Node* rebalance(Node* root) {
        if (root == nullptr) {
            return nullptr;
        }

        update_node(root);

        const int bf = get_bf(root);

        if (bf < -1) { // left heavy
            if (get_bf(root->left) > 0) // LR case
                root->left = rotate_left(root->left);

            root = rotate_right(root);
        }
        else if (bf > 1) { // right heavy

            if (get_bf(root->right) < 0) // RL case
                root->right = rotate_right(root->right);

            root = rotate_left(root);
        }

        return root;
    }

    Node* inorder_succ(Node* root, Operation* op = nullptr) {
        if (root == nullptr) return nullptr;

        if (op) op->count(2);
        root = root->right;
        while (root->left != nullptr) {
            root = root->left;
            if (op) op->count(2);
        }

        return root;
    }

    Node* bst_delete(Node* root, const int key, Operation* op) {
        if (root == nullptr) {
            return nullptr;
        }

        // recurse down to the target node
        if (key < root->key) {
            if (op) op->count(2);
            root->left = bst_delete(root->left, key, op);
        }
        else if (key > root->key) {
            if (op) op->count(2);
            root->right = bst_delete(root->right, key, op);
        }
        else {
            // we found the key
            // break it down to the three cases

            if (op) op->count();
            if (root->right == nullptr) {
                if (op) op->count(2);
                Node* tmp = root->left;
                delete root;
                return tmp;
            }


            if (op) op->count();
            if (root->left == nullptr) {
                if (op) op->count(2);
                Node* tmp = root->right;
                delete root;
                return tmp;
            }

            if (op) op->count(3);
            // two children case
            Node* successor = inorder_succ(root, op);
            root->key = successor->key;

            root->right = bst_delete(root->right, successor->key, op);
        }

        return rebalance(root);
    }

    Node* os_delete(Node* root, int ith, Operation* op) {
        if (op) op->count();
        const Node* tmp = os_select(root, ith, op);
        if (tmp == nullptr)
            return root;

        return bst_delete(root, tmp->key, op); // updates order statistics aswell
    }

    void pretty_print(const Node* root, int depth) {
        if (root == nullptr) {
            return;
        }

        constexpr int spaces_per_depth = 3;
        for (int i = 0; i < depth * spaces_per_depth; i++) {
            putchar(' ');
        }

        printf("%d (b: %d, h: %d, s: %d)\n", root->key, get_bf(root), get_height(root), get_size(root));
        pretty_print(root->left, depth + 1);
        pretty_print(root->right, depth + 1);
    }

    void delete_tree(Node** root) {
        if (root == nullptr || (*root) == nullptr) {
            return;
        }

        delete_tree(&(*root)->left);
        delete_tree(&(*root)->right);

        delete (*root);
        *root = nullptr;
    }

    void print_node(Node* node) {
        printf("%p", node);

        if (node != nullptr) {
            printf(": %d", node->key);
        }

        putchar('\n');
    }

    void demonstrate(int l, int r) {
        Node* root = build_tree(l, r);
        pretty_print(root); // demo 1.3.1

        /*root = os_delete(root, 9);
        root = os_delete(root, 2);
        root = os_delete(root, 5);

        pretty_print(root);*/

        // demo 1.3.2
        int rand_int = random_number(l, r);
        Node *sel = os_select(root, rand_int);
        printf("ith(%d) ", rand_int);
        print_node(sel);

        rand_int = random_number(l, r);
        sel = os_select(root, rand_int);
        printf("ith(%d) ", rand_int);
        print_node(sel);

        rand_int = random_number(l, r);
        sel = os_select(root, rand_int);
        printf("ith(%d) ", rand_int);
        print_node(sel);

        // demo 1.3.3
        rand_int = random_number(l, r);
        sel = os_select(root, rand_int);
        if (sel != nullptr)
            printf("\nsel value: %d\n", sel->key);
        root = os_delete(root, rand_int);
        printf("After deleting ith(%d):\n", rand_int);
        pretty_print(root);

        rand_int = random_number(l, r - 1);
        sel = os_select(root, rand_int);
        if (sel != nullptr)
            printf("\nsel value: %d\n", sel->key);
        root = os_delete(root, rand_int);
        printf("After deleting ith(%d):\n", rand_int);
        pretty_print(root);

        rand_int = random_number(l, r - 2);
        sel = os_select(root, rand_int);
        if (sel != nullptr)
            printf("\nsel value: %d\n", sel->key);
        root = os_delete(root, rand_int);
        printf("After deleting ith(%d):\n", rand_int);
        pretty_print(root);

        putchar('\n');

        delete_tree(&root);
    }

    void performance(Profiler& profiler) {
        for (int times = 0; times < 5; times++) {
            for (int n = 100; n <= 10000; n += 100) {
                Operation osb = profiler.createOperation("osB", n);
                Operation oss = profiler.createOperation("osS", n);
                Operation osD = profiler.createOperation("osD", n);
                Node* root = build_tree(1, n, &osb);

                for (int j = n; j >= 1; j--) {
                    const int rand_int = random_number(1, j);
                    Node* sel = os_select(root, rand_int, &oss);
                    root = os_delete(root, rand_int, &osD);
                }
            }
        }

        profiler.divideValues("osB", 5);
        profiler.divideValues("osS", 5);
        profiler.divideValues("osD", 5);
        profiler.createGroup("Operations", "osB", "osS", "osD");
    }

}


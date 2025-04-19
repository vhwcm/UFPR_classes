#include <stdio.h>
#include <stdlib.h>

// Define colors
#define RED   1
#define BLACK 0

// Node structure
typedef struct Node {
    int key;
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
} Node;

// Tree structure holding root and NIL sentinel
typedef struct Tree {
    struct Node *NIL; // Sentinel node (NIL) - Represents all null leaves
    struct Node *root;
} Tree;

// Global pointer to the Tree structure
Tree *redBlack;

// --- Function Prototypes ---
Node* createNode(int key);
void leftRotate(Node *x); // No longer needs root pointer
void rightRotate(Node *y); // No longer needs root pointer
void insertFixup(Node *z); // No longer needs root pointer
void insert(int key); // No longer needs root pointer
void transplant(Node *u, Node *v); // No longer needs root pointer
Node* minimum(Node *x);
Node* maximum(Node *x);
void deleteFixup(Node *x); // No longer needs root pointer
void deleteNode(Node *z); // No longer needs root pointer
Node* search(int key); // Takes key, starts search from redBlack->root
void printInOrderWithLevel(Node *node, int level);
void printParenthesized(Node *node);
void freeTree(Node *node);

// --- Main Function ---
int main() {
    // Initialize the global Tree structure
    redBlack = (Tree*)malloc(sizeof(Tree));
    if (redBlack == NULL) {
        fprintf(stderr, "Memory allocation failed for RedBlack Tree.\n");
        return 1;
    }

    // Initialize the NIL sentinel node
    redBlack->NIL = (Node*)malloc(sizeof(Node));
    if (redBlack->NIL == NULL) {
        fprintf(stderr, "Memory allocation failed for NIL node.\n");
        free(redBlack); // Free the tree struct if NIL allocation fails
        return 1;
    }
    redBlack->NIL->color = BLACK;
    redBlack->NIL->left = redBlack->NIL;
    redBlack->NIL->right = redBlack->NIL;
    redBlack->NIL->parent = redBlack->NIL;

    // Initialize the root to NIL (empty tree)
    redBlack->root = redBlack->NIL;

    char op;
    int key;

    // Read operations and keys from stdin
    while (scanf(" %c %d", &op, &key) == 2) {
        if (op == 'i') {
            insert(key); // Call insert directly
            // Print tree after insertion
            printf("After inserting %d: ", key);
            printParenthesized(redBlack->root);
            printf("\n");
        } else if (op == 'r') {
            Node *node_to_delete = search(key); // Call search directly
            if (node_to_delete != redBlack->NIL) {
                deleteNode(node_to_delete); // Call deleteNode directly
                // Print tree after deletion
                printf("After deleting %d: ", key);
                printParenthesized(redBlack->root);
                printf("\n");
            } else {
                printf("Node %d not found for deletion.\n", key);
            }
        }
        // else: Invalid operation, ignore or print error
    }

    // Print the final tree nodes in the required format (value, level, color)
    printf("Final In-order Traversal:\n");
    printInOrderWithLevel(redBlack->root, 0);

    // Free the allocated memory
    freeTree(redBlack->root); // Free the actual tree nodes
    free(redBlack->NIL);      // Free the sentinel node
    free(redBlack);           // Free the tree structure itself

    return 0;
}

// --- Function Implementations ---

Node* createNode(int key) {
    Node *newNode = (Node*)malloc(sizeof(Node));
     if (newNode == NULL) {
        fprintf(stderr, "Memory allocation failed for new node.\n");
        // Consider more robust error handling or exiting
        exit(EXIT_FAILURE); // Exit if node allocation fails
    }
    newNode->key = key;
    newNode->color = RED; // New nodes are initially RED
    newNode->left = redBlack->NIL;  // Use global NIL
    newNode->right = redBlack->NIL; // Use global NIL
    newNode->parent = redBlack->NIL;// Use global NIL
    return newNode;
}

// Rotates node x to the left
void leftRotate(Node *x) {
    Node *y = x->right;
    x->right = y->left;
    if (y->left != redBlack->NIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == redBlack->NIL) {
        redBlack->root = y; // Update global root
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
}

// Rotates node y to the right
void rightRotate(Node *y) {
    Node *x = y->left;
    y->left = x->right;
    if (x->right != redBlack->NIL) {
        x->right->parent = y;
    }
    x->parent = y->parent;
    if (y->parent == redBlack->NIL) {
        redBlack->root = x; // Update global root
    } else if (y == y->parent->right) {
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y;
    y->parent = x;
}

// Restores Red-Black properties after insertion
void insertFixup(Node *z) {
    Node *y; // Uncle node
    while (z->parent->color == RED) {
        if (z->parent == z->parent->parent->left) {
            y = z->parent->parent->right; // Uncle
            if (y->color == RED) { // Case 1: Uncle is RED
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else { // Uncle is BLACK
                if (z == z->parent->right) { // Case 2: Triangle
                    z = z->parent;
                    leftRotate(z); // Rotate on parent
                }
                // Case 3: Line
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                rightRotate(z->parent->parent); // Rotate on grandparent
            }
        } else { // Symmetric case: parent is right child
             y = z->parent->parent->left; // Uncle
            if (y->color == RED) { // Case 1: Uncle is RED
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent;
            } else { // Uncle is BLACK
                if (z == z->parent->left) { // Case 2: Triangle
                    z = z->parent;
                    rightRotate(z); // Rotate on parent
                }
                // Case 3: Line
                z->parent->color = BLACK;
                z->parent->parent->color = RED;
                leftRotate(z->parent->parent); // Rotate on grandparent
            }
        }
    }
    redBlack->root->color = BLACK; // Ensure root is always BLACK
}

// Inserts a key into the tree
void insert(int key) {
    Node *z = createNode(key);
    Node *y = redBlack->NIL; // Trailing pointer
    Node *x = redBlack->root; // Start from global root

    // Find position for new node
    while (x != redBlack->NIL) {
        y = x;
        if (z->key < x->key) {
            x = x->left;
        } else {
            // Allow duplicates, place in right subtree
            x = x->right;
        }
    }

    z->parent = y; // Link new node to parent
    if (y == redBlack->NIL) {
        redBlack->root = z; // Tree was empty, z is the new root
    } else if (z->key < y->key) {
        y->left = z;
    } else {
        y->right = z;
    }

    // z->left, z->right, z->color are set by createNode

    insertFixup(z); // Restore Red-Black properties
}

// Replaces subtree rooted at u with subtree rooted at v
void transplant(Node *u, Node *v) {
    if (u->parent == redBlack->NIL) {
        redBlack->root = v; // Update global root
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    // Update v's parent, even if v is NIL
    v->parent = u->parent;
}

// Finds the node with the minimum key in the subtree rooted at x
Node* minimum(Node *x) {
    while (x->left != redBlack->NIL) {
        x = x->left;
    }
    return x;
}

// Finds the node with the maximum key in the subtree rooted at x
Node* maximum(Node *x) {
    while (x->right != redBlack->NIL) {
        x = x->right;
    }
    return x;
}

// Restores Red-Black properties after deletion
void deleteFixup(Node *x) {
    Node *w; // Sibling of x

    while (x != redBlack->root && x->color == BLACK) {
        if (x == x->parent->left) { // x is left child
            w = x->parent->right; // Sibling
            if (w->color == RED) { // Case 1: Sibling is RED
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(x->parent);
                w = x->parent->right; // New sibling must be BLACK
            }
            // Sibling w is now BLACK
            if (w->left->color == BLACK && w->right->color == BLACK) { // Case 2: Sibling's children are BLACK
                w->color = RED;
                x = x->parent; // Move up
            } else {
                if (w->right->color == BLACK) { // Case 3: Sibling's right child is BLACK (left is RED)
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(w);
                    w = x->parent->right; // New sibling
                }
                // Case 4: Sibling's right child is RED
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotate(x->parent);
                x = redBlack->root; // Fixup complete, exit loop
            }
        } else { // Symmetric case: x is right child
            w = x->parent->left; // Sibling
            if (w->color == RED) { // Case 1
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(x->parent);
                w = x->parent->left;
            }
            // Sibling w is now BLACK
            if (w->right->color == BLACK && w->left->color == BLACK) { // Case 2
                w->color = RED;
                x = x->parent;
            } else {
                if (w->left->color == BLACK) { // Case 3
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(w);
                    w = x->parent->left;
                }
                // Case 4
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(x->parent);
                x = redBlack->root; // Fixup complete
            }
        }
    }
    x->color = BLACK; // Ensure the final node (or root) is BLACK
}

// Deletes node z from the tree
void deleteNode(Node *z) {
    Node *y = z; // Node to be spliced out or moved
    Node *x;     // Child that replaces y
    int y_original_color = y->color;

    if (z->left == redBlack->NIL) {
        x = z->right;
        transplant(z, z->right);
    } else if (z->right == redBlack->NIL) {
        x = z->left;
        transplant(z, z->left);
    } else {
        // Node z has two children, find predecessor (max in left subtree)
        y = maximum(z->left);
        y_original_color = y->color;
        x = y->left; // Predecessor's only child is left (or NIL)

        if (y->parent == z) {
            // If y is direct child, x's parent needs careful handling if x is NIL
             if (x != redBlack->NIL) x->parent = y;
             else redBlack->NIL->parent = y; // Ensure NIL parent points correctly for fixup if needed
        } else {
            transplant(y, y->left); // Splice out y
            y->left = z->left;      // Attach z's left subtree to y
            y->left->parent = y;
        }
        transplant(z, y);           // Replace z with y
        y->right = z->right;        // Attach z's right subtree to y
        y->right->parent = y;
        y->color = z->color;        // Give y z's original color

        // If x is NIL and y was not direct child, transplant(y, x) sets NIL->parent correctly
        // If x is NIL and y was direct child, we set NIL->parent above.
    }

    // If the removed/moved node's original color was BLACK, fix properties
    if (y_original_color == BLACK) {
        deleteFixup(x);
    }

    free(z); // Free the memory of the node passed in for deletion
}

// Searches for a node with the given key starting from the root
Node* search(int key) {
    Node *current = redBlack->root;
    while (current != redBlack->NIL && key != current->key) {
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return current; // Returns node or redBlack->NIL if not found
}

// Performs in-order traversal and prints nodes with level and color
void printInOrderWithLevel(Node *node, int level) {
    if (node != redBlack->NIL) {
        printInOrderWithLevel(node->left, level + 1);
        printf("%d,%d,%d\n", node->key, level, node->color);
        printInOrderWithLevel(node->right, level + 1);
    }
}

// Prints the tree in parenthesized notation: key(Color)(Left)(Right)
void printParenthesized(Node *node) {
    // Print node key and color (R for RED, B for BLACK)
    printf("%d%c", node->key, (node->color == RED ? 'R' : 'B'));
    // Print left subtree
    printf("(");
    if(node->left != redBlack->NIL)
        printParenthesized(node->left); // Recurse left
    printf(")");
    // Print right subtree
    printf("(");
    if(node->right != redBlack->NIL)
        printParenthesized(node->right); // Recurse right (fixed bug here)
    printf(")");
}


// Frees the tree memory using post-order traversal
void freeTree(Node *node) {
    if (node == redBlack->NIL || node == NULL) { // Check for NULL just in case, though NIL should be primary check
        return;
    }
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}
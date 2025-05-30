#include <stdio.h>
#include <stdlib.h>

// Define as cores da redblack
#define RED   1
#define BLACK 0

// Estrutura do nó
struct Node {
    int key;
    int color;
    struct Node *parent;
    struct Node *left;
    struct Node *right;
};

// Estrutura da árvore contendo a raiz e o sentinela NIL, como o do livro do Cormen
struct Tree {
    struct Node *NIL; // Nó sentinela (NIL) - Representa todas as folhas nulas
    struct Node *root;
};

// Ponteiro global para a estrutura da Árvore
struct Tree *redBlack;

// Protótipos das Funções
struct Node* createNode(int key);
void leftRotate(struct Node *x); 
void rightRotate(struct Node *y); 
void insertFixup(struct Node *z); 
void insert(int key); 
void transplant(struct Node *u, struct Node *v); 
struct Node* maximum(struct Node *x);
void deleteFixup(struct Node *x); 
void deleteNode(struct Node *z); 
struct Node* search(int key);
void printInOrder(struct Node *node, int level);
void freeTree(struct Node *node);

int main() {
    // Inicializa a estrutura global da Árvore
    redBlack = (struct Tree*)malloc(sizeof(struct Tree));
    if (redBlack == NULL) {
        fprintf(stderr, "Falha na alocação de memória para a Rubro-Negra.\n");
        return 1;
    }

    // Inicializa o T.Nil
    redBlack->NIL = (struct Node*)malloc(sizeof(struct Node));
    if (redBlack->NIL == NULL) {
        fprintf(stderr, "Falha na alocação de memória para o nó NIL.\n");
        free(redBlack); // Libera a estrutura da árvore se a alocação do NIL falhar
        return 1;
    }
    redBlack->NIL->color = BLACK;
    redBlack->NIL->left = redBlack->NIL;
    redBlack->NIL->right = redBlack->NIL;
    redBlack->NIL->parent = redBlack->NIL;

    // Inicializa a raiz como NIL - árvore vazia
    redBlack->root = redBlack->NIL;

    char op;
    int key;

    // Lê operações e chaves da entrada padrão ate parar as entradas
    while (scanf(" %c %d", &op, &key) == 2) {
        if (op == 'i') {
            insert(key); // insere a nova chave
        } else if (op == 'r') {
            struct Node *node_to_delete = search(key); // Procura a chave
            if (node_to_delete != redBlack->NIL) {
                deleteNode(node_to_delete); // Deleta o no, se existir
            } else {
                printf("Nó %d não encontrado.\n", key);
            }
        }
    }

    // Imprime a estrutura da arvore em ordem no formato solicitado 
    printInOrder(redBlack->root, 0);

    // Libera a memória alocada
    freeTree(redBlack->root); // Libera os nós reais da árvore
    free(redBlack->NIL);      // Libera o nó sentinela
    free(redBlack);           // Libera a própria estrutura da árvore
    return 0;
}

// Implementações das Funções 

struct Node* createNode(int key) {
    struct Node *newNode = (struct Node*)malloc(sizeof(struct Node));
     if (newNode == NULL) {
        fprintf(stderr, "Falha na alocação de memória para novo nó.\n");
        exit(EXIT_FAILURE); // Sai se a alocação do nó falhar
    }
    newNode->key = key;
    newNode->color = RED; // Novos nós são inicialmente VERMELHOS, como no Cormen
    newNode->left = redBlack->NIL;  
    newNode->right = redBlack->NIL; 
    newNode->parent = redBlack->NIL;
    return newNode;
}

void leftRotate(struct Node *x) {
    struct Node *y = x->right;
    x->right = y->left; 
    if (y->left != redBlack->NIL) {
        y->left->parent = x;
    }
    y->parent = x->parent; // Liga o pai de x a y, para  jogar depois y para cima
    if (x->parent == redBlack->NIL) { // Caso onde x e raiz
        redBlack->root = y; // Atualiza a raiz global
    } else if (x == x->parent->left) {
        x->parent->left = y;
    } else {
        x->parent->right = y;
    }
    y->left = x; // Coloca x como filho esquerdo de y
    x->parent = y;
}

void rightRotate(struct Node *y) {
    struct Node *x = y->left; 
    y->left = x->right; 
    if (x->right != redBlack->NIL) {
        x->right->parent = y;
    }
    x->parent = y->parent; // Liga o pai de y a x, para  jogar depois x para cima
    if (y->parent == redBlack->NIL) { // Caso onde y e raiz
        redBlack->root = x; // Atualiza a raiz global
    } else if (y == y->parent->right) { 
        y->parent->right = x;
    } else {
        y->parent->left = x;
    }
    x->right = y; // // Coloca y como filho direito de x
    y->parent = x;
}

// Restaura as propriedades Rubro-Negras após a inserção
void insertFixup(struct Node *z) {
    struct Node *y; // Nó tio
    while (z->parent->color == RED) { // Enquanto o pai de z for VERMELHO (violação da propriedade 4)
        if (z->parent == z->parent->parent->left) { // Pai de z é filho esquerdo do avô
            y = z->parent->parent->right; // Tio de z é o filho direito do avô
            if (y->color == RED) { // Caso 1: Tio é VERMELHO
                // Recolore pai, tio e avô
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Move z para o avô para continuar a correção
            } else {
                if (z == z->parent->right) { // Caso 2: z é filho direito do pai
                    z = z->parent; // Move z para o pai
                    leftRotate(z); // Rotaciona à esquerda no pai
                }
                // Caso 3: z é filho esquerdo do pai
                z->parent->color = BLACK; // Recolore o pai para PRETO
                z->parent->parent->color = RED; // Recolore o avô para VERMELHO
                rightRotate(z->parent->parent); // Rotaciona à direita no avô
            }
        } else { // Simétrico: Pai de z é filho direito do avô
            y = z->parent->parent->left; // Tio de z é o filho esquerdo do avô
            if (y->color == RED) { // Caso 1: Tio é VERMELHO
                // Recolore pai, tio e avô
                z->parent->color = BLACK;
                y->color = BLACK;
                z->parent->parent->color = RED;
                z = z->parent->parent; // Move z para o avô para continuar a correção
            } else {
                if (z == z->parent->left) { // Caso 2: z é filho esquerdo do pai
                    z = z->parent; // Move z para o pai
                    rightRotate(z); // Rotaciona à direita no pai
                }
                // Caso 3: z é filho direito do pai
                z->parent->color = BLACK; // Recolore o pai para PRETO
                z->parent->parent->color = RED; // Recolore o avô para VERMELHO
                leftRotate(z->parent->parent); // Rotaciona à esquerda no avô
            }
        }
    }
    redBlack->root->color = BLACK; // Garante que a raiz seja sempre PRETA, de acordo com a propriedade
}

// Insere uma chave na árvore
void insert(int key) {
    struct Node *z = createNode(key);
    struct Node *y = redBlack->NIL; // Ponteiro auxiliar, se tornará o pai de z
    struct Node *x = redBlack->root; // Começa a busca a partir da raiz global

    // Encontra a posição para o novo nó de forma iterativa :)
    while (x != redBlack->NIL) {
        y = x; // Mantém o rastro do pai
        if (z->key < x->key) {
            x = x->left;
        } else {
            // em caso de duplicatas, coloca na subárvore direita
            x = x->right;
        }
    }

    z->parent = y; // Liga o novo nó ao pai
    if (y == redBlack->NIL) {
        redBlack->root = z; // Árvore estava vazia, z é a nova raiz
    } else if (z->key < y->key) {
        y->left = z; 
    } else {
        y->right = z; 
    }

    insertFixup(z); // Restaura as propriedades da Rubro-Negra
}

// Coloca o no v no lugar de u
void transplant(struct Node *u, struct Node *v) {
    if (u->parent == redBlack->NIL) {
        redBlack->root = v; // Atualiza a raiz global
    } else if (u == u->parent->left) {
        u->parent->left = v; 
    } else {
        u->parent->right = v; 
    }
    v->parent = u->parent;
}

// Encontra o nó com a chave máxima na subárvore enraizada em x
struct Node* maximum(struct Node *x) {
    // Percorre para direita ate o fim
    while (x->right != redBlack->NIL) {
        x = x->right;
    }
    return x;
}

// Restaura as propriedades da Rubro-Negra após a remoção
// Essa aqui é complicada
void deleteFixup(struct Node *x) {
    struct Node *w; // Nó irmão de x

    // Loop enquanto x for "duplamente preto" (implicitamente, pois é PRETO e não é a raiz)
    // e x não for a raiz
    while (x != redBlack->root && x->color == BLACK) {
        // Se x é o filho esquerdo
        if (x == x->parent->left) {
            w = x->parent->right; // Irmão w é o filho direito

            // Caso 1: O irmão w é VERMELHO
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                leftRotate(x->parent);
                w = x->parent->right;
            }
            // Agora w é definitivamente PRETO

            // Caso 2: O irmão w é PRETO, e ambos os filhos de w são PRETOS
            if (w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->parent;
            } else {
                // Caso 3: O irmão w é PRETO, o filho esquerdo de w é VERMELHO, o filho direito de w é PRETO
                if (w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotate(w);
                    w = x->parent->right;
                }
                // Caso 4: O irmão w é PRETO, e o filho direito de w é VERMELHO
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->right->color = BLACK;
                leftRotate(x->parent);
                x = redBlack->root;
            }
        } else { // Caso simétrico: x é o filho direito
            w = x->parent->left; // Irmão w é o filho esquerdo

            // Caso 1: O irmão w é VERMELHO
            if (w->color == RED) {
                w->color = BLACK;
                x->parent->color = RED;
                rightRotate(x->parent);
                w = x->parent->left; // Novo irmão deve ser PRETO
            }
            // Agora w é definitivamente PRETO

            // Caso 2: O irmão w é PRETO, e ambos os filhos de w são PRETOS
            if (w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->parent; // Move para cima
            } else {
                // Caso 3: O irmão w é PRETO, o filho direito de w é VERMELHO, o filho esquerdo de w é PRETO
                if (w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotate(w);
                    w = x->parent->left; // Novo irmão
                }
                // Caso 4: O irmão w é PRETO, e o filho esquerdo de w é VERMELHO
                // (O filho direito de w pode ser VERMELHO ou PRETO)
                w->color = x->parent->color;
                x->parent->color = BLACK;
                w->left->color = BLACK;
                rightRotate(x->parent);
                x = redBlack->root; // Correção completa
            }
        }
    }
    // Garante que a "pretura" extra seja absorvida (se x alcançou a raiz ou se tornou VERMELHO)
    x->color = BLACK;
}


// Remove o nó z da árvore usando o antecessor (máximo da subárvore esquerda)
void deleteNode(struct Node *z) {
    struct Node *y = z; // Nó a ser removido ou movido dentro da árvore
    struct Node *x;     // Filho que substitui y
    int y_original_color = y->color; // Armazena a cor de y antes de qualquer mudança

    // Caso 1 & 2: z tem no máximo um filho
    if (z->left == redBlack->NIL) {
        x = z->right; // x é o filho direito (ou NIL)
        transplant(z, z->right); // Substitui z pelo seu filho direito
    } else if (z->right == redBlack->NIL) {
        x = z->left; // x é o filho esquerdo (ou NIL)
        transplant(z, z->left); // Substitui z pelo seu filho esquerdo
    } else {
        // Caso 3: z tem dois filhos
        // Encontra o antecessor (máximo na subárvore esquerda)
        y = maximum(z->left); 
        y_original_color = y->color; // Precisa da cor do nó que *move*
        x = y->left; // O único filho do antecessor é o esquerdo (ou NIL) 
        if (y->parent == z) {
            // Se y é o filho esquerdo direto de z, o pai de x precisa ser tratado com cuidado se x for NIL
            x->parent = y; // Define o pai se x não for NIL
        } else {
            transplant(y, y->left); // Remove y, substituindo-o pelo seu filho esquerdo 
            y->left = z->left;      // Anexa a subárvore esquerda de z a y 
            y->left->parent = y;    
        }
        transplant(z, y);           // Substitui z por y
        y->right = z->right;        // Anexa a subárvore direita de z a y
        y->right->parent = y;       
        y->color = z->color;        // Dá a y a cor original de z
    }

    if (y_original_color == BLACK) {
        deleteFixup(x);
    }

    free(z); 
}


// Busca por um nó com a chave dada a partir da raiz
struct Node* search(int key) {
    struct Node *current = redBlack->root;
    // Percorre a árvore como uma BST padrão
    while (current != redBlack->NIL && key != current->key) {
        if (key < current->key) {
            current = current->left;
        } else {
            current = current->right;
        }
    }
    return current; // Retorna o nó se encontrado, senão redBlack->NIL
}

// Realiza a travessia in-order
void printInOrder(struct Node *node, int level) {
    if (node != redBlack->NIL) {
        // Visita recursivamente a subárvore esquerda
        printInOrder(node->left, level + 1);

        // Imprime o nó atual: valor,nível,cor
        printf("%d,%d,%d\n", node->key, level, node->color);

        // Visita recursivamente a subárvore direita
        printInOrder(node->right, level + 1);
    }
}

// Libera a memória da árvore usando travessia pós-ordem
void freeTree(struct Node *node) {
    // Caso base: Se o nó é o sentinela NIL ou NULL, não faz nada
    if (node == redBlack->NIL || node == NULL) { // Verifica NULL por segurança, mas NIL deve ser a checagem principal
        return;
    }
    // Libera recursivamente as subárvores esquerda e direita
    freeTree(node->left);
    freeTree(node->right);
    // Libera o próprio nó atual
    free(node);
}
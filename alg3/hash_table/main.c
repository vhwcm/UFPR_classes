#include<stdio.h>
#include"ht.h"
int main() {
    // Inicializa a estrutura global da Árvore
    struct Ht* ht = (struct Ht*)malloc(sizeof(struct Ht));
    if (ht == NULL) {
        fprintf(stderr, "Falha na alocação de memória para Hash-Table.\n");
        return 1;
    }

    char op;
    int chave;

    // Lê operações e chaves da entrada padrão até parar as entradas
    while (scanf(" %c %d", &op, &chave) == 2) {
        if (op == 'i') {
            if(inserir(chave,ht) != 0){
            fprint(stderr,"Erro ao inserir chave");
            } // insere a nova chave
        } else if (op == 'r') {
            if (deletar(chave,ht) != 0) {
              fprint(stderr,"Erro ao deletar chave");
            }
        }
    }

    // Imprime e estrutura 
    imprime(ht);

    // Libera a memória alocada
    libera_hash_table(ht);
    return 0;
}


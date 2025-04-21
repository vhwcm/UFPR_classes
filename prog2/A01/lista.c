#include"lista.h"
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include <stdlib.h>

// Cria uma nova lista vazia.
struct lista_t *lista_cria() {
    struct lista_t *n_lista = malloc(sizeof(struct lista_t));
    if (n_lista == NULL) {
        return NULL;  
    }
    //inicioaliza os valores da lista
    n_lista->prim = NULL;
    n_lista->ult = NULL;
    n_lista->tamanho = 0;
    return n_lista;
}

// Destroi todos os itens da lista e libera a memória
struct lista_t *lista_destroi(struct lista_t *lst) {
    if (lst == NULL) 
        return NULL; 

    struct item_t *pont = lst->prim;
    struct item_t *prox;
    while (pont != NULL) {
        prox = pont->prox;
        free(pont);
        pont = prox;
    }
    free(lst);
    lst = NULL;
    return lst;
}

//Insere  um nó na lista
int lista_insere(struct lista_t *lst, int item, int pos) {
    if (!lst)  
        return -1;
    //verifica se a posição é válida
    if (pos > lst->tamanho || pos < -1)
        return -1;

    //cria novo nodo
    struct item_t* pont = malloc(sizeof(struct item_t));
    if (!pont)  
        return -1;
    pont->valor = item;


    if (pos == -1) 
        pos = lst->tamanho;

    if (pos == 0) {
        // Inserir no início
        pont->prox = lst->prim;
        pont->ant = NULL;
        if (lst->prim)
            lst->prim->ant = pont;
        else
            lst->ult = pont;
        lst->prim = pont;
    } else if (pos == lst->tamanho) {
        // Inserir no fim
        pont->prox = NULL;
        pont->ant = lst->ult;
        if (lst->ult)
            lst->ult->prox = pont;
        else
            lst->prim = pont;
        lst->ult = pont;
    } else {
        // Inserir no meio
        struct item_t *current;
        if (pos <= lst->tamanho / 2) {
            current = lst->prim;
            for (int i = 0; i < pos; i++)
                current = current->prox;
        } else {
            current = lst->ult;
            for (int i = lst->tamanho - 1; i > pos; i--)
                current = current->ant;
        }
        pont->prox = current;
        pont->ant = current->ant;
        if (current->ant)
            current->ant->prox = pont;
        current->ant = pont;
    }
    // Incrementa o tamanho da lista e retorna
    return ++(lst->tamanho); 
}


int lista_retira(struct lista_t *lst, int *item, int pos) {
    if (!lst) 
        return -1;

    // Verifica se a lista está vazia ou se a posição é inválida
    if (lst->tamanho == 0 || pos < -1 || pos >= lst->tamanho) 
        return -1;


    struct item_t* pont;
    if (pos == -1) {
        // Remover o último item da lista
        pont = lst->ult;
        *item = pont->valor;
        if (lst->tamanho == 1) {
            lst->prim = NULL;
            lst->ult = NULL;
        } else {
            pont->ant->prox = NULL;
            lst->ult = pont->ant;
        }
        free(pont);
        return --(lst->tamanho);
    }

    if (pos <= lst->tamanho / 2) {
        pont = lst->prim;
        for (int i = 0; i < pos; i++)
            pont = pont->prox;
    } else {
        pont = lst->ult;
        for (int i = lst->tamanho - 1; i > pos; i--)
            pont = pont->ant;
    }

    *item = pont->valor;

    if (pont == lst->prim) {
        // Remover o primeiro item da lista
        lst->prim = pont->prox;
        if (lst->prim)
            lst->prim->ant = NULL;
        else
            lst->ult = NULL;
    } else if (pont == lst->ult) {
        // Remover o último item da lista
        lst->ult = pont->ant;
        if (lst->ult)
            lst->ult->prox = NULL;
        else
            lst->prim = NULL;
    } else {
        // Remover um item no meio da lista
        pont->ant->prox = pont->prox;
        pont->prox->ant = pont->ant;
    }

    free(pont);
    return --(lst->tamanho);
}

//coloca um o valor de uma posição em *item e retorna o tamanho da lista
int lista_consulta(struct lista_t *lst, int *item, int pos) {
    if (!lst)  
        return -1;

    if (lst->tamanho == 0 || pos >= lst->tamanho)  // Verifica se a lista está vazia
        return -1;

    struct item_t* pont;
    if (pos == -1) {
        // Retorna o valor do último item
        *item = lst->ult->valor;
        return lst->tamanho;
    }

    if (pos <= lst->tamanho / 2) {
        pont = lst->prim;
        for (int i = 0; i < pos; i++) 
            pont = pont->prox;
    } else {
        pont = lst->ult;
        for (int i = lst->tamanho - 1; i > pos; i--) 
            pont = pont->ant;
    }

    *item = pont->valor;  //item recebe o valor de determinada posição

    return lst->tamanho;
}

// Informa a posição da 1ª ocorrência do valor indicado na lista.
int lista_procura (struct lista_t *lst, int valor){
    if(!lst) return -1;
    struct item_t* pont = lst->prim;
    int pos = 0;
    //procura até chegar no final e retornar -1 caso não seja encontrado o valor
    while(pont != NULL){
        if(pont->valor == valor){
            return pos;
        }
        pos++;
        pont = pont->prox;
    }
   return -1;
}


int lista_tamanho (struct lista_t *lst){
    if(lst){
        return lst->tamanho;
    }
    return -1;
}

//imprie a lista toda
void lista_imprime(struct lista_t *lst) {
    if (!lst || lst->tamanho == 0) return; 
    
    struct item_t* pont = lst->prim;
    while (pont != NULL) {
        printf("%d", pont->valor);
        pont = pont->prox;
        if (pont != NULL) {
            printf(" ");
        }
    }
}

